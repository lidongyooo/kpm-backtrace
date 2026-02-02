# KPM Backtrace Module

A kernel module for capturing and analyzing stack traces in Android kernel space.

## Installation

Load this KPM module using [Apatch](https://github.com/bmax121/APatch).

## Quick Start

### Monitoring Kernel Logs

Run the following command to monitor kernel messages in real-time:

```bash
adb shell su & dmesg -w | grep "KP "
```
### Filter by UID

```bash
adb shell su & dmesg -w | grep "UID:<TARGET_UID>"
```

## Module Control

### Control Interface
The module provides a control interface for runtime management:
- Stop module: Input `stop` command

- Start module: Input any other char to start

## Output Example
```bash
[ 1112.764645] [-] KP E === User call exit Backtrace (PID:24702 UID:1000) ===
[ 1112.764659] [-] KP E #0 pc=0x74ff4fc348!0xc2348 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.764664] [-] KP E #1 pc=0x5dae5ea410!0x7410 /vendor/bin/time_daemon
[ 1112.764668] [-] KP E #2 pc=0x74ff4a78d0!0x6d8d0 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.764672] [-] KP E #3 pc=0x74ff499d34!0x5fd34 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.796605] [-] KP E === User call exit Backtrace (PID:24361 UID:10159) ===
[ 1112.796620] [-] KP E #0 pc=0x761d9705f4!0x835f4 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.812702] [-] KP E === User call exit Backtrace (PID:24708 UID:10266) ===
[ 1112.812718] [-] KP E #0 pc=0x761d9705f4!0x835f4 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.813864] [-] KP E === User call exit Backtrace (PID:24709 UID:10266) ===
[ 1112.813882] [-] KP E #0 pc=0x761d9705f4!0x835f4 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.818949] [-] KP E === User call exit Backtrace (PID:24711 UID:10266) ===
[ 1112.818960] [-] KP E #0 pc=0x761d9705f4!0x835f4 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.833436] [-] KP E === User call exit_group Backtrace (PID:24706 UID:10265) ===
[ 1112.833464] [-] KP E #0 pc=0x7aa5d10328!0xc2328 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.833475] [-] KP E #1 pc=0x60b5021544!0x36544 /system/bin/toybox
[ 1112.833482] [-] KP E #2 pc=0x60b50215d4!0x365d4 /system/bin/toybox
[ 1112.833488] [-] KP E #3 pc=0x60b501c5a0!0x315a0 /system/bin/toybox
[ 1112.833495] [-] KP E #4 pc=0x60b5048d3c!0x5dd3c /system/bin/toybox
[ 1112.833501] [-] KP E #5 pc=0x60b5023b04!0x38b04 /system/bin/toybox
[ 1112.833507] [-] KP E #6 pc=0x60b5024178!0x39178 /system/bin/toybox
[ 1112.833518] [-] KP E #7 pc=0x7aa5ca428c!0x5628c /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.833524] [-] KP E #8 pc=0x60b501a068!0x2f068 /system/bin/toybox
[ 1112.863261] [-] KP E === User call exit_group Backtrace (PID:24721 UID:10265) ===
[ 1112.863281] [-] KP E #0 pc=0x7ea64f2328!0xc2328 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.863291] [-] KP E #1 pc=0x6225fa9544!0x36544 /system/bin/toybox
[ 1112.863298] [-] KP E #2 pc=0x6225fa95d4!0x365d4 /system/bin/toybox
[ 1112.863305] [-] KP E #3 pc=0x6225fabb08!0x38b08 /system/bin/toybox
[ 1112.863311] [-] KP E #4 pc=0x6225fac178!0x39178 /system/bin/toybox
[ 1112.863319] [-] KP E #5 pc=0x7ea648628c!0x5628c /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.863326] [-] KP E #6 pc=0x6225fa2068!0x2f068 /system/bin/toybox
[ 1112.881617] [-] KP E === User call exit Backtrace (PID:24731 UID:10266) ===
[ 1112.881631] [-] KP E #0 pc=0x761d9705f4!0x835f4 /apex/com.android.runtime/lib64/bionic/libc.so
[ 1112.906175] [-] KP E === User call tgkill Backtrace (PID:24719 UID:10266) ===
[ 1112.906193] [-] KP E #0 pc=0x72e132eecc!0x28eecc /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906201] [-] KP E #1 pc=0x72e11609e8!0xc09e8 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906207] [-] KP E #2 pc=0x72e115e010!0xbe010 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906211] [-] KP E #3 pc=0x72e116b0f8!0xcb0f8 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906214] [-] KP E #4 pc=0x72e11c0a14!0x120a14 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906217] [-] KP E #5 pc=0x72e14459ec!0x3a59ec /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906219] [-] KP E #6 pc=0x72e130908c!0x26908c /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906222] [-] KP E #7 pc=0x72e12f4710!0x254710 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906224] [-] KP E #8 pc=0x72e14a0a24!0x400a24 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906227] [-] KP E #9 pc=0x72e149d960!0x3fd960 /data/app/~~yAfL4M1VUGD6HFuJQOvyOQ==/com.globe.gcash.android-jgaNR3axi3Ngd5KdyYXARQ==/lib/arm64/libloader.so
[ 1112.906234] [-] KP E #10 pc=0x735745b9ec!0x259ec /apex/com.android.art/lib64/libjavacore.so
[ 1112.906240] [-] KP E #11 pc=0x71060f4c!0x12f4c /system/framework/arm64/boot-core-libart.oat
```