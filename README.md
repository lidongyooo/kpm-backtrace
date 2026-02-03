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
[  738.152719] [-] KP E === User call [tgkill] Backtrace (PID:1259 TID:27297 UID:10266) ===
[  738.152720] [-] KP E UID:10266 #0 pc=0x7875546d88!0x55d88 /apex/com.android.runtime/lib64/bionic/libc.so
[  738.152721] [-] KP E UID:10266 #1 pc=0x78847e43fc!0x63fc /apex/com.android.art/lib64/libsigchain.so
[  738.152721] [-] KP E UID:10266 #2 pc=0x788a62e898!0x898 [vdso]
[  738.218325] [-] KP E === User call [exit_group] Backtrace (PID:27266 TID:27584 UID:10266) ===
[  738.218330] [-] KP E UID:10266 #0 pc=0x788a6ea198!0xbb198 /apex/com.android.runtime/bin/linker64
[  738.218330] [-] KP E UID:10266 #1 pc=0x788a71910c!0xea10c /apex/com.android.runtime/bin/linker64
[  738.218331] [-] KP E UID:10266 #2 pc=0x788a6ca8fc!0x9b8fc /apex/com.android.runtime/bin/linker64
[  738.218351] [-] KP E === User call [exit_group] Backtrace (PID:27266 TID:27581 UID:10266) ===
[  738.218353] [-] KP E UID:10266 #0 pc=0x75cd77a328!0xc2328 /apex/com.android.runtime/lib64/bionic/libc.so
[  738.218354] [-] KP E UID:10266 #1 pc=0x598d6223e8!0x3a3e8 /apex/com.android.runtime/bin/crash_dump64
[  738.218355] [-] KP E UID:10266 #2 pc=0x75cd70e28c!0x5628c /apex/com.android.runtime/lib64/bionic/libc.so
[  738.218355] [-] KP E UID:10266 #3 pc=0x598d605068!0x1d068 /apex/com.android.runtime/bin/crash_dump64
[  738.500996] [-] KP E === User call [rt_tgsigqueueinfo] Backtrace (PID:1259 TID:27297 UID:10266) ===
[  738.501001] [-] KP E UID:10266 #0 pc=0x788a6d8960!0xa9960 /apex/com.android.runtime/bin/linker64
[  738.501002] [-] KP E UID:10266 #1 pc=0x788a718558!0xe9558 /apex/com.android.runtime/bin/linker64
[  738.501003] [-] KP E UID:10266 #2 pc=0x750d556fb0!0x43fb0 /data/app/~~DY9oIHvQfRyFxFMvUogJRA==/com.xxx.xxxx-M17fgyp7qQe28quxKwuA9w==/lib/arm64/libBaiduMapSDK_base_v7_6_5.so
[  738.501004] [-] KP E UID:10266 #3 pc=0x78847e43fc!0x63fc /apex/com.android.art/lib64/libsigchain.so
[  738.501005] [-] KP E UID:10266 #4 pc=0x788a62e898!0x898 [vdso]
[  738.501008] [-] KP E === User call [exit_group] Backtrace (PID:1 TID:27583 UID:10266) ===
[  738.501010] [-] KP E UID:10266 #0 pc=0x75cd77a328!0xc2328 /apex/com.android.runtime/lib64/bionic/libc.so
[  738.501011] [-] KP E UID:10266 #1 pc=0x75cd719ee4!0x61ee4 /apex/com.android.runtime/lib64/bionic/libc.so
[  738.501011] [-] KP E UID:10266 #2 pc=0x75cd70e290!0x56290 /apex/com.android.runtime/lib64/bionic/libc.so
[  738.501012] [-] KP E UID:10266 #3 pc=0x598d605068!0x1d068 /apex/com.android.runtime/bin/crash_dump64
[  738.501897] [-] KP E === User call [exit_group] Backtrace (PID:1 TID:27585 UID:10266) ===
[  738.501899] [-] KP E UID:10266 #0 pc=0x788a6ea198!0xbb198 /apex/com.android.runtime/bin/linker64
[  738.501900] [-] KP E UID:10266 #1 pc=0x788a71910c!0xea10c /apex/com.android.runtime/bin/linker64
[  738.501901] [-] KP E UID:10266 #2 pc=0x788a6ca8fc!0x9b8fc /apex/com.android.runtime/bin/linker64
```