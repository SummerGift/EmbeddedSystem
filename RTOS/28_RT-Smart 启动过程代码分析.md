# RT-Smart 启动过程代码分析

在熟悉 RT-Smart 架构的过程中，对其启动过程的了解是必不可少的，在系统正常运行之前，做了哪些准备工作呢？

RT-Smart 与 RT-Thread 的一大区别是使用了 MMU，将用户态和内核态的区分开来。

在先前的 RTOS 实践中，即使使用 MMU，一般也是简单配置成一一映射的方式，也就是虚拟地址和物理地址相同。但是在 SMART 下这样简单的配置是不够的，因为在 SMART 上操作系统运行在内核态，而用户进程运行在用户态。

### 系统初始化过程

![image-20210512173417892](figures/image-20210512173417892.png)

系统刚启动的时候，内核程序被加载器加载到 `0x60010000` 的位置，但是内核在编译时被链接到 `0xc0010000` 的位置，此时 `MMU` 还没有开启。因此一开始想要使用全局变量的时候，这些虚拟的全局变量的地址需要加上 `PV_OFFSET`（物理地址减去虚拟地址的偏移量）获取实际的物理地址，才能正常使用内存。

### MMU 初始化

为了能让内核正常运行在内核地址空间，需要一些初始化对 `MMU` 进行逐步配置，初始化步骤如下：

1. 使用实际物理地址设置栈，为调用 C 语言函数对 `MMU` 页表进行初始化作准备
2. 建立从 `0x60010000` 到  `0x60010000`  的原地址映射
3. 建立从 `0x60010000` 到 `0xc0010000` 的物理地址到内核地址空间的映射
4. 使能 `MMU`，使地址映射生效，以上要建立两次映射的原因是，如果只建立物理地址空间到内核地址空间的映射，此时程序还运行在 `0x60010000` 的物理空间上，如果此时开启 `MMU`，那么当前程序的地址空间变得无法访问，导致程序 fault
5. 切换到内核地址空间，在内核地址空间重新设置栈
6. 解除 `0x60010000` 到  `0x60010000`  的原地址映射关系

现在遇到的问题是，没看明白 MMU 表描述符里面内容的具体意义，后面要仔细阅读 armv7 短描述符的意义，看看是如何进行配置的。


```assembly
.equ UND_Stack_Size,     0x00000400
.equ SVC_Stack_Size,     0x00000400
.equ ABT_Stack_Size,     0x00000400
.equ RT_FIQ_STACK_PGSZ,  0x00000000
.equ RT_IRQ_STACK_PGSZ,  0x00000800
.equ USR_Stack_Size,     0x00000400

#define ISR_Stack_Size  (UND_Stack_Size + SVC_Stack_Size + ABT_Stack_Size + \
                 RT_FIQ_STACK_PGSZ + RT_IRQ_STACK_PGSZ)

.section .data.share.isr
/* stack */
.globl stack_start
.globl stack_top

stack_start:
.rept ISR_Stack_Size
.byte 0
.endr
stack_top:

#ifdef RT_USING_USERSPACE
.data
.align 14
init_mtbl:
    .space 16*1024
#endif

.text
/* reset entry */
.globl _reset
_reset:

#ifdef RT_USING_USERSPACE
    ldr r5, =PV_OFFSET

    mov r7, #0x100000
    sub r7, #1
    mvn r8, r7

    ldr r9, =KERNEL_VADDR_START

    ldr r6, =__bss_end
    add r6, r7
    and r6, r8 //r6 end vaddr align up to 1M
    sub r6, r9 //r6 is size

    ldr sp, =stack_top
    add sp, r5 //use paddr

    ldr r0, =init_mtbl
    add r0, r5
    mov r1, r6
    mov r2, r5
    bl init_mm_setup

    ldr lr, =after_enable_mmu
    ldr r0, =init_mtbl
    add r0, r5
    b enable_mmu

after_enable_mmu:
#endif

#ifndef SOC_BCM283x
    /* set the cpu to SVC32 mode and disable interrupt */
    cps #Mode_SVC
#endif

    /* disable the data alignment check */
    mrc p15, 0, r1, c1, c0, 0
    bic r1, #(1<<1)
    mcr p15, 0, r1, c1, c0, 0

    /* setup stack */
    bl      stack_setup

    /* clear .bss */
    mov r0,#0                   /* get a zero                       */
    ldr r1,=__bss_start         /* bss start                        */
    ldr r2,=__bss_end           /* bss end                          */

bss_loop:
    cmp r1,r2                   /* check if data to clear           */
    strlo r0,[r1],#4            /* clear 4 bytes                    */
    blo bss_loop                /* loop until done                  */

    /* initialize the mmu table and enable mmu */
    ldr r0, =platform_mem_desc
    ldr r1, =platform_mem_desc_size
    ldr r1, [r1]
    bl rt_hw_init_mmu_table

#ifdef RT_USING_USERSPACE
    ldr r0, =MMUTable    /* vaddr    */
    add r0, r5           /* to paddr */
    bl  switch_mmu
#endif

    /* call C++ constructors of global objects                      */
    ldr     r0, =__ctors_start__
    ldr     r1, =__ctors_end__

ctor_loop:
    cmp     r0, r1
    beq     ctor_end
    ldr     r2, [r0], #4
    stmfd   sp!, {r0-r1}
    mov     lr, pc
    bx      r2
    ldmfd   sp!, {r0-r1}
    b       ctor_loop
ctor_end:

    /* start RT-Thread Kernel */
    ldr     pc, _rtthread_startup
_rtthread_startup:
    .word rtthread_startup
```

内存页表初始化函数：

初始化页表前 4k 的内存

```c
void init_mm_setup(unsigned int *mtbl, unsigned int size, unsigned int pv_off) {
    unsigned int va;

    for (va = 0; va < 0x1000; va++) {
        unsigned int vaddr = (va << 20);
        if (vaddr >= KERNEL_VADDR_START && vaddr - KERNEL_VADDR_START < size) {
            mtbl[va] = ((va << 20) + pv_off) | NORMAL_MEM;
        } else if (vaddr >= (KERNEL_VADDR_START + pv_off) && vaddr - (KERNEL_VADDR_START + pv_off) < size) {
            mtbl[va] = (va << 20) | NORMAL_MEM;
        } else {
            mtbl[va] = 0;
        }
    }
}
```

使能 MMU：

```assembly
.align 2
.global enable_mmu
enable_mmu:
    orr r0, #0x18
    mcr p15, 0, r0, c2, c0, 0 //ttbr0

    mov r0, #(1 << 5)         //PD1=1
    mcr p15, 0, r0, c2, c0, 2 //ttbcr

    mov r0, #1
    mcr p15, 0, r0, c3, c0, 0 //dacr

    // invalid tlb before enable mmu
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0
    mcr p15, 0, r0, c7, c5, 0   ;//iciallu
    mcr p15, 0, r0, c7, c5, 6   ;//bpiall

    mrc p15, 0, r0, c1, c0, 0
    orr r0, #(1 | 4)
    orr r0, #(1 << 12)
    mcr p15, 0, r0, c1, c0, 0
    dsb
    isb
    mov pc, lr
```

转换 MMU：

```
.global switch_mmu
switch_mmu:
    orr r0, #0x18
    mcr p15, 0, r0, c2, c0, 0 //ttbr0

    // invalid tlb
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0
    mcr p15, 0, r0, c7, c5, 0   ;//iciallu
    mcr p15, 0, r0, c7, c5, 6   ;//bpiall

    dsb
    isb
    mov pc, lr
```



