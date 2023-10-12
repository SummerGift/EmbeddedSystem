# Zephyr OS 基础

## Zephyr 安全机制

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
    rfeia sp!  # 从任务栈中恢复 CPSR 寄存器的值及 PC 寄存器的值，即会恢复到任务接下来要执行的指令地址

```

在这种情况下，如果 A 中断正常 eoi 结束，此时 A 中断不停再次触发，会导致多次向 sys stack （任务栈）中压栈，最终导致栈溢出，出现一些莫名其妙的故障。

## zephyr aarch32 汇编分析

### 中断向量表

```asm

SECTION_SUBSEC_FUNC(exc_vector_table,_vector_table_section,_vector_table)
	ldr pc, =z_arm_reset             /*                   offset 0 */
	ldr pc, =z_arm_undef_instruction /* undef instruction offset 4 */
	ldr pc, =z_arm_svc               /* svc               offset 8 */
	ldr pc, =z_arm_prefetch_abort    /* prefetch abort offset  0xc */
	ldr pc, =z_arm_data_abort        /* data abort     offset 0x10 */
	nop				                 /*                offset 0x14 */
#ifdef CONFIG_GEN_SW_ISR_TABLE
	ldr pc, =_isr_wrapper		     /* IRQ            offset 0x18 */
#else
	ldr pc, =z_irq_spurious
#endif
	ldr pc, =z_arm_nmi               /* FIQ            offset 0x1c */

```

### reset 启动

```asm
/**
 *
 * @brief Reset vector
 *
 * Ran when the system comes out of reset. The processor is in Supervisor mode
 * and interrupts are disabled. The processor architectural registers are in
 * an indeterminate state.
 *
 * When these steps are completed, jump to z_arm_prep_c(), which will finish
 * setting up the system for running C code.
 *
 * @return N/A
 */
SECTION_SUBSEC_FUNC(TEXT, _reset_section, z_arm_reset)
SECTION_SUBSEC_FUNC(TEXT, _reset_section, __start)
#if defined(CONFIG_AARCH32_ARMV8_R)
    /* Check if we are starting in HYP mode */
    mrs r0, cpsr
    and r0, r0, #MODE_MASK
    cmp r0, #MODE_HYP
    bne EL1_Reset_Handler

    /* Init HSCTLR see Armv8-R AArch32 architecture profile */
    ldr r0, =(HSCTLR_RES1 | SCTLR_I_BIT | SCTLR_C_BIT)
    mcr p15, 4, r0, c1, c0, 0

    /* Init HACTLR: Enable EL1 access to all IMP DEF registers */
    ldr r0, =HACTLR_INIT
    mcr p15, 4, r0, c1, c0, 1

   /* Go to SVC mode */
    mrs r0, cpsr
    bic r0, #MODE_MASK
    orr r0, #MODE_SVC
    msr spsr_cxsf, r0

    ldr r0, =EL1_Reset_Handler
    msr elr_hyp, r0       # 设置异常等级切换后的
    dsb
    isb
    eret

EL1_Reset_Handler:

#endif
#if defined(CONFIG_CPU_HAS_DCLS)
    /*
     * Initialise CPU registers to a defined state if the processor is
     * configured as Dual-redundant Core Lock-step (DCLS). This is required
     * for state convergence of the two parallel executing cores.
     */

    /* Common and SVC mode registers */
    mov r0,  #0
    mov r1,  #0
    mov r2,  #0
    mov r3,  #0
    mov r4,  #0
    mov r5,  #0
    mov r6,  #0
    mov r7,  #0
    mov r8,  #0
    mov r9,  #0
    mov r10, #0
    mov r11, #0
    mov r12, #0
    mov r13, #0         /* r13_svc */
    mov r14, #0         /* r14_svc */
    mrs r0,  cpsr
    msr spsr_cxsf, r0   /* spsr_svc */

    /* FIQ mode registers */
    cps #MODE_FIQ
    mov r8,  #0         /* r8_fiq */
    mov r9,  #0         /* r9_fiq */
    mov r10, #0         /* r10_fiq */
    mov r11, #0         /* r11_fiq */
    mov r12, #0         /* r12_fiq */
    mov r13, #0         /* r13_fiq */
    mov r14, #0         /* r14_fiq */
    mrs r0,  cpsr
    msr spsr_cxsf, r0   /* spsr_fiq */

    /* IRQ mode registers */
    cps #MODE_IRQ
    mov r13, #0         /* r13_irq */
    mov r14, #0         /* r14_irq */
    mrs r0,  cpsr
    msr spsr_cxsf, r0   /* spsr_irq */

    /* ABT mode registers */
    cps #MODE_ABT
    mov r13, #0         /* r13_abt */
    mov r14, #0         /* r14_abt */
    mrs r0,  cpsr
    msr spsr_cxsf, r0   /* spsr_abt */

    /* UND mode registers */
    cps #MODE_UND
    mov r13, #0         /* r13_und */
    mov r14, #0         /* r14_und */
    mrs r0,  cpsr
    msr spsr_cxsf, r0   /* spsr_und */

    /* SYS mode registers */
    cps #MODE_SYS
    mov r13, #0         /* r13_sys */
    mov r14, #0         /* r14_sys */

#if defined(CONFIG_FPU)
    /*
     * Initialise FPU registers to a defined state.
     */

    /* Allow VFP coprocessor access */
    mrc p15, 0, r0, c1, c0, 2
    orr r0, r0, #(CPACR_CP10(CPACR_FA) | CPACR_CP11(CPACR_FA))
    mcr p15, 0, r0, c1, c0, 2

    /* Enable VFP */
    mov  r0, #FPEXC_EN
    fmxr fpexc, r0

    /* Initialise VFP registers */
    fmdrr d0,  r1, r1
    fmdrr d1,  r1, r1
    fmdrr d2,  r1, r1
    fmdrr d3,  r1, r1
    fmdrr d4,  r1, r1
    fmdrr d5,  r1, r1
    fmdrr d6,  r1, r1
    fmdrr d7,  r1, r1
    fmdrr d8,  r1, r1
    fmdrr d9,  r1, r1
    fmdrr d10, r1, r1
    fmdrr d11, r1, r1
    fmdrr d12, r1, r1
    fmdrr d13, r1, r1
    fmdrr d14, r1, r1
    fmdrr d15, r1, r1
#endif /* CONFIG_FPU */

#endif /* CONFIG_CPU_HAS_DCLS */

    /*
     * Configure stack.
     */

    /* FIQ mode stack */
    msr CPSR_c, #(MODE_FIQ | I_BIT | F_BIT)
    ldr sp, =(z_arm_fiq_stack + CONFIG_ARMV7_FIQ_STACK_SIZE)

    /* IRQ mode stack */
    msr CPSR_c, #(MODE_IRQ | I_BIT | F_BIT)
    ldr sp, =(z_interrupt_stacks + CONFIG_ISR_STACK_SIZE)

    /* ABT mode stack */
    msr CPSR_c, #(MODE_ABT | I_BIT | F_BIT)
    ldr sp, =(z_arm_abort_stack + CONFIG_ARMV7_EXCEPTION_STACK_SIZE)

    /* UND mode stack */
    msr CPSR_c, #(MODE_UND | I_BIT | F_BIT)
    ldr sp, =(z_arm_undef_stack + CONFIG_ARMV7_EXCEPTION_STACK_SIZE)

    /* SVC mode stack */
    msr CPSR_c, #(MODE_SVC | I_BIT | F_BIT)
    ldr sp, =(z_arm_svc_stack + CONFIG_ARMV7_SVC_STACK_SIZE)

    /* SYS mode stack */
    msr CPSR_c, #(MODE_SYS | I_BIT | F_BIT)
    ldr sp, =(z_arm_sys_stack + CONFIG_ARMV7_SYS_STACK_SIZE)

#if defined(CONFIG_PLATFORM_SPECIFIC_INIT)
    /* Execute platform-specific initialisation if applicable */
    bl z_arm_platform_init
#endif

#if defined(CONFIG_WDOG_INIT)
    /* board-specific watchdog initialization is necessary */
    bl z_arm_watchdog_init
#endif

#if defined(CONFIG_DISABLE_TCM_ECC)
    bl z_arm_tcm_disable_ecc
#endif

    b z_arm_prep_c
```

### 线程栈构造

```asm
/* An initial context, to be "restored" by z_arm_pendsv(), is put at the other
 * end of the stack, and thus reusable by the stack when not needed anymore.
 *
 * The initial context is an exception stack frame (ESF) since exiting the
 * PendSV exception will want to pop an ESF. Interestingly, even if the lsb of
 * an instruction address to jump to must always be set since the CPU always
 * runs in thumb mode, the ESF expects the real address of the instruction,
 * with the lsb *not* set (instructions are always aligned on 16 bit
 * halfwords).  Since the compiler automatically sets the lsb of function
 * addresses, we have to unset it manually before storing it in the 'pc' field
 * of the ESF.
 */
void arch_new_thread(struct k_thread *thread, k_thread_stack_t *stack,
		     char *stack_ptr, k_thread_entry_t entry,
		     void *p1, void *p2, void *p3)
{
	struct __basic_sf *iframe;

#ifdef CONFIG_MPU_STACK_GUARD
#if defined(CONFIG_USERSPACE)
	if (z_stack_is_user_capable(stack)) {
		/* Guard area is carved-out of the buffer instead of reserved
		 * for stacks that can host user threads
		 */
		thread->stack_info.start += MPU_GUARD_ALIGN_AND_SIZE;
		thread->stack_info.size -= MPU_GUARD_ALIGN_AND_SIZE;
	}
#endif /* CONFIG_USERSPACE */
#if FP_GUARD_EXTRA_SIZE > 0
	if ((thread->base.user_options & K_FP_REGS) != 0) {
		/* Larger guard needed due to lazy stacking of FP regs may
		 * overshoot the guard area without writing anything. We
		 * carve it out of the stack buffer as-needed instead of
		 * unconditionally reserving it.
		 */
		thread->stack_info.start += FP_GUARD_EXTRA_SIZE;
		thread->stack_info.size -= FP_GUARD_EXTRA_SIZE;
	}
#endif /* FP_GUARD_EXTRA_SIZE */
#endif /* CONFIG_MPU_STACK_GUARD */

	iframe = Z_STACK_PTR_TO_FRAME(struct __basic_sf, stack_ptr);
#if defined(CONFIG_USERSPACE)
	if ((thread->base.user_options & K_USER) != 0) {
		iframe->pc = (uint32_t)arch_user_mode_enter;
	} else {
		iframe->pc = (uint32_t)z_thread_entry;
	}
#else
	iframe->pc = (uint32_t)z_thread_entry;
#endif

#if defined(CONFIG_CPU_CORTEX_M)
	/* force ARM mode by clearing LSB of address */
	iframe->pc &= 0xfffffffe;
#endif
	iframe->a1 = (uint32_t)entry;
	iframe->a2 = (uint32_t)p1;
	iframe->a3 = (uint32_t)p2;
	iframe->a4 = (uint32_t)p3;

#if defined(CONFIG_CPU_CORTEX_M)
	iframe->xpsr =
		0x01000000UL; /* clear all, thumb bit is 1, even if RO */
#else
	iframe->xpsr = A_BIT | MODE_SYS;   # 这里只屏蔽了 Asynchronous abort，相当于解除了 IRQ 和 FIQ 的屏蔽
#if defined(CONFIG_COMPILER_ISA_THUMB2)
	iframe->xpsr |= T_BIT;
#endif /* CONFIG_COMPILER_ISA_THUMB2 */
#endif /* CONFIG_CPU_CORTEX_M */

	thread->callee_saved.psp = (uint32_t)iframe;
	thread->arch.basepri = 0;

#if defined(CONFIG_ARM_STORE_EXC_RETURN) || defined(CONFIG_USERSPACE)
	thread->arch.mode = 0;
#if defined(CONFIG_ARM_STORE_EXC_RETURN)
	thread->arch.mode_exc_return = DEFAULT_EXC_RETURN;
#endif
#if FP_GUARD_EXTRA_SIZE > 0
	if ((thread->base.user_options & K_FP_REGS) != 0) {
		thread->arch.mode |= Z_ARM_MODE_MPU_GUARD_FLOAT_Msk;
	}
#endif
#if defined(CONFIG_USERSPACE)
	thread->arch.priv_stack_start = 0;
#endif
#endif
	/*
	 * initial values in all other registers/thread entries are
	 * irrelevant.
	 */
}
```

### 中断进入

```asm
SECTION_FUNC(TEXT, _isr_wrapper)

#if defined(CONFIG_CPU_CORTEX_M)
	push {r0,lr}		/* r0, lr are now the first items on the stack */
#elif defined(CONFIG_CPU_AARCH32_CORTEX_R)

#if defined(CONFIG_USERSPACE)
	/* See comment below about svc stack usage */
	cps #MODE_SVC
	push {r0}

	/* Determine if interrupted thread was in user context */
	cps #MODE_IRQ
	mrs r0, spsr
	and r0, #MODE_MASK
	cmp r0, #MODE_USR
	bne isr_system_thread

	ldr r0, =_kernel
	ldr r0, [r0, #_kernel_offset_to_current]

	/* Save away user stack pointer */
	cps #MODE_SYS
	str sp, [r0, #_thread_offset_to_sp_usr] /* sp_usr */

	/* Switch to privileged stack */
	ldr sp, [r0, #_thread_offset_to_priv_stack_end] /* priv stack end */

isr_system_thread:
	cps #MODE_SVC
	pop {r0}
	cps #MODE_IRQ
#endif

	/*
	 * Save away r0-r3, r12 and lr_irq for the previous context to the
	 * process stack since they are clobbered here.  Also, save away lr
	 * and spsr_irq since we may swap processes and return to a different
	 * thread.
	 */
	sub lr, lr, #4
	srsdb #MODE_SYS!
	cps #MODE_SYS
	push {r0-r3, r12, lr}

	/*
	 * Use SVC mode stack for predictable interrupt behaviour; running ISRs
	 * in the SYS/USR mode stack (i.e. interrupted thread stack) leaves the
	 * ISR stack usage at the mercy of the interrupted thread and this can
	 * be prone to stack overflows if any of the ISRs and/or preemptible
	 * threads have high stack usage.
	 *
	 * When userspace is enabled, this also prevents leaking privileged
	 * information to the user mode.
	 */
	cps #MODE_SVC

	/*
	 * Preserve lr_svc which may contain the branch return address of the
	 * interrupted context in case of a nested interrupt. This value will
	 * be restored prior to exiting the interrupt in z_arm_int_exit.
	 */
	push {lr}

	/* Align stack at double-word boundary */
	and r3, sp, #4
	sub sp, sp, r3
	push {r2, r3}

	/* Increment interrupt nesting count */          # 因为系统默认允许中断嵌套，因此这里增加系统的中断嵌套层级
	ldr r2, =_kernel
	ldr r0, [r2, #_kernel_offset_to_nested]
	add r0, r0, #1
	str r0, [r2, #_kernel_offset_to_nested]
#endif /* CONFIG_CPU_CORTEX_M */

#ifdef CONFIG_TRACING_ISR
	bl sys_trace_isr_enter
#endif

#ifdef CONFIG_PM
	/*
	 * All interrupts are disabled when handling idle wakeup.  For tickless
	 * idle, this ensures that the calculation and programming of the
	 * device for the next timer deadline is not interrupted.  For
	 * non-tickless idle, this ensures that the clearing of the kernel idle
	 * state is not interrupted.  In each case, z_pm_save_idle_exit
	 * is called with interrupts disabled.
	 */

#if defined(CONFIG_CPU_CORTEX_M)
	/*
	 * Disable interrupts to prevent nesting while exiting idle state. This
	 * is only necessary for the Cortex-M because it is the only ARM
	 * architecture variant that automatically enables interrupts when
	 * entering an ISR.
	 */
	cpsid i  /* PRIMASK = 1 */
#endif

	/* is this a wakeup from idle ? */
	ldr r2, =_kernel
	/* requested idle duration, in ticks */
	ldr r0, [r2, #_kernel_offset_to_idle]
	cmp r0, #0

#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
	beq _idle_state_cleared
	movs.n r1, #0
	/* clear kernel idle state */
	str r1, [r2, #_kernel_offset_to_idle]
	bl z_pm_save_idle_exit
_idle_state_cleared:

#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
	ittt ne
	movne	r1, #0
		/* clear kernel idle state */
		strne	r1, [r2, #_kernel_offset_to_idle]
		blne	z_pm_save_idle_exit
#elif defined(CONFIG_ARMV7_R) || defined(CONFIG_AARCH32_ARMV8_R)
	beq _idle_state_cleared
	movs r1, #0
	/* clear kernel idle state */
	str r1, [r2, #_kernel_offset_to_idle]
	bl z_pm_save_idle_exit
_idle_state_cleared:
#else
#error Unknown ARM architecture
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */

#if defined(CONFIG_CPU_CORTEX_M)
	cpsie i		/* re-enable interrupts (PRIMASK = 0) */
#endif

#endif /* CONFIG_PM */

#if defined(CONFIG_CPU_CORTEX_M)
	mrs r0, IPSR	/* get exception number */
#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
	ldr r1, =16
	subs r0, r1	/* get IRQ number */
	lsls r0, #3	/* table is 8-byte wide */
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
	sub r0, r0, #16	/* get IRQ number */
	lsl r0, r0, #3	/* table is 8-byte wide */
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */
#elif defined(CONFIG_CPU_AARCH32_CORTEX_R)
	/* Get active IRQ number from the interrupt controller */
#if !defined(CONFIG_ARM_CUSTOM_INTERRUPT_CONTROLLER)
	bl arm_gic_get_active
#else
	bl z_soc_irq_get_active
#endif /* !CONFIG_ARM_CUSTOM_INTERRUPT_CONTROLLER */
	push {r0, r1}
	lsl r0, r0, #3	/* table is 8-byte wide */
#else
#error Unknown ARM architecture
#endif /* CONFIG_CPU_CORTEX_M */

#if !defined(CONFIG_CPU_CORTEX_M)
	/*
	 * Enable interrupts to allow nesting.
	 *
	 * Note that interrupts are disabled up to this point on the ARM
	 * architecture variants other than the Cortex-M. It is also important
	 * to note that that most interrupt controllers require that the nested
	 * interrupts are handled after the active interrupt is acknowledged;
	 * this is be done through the `get_active` interrupt controller
	 * interface function.
	 */
	cpsie i                                         # 这里打开中断，此时已经可以执行中断嵌套

	/*
	 * Skip calling the isr if it is a spurious interrupt.
	 */
	mov r1, #CONFIG_NUM_IRQS
	lsl r1, r1, #3
	cmp r0, r1
	bge spurious_continue
#endif /* !CONFIG_CPU_CORTEX_M */

	ldr r1, =_sw_isr_table
	add r1, r1, r0	/* table entry: ISRs must have their MSB set to stay
			 * in thumb mode */

	ldm r1!,{r0,r3}	/* arg in r0, ISR in r3 */
	blx r3		/* call ISR */

#if defined(CONFIG_CPU_AARCH32_CORTEX_R)
spurious_continue:
	/* Signal end-of-interrupt */
	pop {r0, r1}
#if !defined(CONFIG_ARM_CUSTOM_INTERRUPT_CONTROLLER)
	bl arm_gic_eoi
#else
	bl z_soc_irq_eoi
#endif /* !CONFIG_ARM_CUSTOM_INTERRUPT_CONTROLLER */
#endif /* CONFIG_CPU_AARCH32_CORTEX_R */

#ifdef CONFIG_TRACING_ISR
	bl sys_trace_isr_exit
#endif

#if defined(CONFIG_ARMV6_M_ARMV8_M_BASELINE)
	pop {r0, r3}
	mov lr, r3
#elif defined(CONFIG_ARMV7_M_ARMV8_M_MAINLINE)
	pop {r0, lr}
#elif defined(CONFIG_ARMV7_R) || defined(CONFIG_AARCH32_ARMV8_R)
	/*
	 * r0 and lr_irq were saved on the process stack since a swap could
	 * happen.  exc_exit will handle getting those values back
	 * from the process stack to return to the correct location
	 * so there is no need to do anything here.
	 */
#else
#error Unknown ARM architecture
#endif /* CONFIG_ARMV6_M_ARMV8_M_BASELINE */

	/* Use 'bx' instead of 'b' because 'bx' can jump further, and use
	 * 'bx' instead of 'blx' because exception return is done in
	 * z_arm_int_exit() */
	ldr r1, =z_arm_int_exit
	bx r1

```

### 中断退出

```asm

/**
 * @brief Kernel housekeeping when exiting interrupt handler installed directly
 *        in the vector table
 *
 * Kernel allows installing interrupt handlers (ISRs) directly into the vector
 * table to get the lowest interrupt latency possible. This allows the ISR to
 * be invoked directly without going through a software interrupt table.
 * However, upon exiting the ISR, some kernel work must still be performed,
 * namely possible context switching. While ISRs connected in the software
 * interrupt table do this automatically via a wrapper, ISRs connected directly
 * in the vector table must invoke z_arm_int_exit() as the *very last* action
 * before returning.
 *
 * e.g.
 *
 * void myISR(void)
 * {
 * 	printk("in %s\n", __FUNCTION__);
 * 	doStuff();
 * 	z_arm_int_exit();
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
	cpsid i                                         # 在降低系统中断嵌套层级之前，需要关掉中断，避免此时再次陷入中断

	/* Decrement interrupt nesting count */         # 降低中断嵌套层级
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

/**
 * @brief Kernel housekeeping when exiting exception handler
 *
 * The exception exit routine performs appropriate housekeeping tasks depending
 * on the mode of exit:
 *
 * If exiting a nested or non-fatal exception, the exit routine restores the
 * saved exception stack frame to resume the excepted context.
 *
 * If exiting a non-nested fatal exception, the exit routine, assuming that the
 * current faulting thread is aborted, discards the saved exception stack
 * frame containing the aborted thread context and switches to the next
 * scheduled thread.
 *
 * void z_arm_exc_exit(bool fatal)
 *
 * @param fatal True if exiting from a fatal fault; otherwise, false
 */
SECTION_SUBSEC_FUNC(TEXT, _HandlerModeExit, z_arm_exc_exit)
	/* Do not context switch if exiting a nested exception */
	ldr r3, =_kernel
	ldr r1, [r3, #_kernel_offset_to_nested]
	cmp r1, #1
	bhi __EXIT_EXC

	/* If the fault is not fatal, return to the current thread context */
	cmp r0, #0
	beq __EXIT_EXC

	/*
	 * If the fault is fatal, the current thread must have been aborted by
	 * the exception handler. Clean up the exception stack frame and switch
	 * to the next scheduled thread.
	 */

	/* Clean up exception stack frame */
	add sp, #32

	/*
	 * Switch in the next scheduled thread.
	 *
	 * Note that z_arm_pendsv must be called in the SVC mode because it
	 * switches to the SVC mode during context switch and returns to the
	 * caller using lr_svc.
	 */
	cps #MODE_SVC
	bl z_arm_pendsv

	/* Decrement exception nesting count */
	ldr r3, =_kernel
	ldr r0, [r3, #_kernel_offset_to_nested]
	sub r0, r0, #1
	str r0, [r3, #_kernel_offset_to_nested]

	/* Return to the switched thread */
	cps #MODE_SYS
	pop {r0-r3, r12, lr}
	userspace_exc_exit
	rfeia sp!

__EXIT_EXC:
	/* Decrement exception nesting count */
	ldr r0, [r3, #_kernel_offset_to_nested]
	sub r0, r0, #1
	str r0, [r3, #_kernel_offset_to_nested]

	/*
	 * Restore r0-r3, r12, lr, lr_und and spsr_und from the exception stack
	 * and return to the current thread.
	 */
	ldmia sp, {r0-r3, r12, lr}^
	add sp, #24
	rfeia sp!  # 从任务栈中恢复 CPSR 寄存器的值及 PC 寄存器的值，即会恢复到任务接下来要执行的指令地址

```

### 异常处理

#### data abort

```c
/**
 * @brief Data abort fault handler
 *
 * @return Returns true if the fault is fatal
 */
bool z_arm_fault_data(z_arch_esf_t *esf)
{
	/* Read and parse Data Fault Status Register (DFSR) */
	uint32_t dfsr = __get_DFSR();
	uint32_t fs = ((dfsr & DFSR_FS1_Msk) >> 6) | (dfsr & DFSR_FS0_Msk);

	/* Read Data Fault Address Register (DFAR) */
	uint32_t dfar = __get_DFAR();

#if defined(CONFIG_USERSPACE)
	if ((fs == FSR_FS_BACKGROUND_FAULT)
			|| (fs == FSR_FS_PERMISSION_FAULT)) {
		if (memory_fault_recoverable(esf)) {
			return false;
		}
	}
#endif

	/* Print fault information*/
	LOG_ERR("***** DATA ABORT *****");
	if (FAULT_DUMP_VERBOSE) {
		dump_fault(fs, dfar);
	}

	/* Invoke kernel fatal exception handler */
	z_arm_fatal_error(K_ERR_CPU_EXCEPTION, esf);

	/* All data aborts are treated as fatal for now */
	return true;
}
```

#### undef instruction

```c
/**
 * @brief Undefined instruction fault handler
 *
 * @return Returns true if the fault is fatal
 */
bool z_arm_fault_undef_instruction(z_arch_esf_t *esf)
{
	/* Print fault information */
	LOG_ERR("***** UNDEFINED INSTRUCTION ABORT *****");

	/* Invoke kernel fatal exception handler */
	z_arm_fatal_error(K_ERR_CPU_EXCEPTION, esf);

	/* All undefined instructions are treated as fatal for now */
	return true;
}

```

#### perfetch abort

```c
/**
 * @brief Prefetch abort fault handler
 *
 * @return Returns true if the fault is fatal
 */
bool z_arm_fault_prefetch(z_arch_esf_t *esf)
{
	/* Read and parse Instruction Fault Status Register (IFSR) */
	uint32_t ifsr = __get_IFSR();
	uint32_t fs = ((ifsr & IFSR_FS1_Msk) >> 6) | (ifsr & IFSR_FS0_Msk);

	/* Read Instruction Fault Address Register (IFAR) */
	uint32_t ifar = __get_IFAR();

	/* Print fault information*/
	LOG_ERR("***** PREFETCH ABORT *****");
	if (FAULT_DUMP_VERBOSE) {
		dump_fault(fs, ifar);
	}

	/* Invoke kernel fatal exception handler */
	z_arm_fatal_error(K_ERR_CPU_EXCEPTION, esf);

	/* All prefetch aborts are treated as fatal for now */
	return true;
}
```

#### nmi abort

```c
/**
 *
 * @brief Handler installed in the vector table
 *
 * Simply call what is installed in 'static void(*handler)(void)'.
 *
 * @return N/A
 */

void z_arm_nmi(void)
{
	handler();
	z_arm_int_exit();
}
```

