# Cortex-A9 内核中线程切换函数说明

## rt_hw_context_switch_to()

只有目标，没有来源线程。

- rt_hw_context_switch_to

```asm
/*
 * void rt_hw_context_switch_to(rt_uint32 to, struct rt_thread *to_thread);
 * r0 --> to (thread stack)
 * r1 --> to_thread
 */
 .globl rt_hw_context_switch_to
rt_hw_context_switch_to:
    ldr sp, [r0]           # 将 r0 中新线程的栈地址放入 sp 寄存器

    b       rt_hw_context_switch_exit # 跳转准备执行上下文切换
```

- rt_hw_context_switch

```asm
/*
 * void rt_hw_context_switch(rt_uint32 from, rt_uint32 to, struct rt_thread *to_thread);
 * r0 --> from (from_thread stack)
 * r1 --> to (to_thread stack)
 * r2 --> to_thread
 */
.globl rt_hw_context_switch
rt_hw_context_switch:
    stmfd   sp!, {lr}           # 将 lr 中的值 push 到栈
    stmfd   sp!, {r0-r12, lr}   # 将相关寄存器的值 push 到栈，这里的 lr 在主动切换时没有用处，只是起到占位的作用，这种写法下，寄存器 index 大在栈中处于栈 offset 较大的位置（栈中高地址的位置）

    mrs r4, cpsr                # 将 cpsr 中的值放入到 r4
    tst lr, #0x01               # 将 lr 中的值与 0x01 相与后查看状态
    orrne r4, r4, #0x20         # 如果结果不为 0，则将 r4 寄存器中的值或上 0x20

    stmfd sp!, {r4}             # 将 r4 的值 push 到栈  

    /* fpu context */
    vmrs r6, fpexc              # 将 fpexc 中的值放入 r6
    tst  r6, #(1<<30)           # 判断 r6 中的值第 30 位是否为 1
    beq 1f                      # 如果不是 1 跳转到 1f
    vstmdb sp!, {d0-d15}        # 将 d0-d15 寄存器 push 到栈
    vstmdb sp!, {d16-d31}       # 将 d16-d31 寄存器 push 到栈
    vmrs r5, fpscr              # 将 fpscr 中的值放入 r5
    stmfd sp!, {r5}             # 将 r5 中的值 push 到栈
1:
    stmfd sp!, {r6}             # 将 r6 中的值 push 到栈

    str sp, [r0]                # 将 sp 中的值放入 [r0] 的地址中
    ldr sp, [r1]                # 将 [r1] 地址中的值（也就是 to 线程的栈地址）放入 sp

    b       rt_hw_context_switch_exit   # 跳转到 rt_hw_context_switch_exit 执行
```

- rt_hw_context_switch_interrupt

```asm
.globl rt_thread_switch_interrupt_flag
.globl rt_interrupt_from_thread
.globl rt_interrupt_to_thread
.globl rt_hw_context_switch_interrupt
rt_hw_context_switch_interrupt:

    ldr r2, =rt_thread_switch_interrupt_flag  # 将 intr flag 的地址放入 r2
    ldr r3, [r2]                              # 将 flag 中的值放入 r3 寄存器
    cmp r3, #1                                # 判断 r3 中的值是否为 1
    beq _reswitch                             # 如果不为 1 则跳转到 _reswitch
    ldr ip, =rt_interrupt_from_thread         # 将 from thread 的值放入 ip(R12) 寄存器
    mov r3, #1              # 设置 r3 寄存器中的值为 1
    str r0, [ip]            # 将 r0 寄存器中的值放入到 rt_interrupt_from_thread
    str r3, [r2]            # 将 r3 寄存器中的值（#1）放入到 rt_thread_switch_interrupt_flag
_reswitch:
    ldr r2, =rt_interrupt_to_thread    # 将 rt_interrupt_to_thread 的地址放入 r2 
    str r1, [r2]                       # 将 r1 中的值放入 rt_interrupt_to_thread      
    bx  lr                             # 返回到 lr 中存放的地址
```

- rt_hw_context_switch_exit

```asm
.global rt_hw_context_switch_exit
rt_hw_context_switch_exit:

/* fpu context */
    ldmfd sp!, {r6}        # 将栈顶的数据放入到 r6 寄存器中               
    vmsr fpexc, r6         # 将 r6 寄存器里的值放入到 fpexc 寄存器中
    tst  r6, #(1<<30)      # 判断 r6 中第 30 位是否为 1
    beq 1f                 # 如果不是 1，则跳转到 1f
    ldmfd sp!, {r5}        # 将栈中的数据 pop 到 r5 寄存器中
    vmsr fpscr, r5         # 将 r5 寄存器中的值放入 fpscr 寄存器中
    vldmia sp!, {d16-d31}  # 将栈中的数据 pop 到 d16-d31
    vldmia sp!, {d0-d15}   # 将栈中的数据 pop 到 d0-d15
1:   

    ldmfd   sp!, {r1}                             # 将栈中的数据放入 r1 寄存器
    msr     spsr_cxsf, r1  /* original mode */    # 将 r1 的内容放入 spsr_scsf
    ldmfd   sp!, {r0-r12,lr,pc}^ /* irq return */ # 将栈中的数据 pop 回相应的寄存器
```

```asm
.align  5
.globl vector_irq
vector_irq:                       # 开始进行处理 irq 中断
    stmfd   sp!, {r0-r12,lr}      # 将 r0-r12 lr push 到栈中
    
    # 这里也需要将 fpu 相关的寄存器压入中断栈，然后再存放到当前线程的栈中

    bl      rt_interrupt_enter
    bl      rt_hw_trap_irq        # 开始处理 irq 中断，这和过程中可能会调用 rt_schedule，如果调用
                                  # 了线程切换函数，就会将 intr flag 置为 1
    bl      rt_interrupt_leave

    @ if rt_thread_switch_interrupt_flag set, jump to
    @ rt_hw_context_switch_interrupt_do and don't return
    ldr     r0, =rt_thread_switch_interrupt_flag  # 将 intr flag 的地址放入到 r0 中
    ldr     r1, [r0]                              # 将 flag 的值放入 r1 寄存器中
    cmp     r1, #1                                # 判断 r1 中的值是否为 1
    beq     rt_hw_context_switch_interrupt_do     # 如果上述判断成立，则跳转到 interrupt_do 

    ldmfd   sp!, {r0-r12,lr}                      # 如果不成立，则 pop 栈到 r0-r12
    subs    pc,  lr, #4                           # 将 lr 的值减 4 放回 pc 中，相当于中断返回

rt_hw_context_switch_interrupt_do:
    mov     r1,  #0         # 清空 r1 寄存器中的值为 0
    str     r1,  [r0]       # 将 r1 中的值放入到 intr flag，也就是将 intr flag 置为 0

    mov     r1, sp          @ r1 point to {r0-r3} in stack  # 将 sp 中的值放入 r1
    add     sp, sp, #4*4                                    # sp = sp + 16 
    ldmfd   sp!, {r4-r12,lr}@ reload saved registers        # 从栈中恢复寄存器
    mrs     r0,  spsr       @ get cpsr of interrupt thread  # 读取 spsr 寄存器到 r0
    sub     r2,  lr, #4     @ save old task's pc to r2      # lr - 4 后存储到 r2

    @ Switch to SVC mode with no interrupt. If the usr mode guest is
    @ interrupted, this will just switch to the stack of kernel space.
    @ save the registers in kernel space won't trigger data abort.
    msr     cpsr_c, #I_Bit|F_Bit|Mode_SVC                   # 设置 cpsr_c 进入 SVC 模式

    stmfd   sp!, {r2}       @ push old task's pc            # 将 r2 中的值压栈
    stmfd   sp!, {r4-r12,lr}@ push old task's lr,r12-r4     # 将寄存器 push 入栈
    ldmfd   r1,  {r1-r4}    @ restore r0-r3 of the interrupt thread
    stmfd   sp!, {r1-r4}    @ push old task's r0-r3         # 将 r1-r4 压栈
    stmfd   sp!, {r0}       @ push old task's cpsr          # 将 r0 的值压栈

#ifdef RT_USING_FPU
    /* fpu context */
    vmrs r6, fpexc                           # 取出 fpexc 寄存器中的值到 r6
    tst  r6, #(1<<30)                        # 判断是否开启了 fpu
    beq 1f                                   # 如果没有开启则跳转到 1
    vstmdb sp!, {d0-d15}                     # 将 d0-d15 压栈
    vstmdb sp!, {d16-d31}                    # 将 d16-d31 压栈
    vmrs r5, fpscr                           # 读取 fpscr 的值到 r5
    stmfd sp!, {r5}                          # 将 r5 寄存器压栈
1:
    stmfd sp!, {r6}                          # 将 r6 压栈
#endif

    ldr     r4,  =rt_interrupt_from_thread   # 将 rt_interrupt_from_thread 变量的地址存入 r4
    ldr     r5,  [r4]                        # 将变量的值存入 r5
    str     sp,  [r5]       @ store sp in preempted tasks's TCB

    ldr     r6,  =rt_interrupt_to_thread
    ldr     r6,  [r6]
    ldr     sp,  [r6]       @ get new task's stack pointer

#ifdef RT_USING_FPU
/* fpu context */
    ldmfd sp!, {r6}
    vmsr fpexc, r6
    tst  r6, #(1<<30)
    beq 1f
    ldmfd sp!, {r5}
    vmsr fpscr, r5
    vldmia sp!, {d16-d31}
    vldmia sp!, {d0-d15}
1:
#endif

    ldmfd   sp!, {r4}       @ pop new task's cpsr to spsr
    msr     spsr_cxsf, r4

    ldmfd   sp!, {r0-r12,lr,pc}^ @ pop new task's r0-r12,lr & pc, copy spsr to cpsr
```

```
ldmfd   sp!, {r0-r12,lr,pc}^
```

这句话最后 ^ 的作用：

1. 如果加载的目标地址有 pc，那么加载目标地址中有 lr 或 sp ，指的是当前状态下的 lr 或 sp。 
2. 如果加载的目标地址没有 pc，此时加载目标的 lr 和 sp 指的是**用户态**的 lr 和 sp。
3. 如果加载的目标地址既没有 lr 也没有 sp, 则 ^ 没有作用。

加了 ^ 之后，在进行 lr 地址返回的同时，也会将 spsr 的状态恢复到系统中 cpsr 寄存器中，也就是同时更改了 CPU 目前所处的状态，例如从中断返回时，将从 IRQ 状态返回 SVC 状态，切换的同时也将打开系统中断。

重点是恢复了 pc 中的值后，程序将跳转到 pc 指向的代码执行，而没有机会在后面再插入代码恢复 CPSR 的状态。