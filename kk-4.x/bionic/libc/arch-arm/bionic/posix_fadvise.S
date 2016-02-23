
#include <sys/linux-syscalls.h>

#ifndef __NR_arm_fadvise64_64
#define __NR_arm_fadvise64_64		(__NR_SYSCALL_BASE+270)
#endif

    .text
    .type posix_fadvise64, #function
    .globl posix_fadvise64
    .align 4
    .fnstart

posix_fadvise64:
    .save   {r4, r5, r7}
    stmfd   sp!, {r4, r5, r7}
    ldr     r1, [sp, #20]
    ldr     r4, [sp, #12]
    ldr     r5, [sp, #16]
    ldr     r7, =__NR_arm_fadvise64_64
    swi     #0
    ldmfd   sp!, {r4, r5, r7}
    movs    r0, r0
    bxpl    lr
    b       __set_syscall_errno
    .fnend
