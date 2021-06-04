# Syscall 实现原理分析

## 简介

内核提供用户空间程序与内核空间进行交互的一套标准接口，这些接口让用户态程序能受限地访问硬件设备，比如申请系统资源，操作设备读写，创建新进程等。用户空间发生请求，内核空间负责执行，这些接口便是用户空间和内核空间共同识别的桥梁，这里提到两个字“受限”，是由于为了保证内核稳定性，而不能让用户空间程序随意更改系统，必须是内核对外开放的且满足权限的程序才能调用相应接口。

在用户空间和内核空间之间，有一个叫做系统调用（Syscall）中间层，是连接用户态和内核态的桥梁。这样即提高了内核的安全型，也便于移植，只需实现同一套接口即可。Linux系统，用户空间通过向内核空间发出 `Syscall`，产生软中断，从而让程序陷入内核态，执行相应的操作。对于每个系统调用都会有一个对应的系统调用号。

## Syscall 流程 

Syscall 是通过中断方式实现的，ARM 平台上通过 swi 中断来实现系统调用，实现从用户态切换到内核态，发送软中断 swi 时，从异常向量表中可以查看跳转代码。在内核中有相应的异常处理函数来处理对应的异常，SVC_Handler 函数如下所示：

```asm
.global SVC_Handler
.type SVC_Handler, % function
SVC_Handler:
    /* x0 is initial sp */
    mov sp, x0

    msr daifclr, #3  /* enable interrupt */

    bl  rt_thread_self
    bl  lwp_user_setting_save

    ldp x8, x9, [sp, #(CONTEXT_OFFSET_X8)]
    uxtb x0, w8
    cmp x0, #0xfe            /* 判断是否是信号退出功能，如果是则执行相关函数  */ 
    beq lwp_signal_quit
    bl lwp_get_sys_api       /* 获取要执行的系统调用函数 */
    cmp x0, xzr              /* 判断返回的系统调用函数是否为 null，如果为 null 则直接退出 */
    mov x30, x0
    beq svc_exit
    ldp x0, x1, [sp, #(CONTEXT_OFFSET_X0)]     /* 从栈中将函数参数写入寄存器 */
    ldp x2, x3, [sp, #(CONTEXT_OFFSET_X2)]
    ldp x4, x5, [sp, #(CONTEXT_OFFSET_X4)]
    ldp x6, x7, [sp, #(CONTEXT_OFFSET_X6)]
    blr x30        /* 跳转到 x30 内函数的同时将下一条指令的地址（也就是 svc_exit）放入 x30 */
svc_exit:
    msr daifset, #3

    ldp x2, x3, [sp], #0x10  /* SPSR and ELR. */
    msr spsr_el1, x3
    msr elr_el1, x2

    ldp x29, x30, [sp], #0x10
    msr sp_el0, x29
    ldp x28, x29, [sp], #0x10
    msr fpcr, x28
    msr fpsr, x29
    ldp x28, x29, [sp], #0x10
    ldp x26, x27, [sp], #0x10
    ldp x24, x25, [sp], #0x10
    ldp x22, x23, [sp], #0x10
    ldp x20, x21, [sp], #0x10
    ldp x18, x19, [sp], #0x10
    ldp x16, x17, [sp], #0x10
    ldp x14, x15, [sp], #0x10
    ldp x12, x13, [sp], #0x10
    ldp x10, x11, [sp], #0x10
    ldp x8, x9, [sp], #0x10
    add sp, sp, #0x40
    RESTORE_FPU sp
    
```

