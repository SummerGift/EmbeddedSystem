# Zephyr OS 基础

## 安全机制分析

参考如下文档：

https://www.cnblogs.com/fozu/p/16037954.html

## 中断嵌套可能导致的 bug

在 Zephyr 操作系统中，线程环境下发生中断会先保存上下文到 SYS stack，参考代码 zephyr/arch/arm/core/aarch32/isr_wrapper.S：

```asm
    sub lr, lr, #4
    srsdb #MODE_SYS!
    cps #MODE_SYS
    push {r0-r3, r12, lr}
```

最后在中断退出时再从 sys 栈中恢复上下文，参考代码：zephyr/arch/arm/core/aarch32/cortex_a_r/exc_exit.S：

```asm
 /*
 *void myISR(void)
 * {
 *  printk("in %s\n", __FUNCTION__);
 *  doStuff();
 *  z_arm_int_exit();
 * }
 */
SECTION_SUBSEC_FUNC(TEXT, _HandlerModeExit, z_arm_int_exit)

#ifdef CONFIG_PREEMPT_ENABLED
    /* Do not context switch if exiting a nested interrupt */
    ldr r3, =_kernel
    ldr r0, [r3, #_kernel_offset_to_nested]
    cmp r0, #1
    bhi __EXIT_INT

    ldr r1, [r3, #_kernel_offset_to_current]
    ldr r0, [r3, #_kernel_offset_to_ready_q_cache]
    cmp r0, r1
    blne z_arm_pendsv
__EXIT_INT:
#endif /* CONFIG_PREEMPT_ENABLED */

#ifdef CONFIG_STACK_SENTINEL
    bl z_check_stack_sentinel
#endif /* CONFIG_STACK_SENTINEL */

    /* Disable nested interrupts while exiting */
    cpsid i

    /* Decrement interrupt nesting count */
    ldr r2, =_kernel
    ldr r0, [r2, #_kernel_offset_to_nested]
    sub r0, r0, #1
    str r0, [r2, #_kernel_offset_to_nested]

    /* Restore previous stack pointer */
    pop {r2, r3}
    add sp, sp, r3

    /*
     * Restore lr_svc stored into the SVC mode stack by the mode entry
     * function. This ensures that the return address of the interrupted
     * context is preserved in case of interrupt nesting.
     */
    pop {lr}

    /*
     * Restore r0-r3, r12 and lr_irq stored into the process stack by the
     * mode entry function. These registers are saved by _isr_wrapper for
     * IRQ mode and z_arm_svc for SVC mode.
     *
     * r0-r3 are either the values from the thread before it was switched
     * out or they are the args to _new_thread for a new thread.
     */
    cps #MODE_SYS
    pop {r0-r3, r12, lr}
    userspace_exc_exit
    rfeia sp!

```

在这种情况下，如果 A 中断正常 eoi 结束，此时 A 中断不停再次触发，会导致多次向 sys stack （任务栈）中压栈，最终导致栈溢出，出现一些莫名其妙的故障。