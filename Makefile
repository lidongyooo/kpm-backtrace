TARGET_COMPILE = aarch64-unknown-linux-gnu-

ifndef KP_DIR
    KP_DIR = ../..
endif


CC = $(TARGET_COMPILE)gcc
LD = $(TARGET_COMPILE)ld

INCLUDE_DIRS := . include patch/include linux/include linux/arch/arm64/include linux/tools/arch/arm64/include
INCLUDE_FLAGS := $(foreach dir,$(INCLUDE_DIRS),-I$(KP_DIR)/kernel/$(dir))

objs := backtrace.o

all: backtrace.kpm

backtrace.kpm: ${objs}
	${LD} -r -o $@ ${LDFLAGS} $^

%.o: %.c
	${CC} $(CFLAGS) $(INCLUDE_FLAGS) -mcmodel=large -fno-asynchronous-unwind-tables -fno-unwind-tables -c -O2 -o $@ $<

.PHONY: clean
clean:
	rm -rf *.kpm
	find . -name "*.o" | xargs rm -f
