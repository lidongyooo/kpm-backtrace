#include <log.h>
#include <compiler.h>
#include <kpmodule.h>
#include <hook.h>
#include <kputils.h>
#include <linux/printk.h>
#include <linux/errno.h>
#include <linux/uaccess.h>
#include <syscall.h>
#include <asm/current.h>
#include <asm/ptrace.h>
#include "linux/include/linux/string.h"
#include <linux/err.h>
#include "linux/arch/arm64/include/asm/processor.h"
#include "linux/include/linux/sched.h"
#include "linux/include/linux/mm_types.h"


KPM_NAME("kpm-backtrace");
KPM_VERSION("1.0.0");
KPM_LICENSE("GPL v2");
KPM_AUTHOR("lidongyooo");
KPM_DESCRIPTION("Traceback the user stack in kernel");

struct seq_file {
    char *buf;
    size_t size;
    size_t from;
    size_t count;
    size_t pad;
    loff_t index;
    loff_t read_pos;
    u64 version;
    void *lock[4];
    const void *op;
    int poll_event;
    const void *file;
    void *private;
};

typedef void* (*find_memdup_user)(const void *src, size_t len);
typedef void (*find_kfree)(const void* ptr);
typedef unsigned long (*find_copy_from_user)(void *to, const void *from, unsigned long n);
typedef struct vm_area_struct* (*find_find_vma)(struct mm_struct *, unsigned long);
typedef struct mm_struct* (*find_get_task_mm)(struct task_struct*);
typedef void (*find_mmput)(struct mm_struct *);
typedef pid_t (*find_task_pid_nr_ns)(struct task_struct *task, int type, void *ns);
typedef int (*find_show_map_vma)(struct seq_file *m, struct vm_area_struct *vma);

static find_memdup_user got_memdup_user = NULL;
static find_kfree got_kfree = NULL;
static find_copy_from_user got_copy_from_user = NULL;
static find_find_vma got_find_vma = NULL;
static find_get_task_mm got_get_task_mm = NULL;
static find_mmput got_mmput = NULL;
static find_task_pid_nr_ns got_task_pid_nr_ns = NULL;
static find_show_map_vma got_show_map_vma = NULL;
static long (*probe_read)(void *dst, const void *src, size_t size) = NULL;

struct exit_type {
    const char* name;
    int no;
    int args;
};

static struct exit_type exit_types[] = {
    {"exit", __NR_exit, 1},
    {"exit_group", __NR_exit_group, 1},
    {"rt_sigqueueinfo", __NR_rt_sigqueueinfo, 3},      // 发送致命信号导致退出
    {"kill", __NR_kill, 2},                            // 进程终止信号
    {"tgkill", __NR_tgkill, 3},                        // 线程组终止
    {"tkill", __NR_tkill, 2},                          // 线程终止
    {"rt_tgsigqueueinfo", __NR_rt_tgsigqueueinfo, 4}  // 发送线程组信号
};


static bool install_successful = false;
static char* control_status = "start";

static inline bool is_user_range(uint64_t ptr)
{
    return ptr > 0x1000 && ptr < 0xFFFFFFFF00;
}

static inline uint64_t strip_pac(uint64_t ptr) {
    uint64_t result;
    asm volatile(
        ".arch armv8.3-a\n\t"
        "xpaci %0"
        : "=r" (result)
        : "0" (ptr)
    );
    return result;
}


static int safe_read_user_stack(uint64_t user_fp, uint64_t *out_fp, uint64_t *out_pc) {
    uint64_t frame[2];

    if (!got_memdup_user) return -1;

    void *data = got_memdup_user((void __user *)user_fp, sizeof(frame));
    if (IS_ERR(data)) {
        return PTR_ERR(data);
    }
    memcpy(frame, data, sizeof(frame));
    got_kfree(data);

    *out_fp = frame[0];
    *out_pc = frame[1] > 0xFFFFFFFF00 ? strip_pac(frame[1]) : frame[1];

    if (!is_user_range(*out_fp) || !is_user_range(*out_pc)) {
        return -EINVAL;
    }

    return 0;
}


static void get_vma_path(struct vm_area_struct *vma, char* path, int path_len) {
    char buf[512];
    struct seq_file m;
    memset(&m, 0, sizeof(m));
    m.buf = buf;
    m.size = sizeof(buf);

    if (path) path[0] = 0;

    if (got_show_map_vma) {
        got_show_map_vma(&m, vma);
        if (m.count < sizeof(buf)) {
            buf[m.count] = 0;

            char *p = buf;

            for (int i = 0; i < 5; i++) {
                while (*p && *p != ' ') p++;
                while (*p && *p == ' ') p++;
            }

            if (*p && *p != '\n') {
                char *nl = strchr(p, '\n');
                if (nl) *nl = 0;

                int len = strlen(p);
                while (len > 0 && p[len-1] == ' ') {
                    p[len-1] = 0;
                    len--;
                }

                if (p[0] != 0 && path) {
                    strlcpy(path, p, path_len);
                }
            }
        }
    } else {
        logke("show_map_vma not found");
    }
}

static unsigned long get_lib_base(struct mm_struct *mm, struct vm_area_struct *vma, unsigned long vm_start, const char *path) {
    unsigned long base = vm_start;
    struct vm_area_struct *curr_vma = vma;
    char prev_path[256];

    for (int i = 0; i < 5; i++) {
        struct vm_area_struct *prev = got_find_vma(mm, base - 1);

        if (!prev || prev == curr_vma) break;

        unsigned long prev_end = 0;
        if (probe_read(&prev_end, (char*)prev + 8, sizeof(prev_end))) break;

        if (prev_end != base) break;

        memset(prev_path, 0, sizeof(prev_path));
        get_vma_path(prev, prev_path, sizeof(prev_path));

        if (strcmp(path, prev_path) != 0) break;

        if (probe_read(&base, prev, sizeof(base))) break;
        curr_vma = prev;
    }

    return base;
}


void before_exit(hook_fargs4_t *args, void *udata) {
    if (strcmp(control_status, "start") != 0) {
        return;
    }

    char* type_name = udata;
    int cu_uid = current_uid();
    if (cu_uid <= 0) {
        return;
    }

     if (strcmp(type_name, "rt_sigqueueinfo") == 0 ||
        strcmp(type_name, "kill") == 0 ||
        strcmp(type_name, "tgkill") == 0 ||
        strcmp(type_name, "tkill") == 0 ||
        strcmp(type_name, "rt_tgsigqueueinfo") == 0) {

        int sig = -1;
        if (strcmp(type_name, "kill") == 0 || strcmp(type_name, "tkill") == 0) {
            sig = args->arg1;
        } else if (strcmp(type_name, "tgkill") == 0) {
            sig = args->arg2;
        } else if (strcmp(type_name, "rt_sigqueueinfo") == 0) {
            sig = args->arg1;
        } else if (strcmp(type_name, "rt_tgsigqueueinfo") == 0) {
            sig = args->arg2;
        }

        if (sig > 0) {
            int fatal_signals[] = {
                2,  // SIGINT
                3,  // SIGQUIT
                6,  // SIGABRT
                14, // SIGALRM
                9,  // SIGKILL
                15, // SIGTERM
            };

            int is_fatal = 0;
            for (size_t i = 0; i < sizeof(fatal_signals)/sizeof(fatal_signals[0]); i++) {
                if (sig == fatal_signals[i]) {
                    is_fatal = 1;
                    break;
                }
            }

            if (!is_fatal) {
                return;
            }
        }
    }

    struct task_struct* task = current;
    struct pt_regs* user_regs = _task_pt_reg(task);

    if (!user_regs) {
        logke("No user regs");
        return;
    }

    struct mm_struct *mm = NULL;
    if (got_get_task_mm) {
        mm = got_get_task_mm(task);
    }

    if (!mm) {
        logke("No mm");
        return;
    }

    pid_t pid = -1;
    if (got_task_pid_nr_ns) {
        pid = got_task_pid_nr_ns(task, PIDTYPE_PID, NULL);
    }

    char path_buf[256];
    uint64_t fp = user_regs->regs[29];
    uint64_t pc = user_regs->pc;

    logke("=== User call %s Backtrace (PID:%d UID:%d) ===", type_name, pid, cu_uid);
    int depth = 0;
    while (is_user_range(fp) && is_user_range(pc) && depth < 32) {
        struct vm_area_struct *vma = got_find_vma(mm, pc);
        unsigned long vm_start = *(uint64_t*)vma;
        memset(path_buf, 0, sizeof(path_buf));
        if (vma) {
            get_vma_path(vma, path_buf, sizeof(path_buf));
            if (vm_start != 0 && path_buf[0] != 0 && path_buf[0] != '[') {
                vm_start = get_lib_base(mm, vma, vm_start, path_buf);
            }
        }

        // 计算相对偏移
        unsigned long offset = 0;
        if (vm_start != 0 && pc >= vm_start) {
            offset = pc - vm_start;
        } else {
            offset = pc;
        }

        logke("UID:%d #%d pc=0x%llx!0x%lx %s", cu_uid, depth, pc - 4, offset - 4, path_buf);

        // 读取下一帧
        uint64_t next_fp, next_pc;
        if (safe_read_user_stack(fp, &next_fp, &next_pc) != 0) {
            break;
        }

        fp = next_fp;
        pc = next_pc;
        depth++;
    }

    if (got_mmput) {
        got_mmput(mm);
    }
}


bool init() {
    int counter = sizeof(exit_types) / sizeof(exit_types[0]);
    bool flag = true;
    for (int i = 0; i < counter; i++) {
        struct exit_type _exit_type = exit_types[i];
        hook_err_t err = fp_hook_syscalln(_exit_type.no, _exit_type.args, before_exit, 0, (void*)_exit_type.name);
        if (err == HOOK_NO_ERR) {
            continue;
        }

        err = fp_hook_compat_syscalln(_exit_type.no, _exit_type.args, before_exit, 0, (void*)_exit_type.name);
        if (err != HOOK_NO_ERR) {
            flag = false;
            break;
        }
    }

    if (flag == false) {
        return false;
    }

    got_memdup_user = (find_memdup_user)kallsyms_lookup_name("memdup_user");
    got_kfree = (find_kfree)kallsyms_lookup_name("kfree");
    got_copy_from_user = (find_copy_from_user)kallsyms_lookup_name("copy_from_user");
    got_find_vma = (find_find_vma)kallsyms_lookup_name("find_vma");
    got_get_task_mm = (find_get_task_mm)kallsyms_lookup_name("get_task_mm");
    got_mmput = (find_mmput)kallsyms_lookup_name("mmput");
    got_task_pid_nr_ns = (find_task_pid_nr_ns)kallsyms_lookup_name("__task_pid_nr_ns");
    got_show_map_vma = (find_show_map_vma)kallsyms_lookup_name("show_map_vma");

    probe_read = (void *)kallsyms_lookup_name("copy_from_kernel_nofault");
    if (!probe_read) {
        probe_read = (void *)kallsyms_lookup_name("probe_kernel_read");
    }

    return true;
}


static long install(const char *args, const char *event, void *__user reserved)
{
    install_successful = init();
    if (!install_successful) {
        logke("backtrace module install fail. not support syscall hook. check your kernel version or post issues.");
    } else {
        logke("backtrace module install successful.");
    }

    return 0;
}

static long control(const char *args, char *__user out_msg, int outlen)
{
    if (strcmp(args, "stop") == 0) {
        control_status = "stop";
    } else {
        control_status = "start";
    }

    return 0;
}

static long uninstall(void *__user reserved)
{
    if (install_successful) {
        int counter = sizeof(exit_types) / sizeof(exit_types[0]);
        for (int i = 0; i < counter; i++) {
            struct exit_type _exit_type = exit_types[i];
            fp_unhook_syscalln(_exit_type.no, before_exit, 0);
            fp_unhook_compat_syscalln(_exit_type.no, before_exit, 0);
        }
    }

    logke("uninstall successful\n");
    return 0;
}

KPM_INIT(install);
KPM_CTL0(control);
KPM_EXIT(uninstall);