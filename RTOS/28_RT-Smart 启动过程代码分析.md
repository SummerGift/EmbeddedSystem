# RT-Smart 启动过程源代码分析

在熟悉 RT-Smart 架构的过程中，研究其启动过程的是必不可少的，那么在系统正常运行之前，需要做哪些准备工作呢。本文将以 32 位 RT-Smart 的源代码为基础，讲解 RT-Smart 的启动过程。

## 内核地址空间

RT-Smart 与 RT-Thread 的一大区别是用户态和内核态的地址空间被隔离开来。内核运行在内核地址空间，用户进程运行在用户地址空间。由下图可知，RT-Smart 32 位内核运行在地址空间的高地址，而用户程序代码运行在低地址。

![image-20210512173417892](figures/image-20210512173417892.png)

## 系统初始化流程

上面说到 RT-Smart 将内核搬运到高地址空间运行，为了能让内核正常运行在内核地址空间，需要一些初始化对 `MMU` 进行逐步配置，初始化步骤如下：

1. 使用实际物理地址设置栈，为调用 C 语言函数对 `MMU` 页表进行初始化作准备
2. 建立从 `0x60010000` 到  `0x60010000`  的原地址映射
3. 建立从 `0x60010000` 到 `0xc0010000` 的物理地址到内核地址空间的映射
4. 使能 `MMU`，使地址映射生效，需要建立双重映射的原因是，如果只建立第三步的映射，此时程序还运行在 `0x60010000` 的物理空间上，此时开启 `MMU`，当前程序正在运行的地址空间变得无法访问，导致程序 fault 无法继续运行
5. 切换到内核地址空间，在内核地址空间重新设置栈
6. 解除 `0x60010000` 到  `0x60010000`  的原地址映射关系

## 启动过程代码详解（ARMv7）

系统启动前，内核程序被加载到 `0x60010000` ，但内核在编译时被链接到 `0xc0010000` 的位置，此时 `MMU` 还没有开启。如果此时想要使用全局变量的，就需要将全局变量的地址加上 `PV_OFFSET`（物理地址减去虚拟地址的偏移量）获取实际的物理地址，才能正常访问该全局变量。


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

/*
使用 1M 大小的 section 映射，描述符的类型为 unsigned int，占用 4 个字节内存，整个系统地址空间为 4GB，因此需要 4096 个描述符，总共占用内存 16kb。
*/
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
    mvn r8, r7     /* r8: 0xfff0_0000 */

    ldr r9, =KERNEL_VADDR_START

    ldr r6, =__bss_end
    add r6, r7
    and r6, r8          /* r6 end vaddr align up to 1M */
    sub r6, r9          /* r6 is size */

    ldr sp, =stack_top  
    add sp, r5          /* 使用栈的物理地址初始化栈 */

    ldr r0, =init_mtbl
    add r0, r5
    mov r1, r6
    mov r2, r5
    bl init_mm_setup    /* 初始化内存映射表，建立双重映射，即程序加载原地址映射与原地址到内核地址空间映射 */

    ldr lr, =after_enable_mmu
    ldr r0, =init_mtbl
    add r0, r5
    b enable_mmu        /* 使用初始化后的映射表使能 MMU */

after_enable_mmu:
#endif

    /* set the cpu to SVC32 mode and disable interrupt */
    cps #Mode_SVC

    /* disable the data alignment check */
    mrc p15, 0, r1, c1, c0, 0
    bic r1, #(1<<1)
    mcr p15, 0, r1, c1, c0, 0

    /* setup stack */
    bl      stack_setup  /* 使用内核空间栈的虚拟地址初始化栈 */

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

### 双重地址映射函数详解

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

该函数初始化了内存映射表，从 0 地址开始，以 `1M` 的粒度扫描整个 `4G` 地址空间，建立两段映射关系：

1. 如果发现虚拟地址在内核地址空间上，则建立从内核地址空间到内核程序加载地址的映射
2. 如果发现虚拟地址在处于内核程序的加载地址，则建立相对应的原地址映射
3. 其他地址配置成无效，如下图中的空白部分

配置的映射关系如下图所示：



![image-20210513182917961](figures/image-20210513182917961.png)

### 使能 MMU

```assembly
.align 2
.global enable_mmu
enable_mmu:
    orr r0, #0x18
    mcr p15, 0, r0, c2, c0, 0 // ttbr0

    mov r0, #(1 << 5)         // PD1=1
    mcr p15, 0, r0, c2, c0, 2 // ttbcr

    mov r0, #1
    mcr p15, 0, r0, c3, c0, 0 // dacr

    // invalid tlb before enable mmu
    mov r0, #0
    mcr p15, 0, r0, c8, c7, 0
    mcr p15, 0, r0, c7, c5, 0   ; // iciallu
    mcr p15, 0, r0, c7, c5, 6   ; // bpiall

    mrc p15, 0, r0, c1, c0, 0
    orr r0, #(1 | 4)
    orr r0, #(1 << 12)
    mcr p15, 0, r0, c1, c0, 0
    dsb
    isb
    mov pc, lr
```

### 切换 MMU

```assembly
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

### 设置内核空间栈

```assembly
stack_setup:
    ldr     r0, =stack_top     /* 获取内核地址空间下的栈地址，然后设置各模式下的栈 */

    @  Set the startup stack for svc
    mov     sp, r0

    @  Enter Undefined Instruction Mode and set its Stack Pointer
    msr     cpsr_c, #Mode_UND|I_Bit|F_Bit
    mov     sp, r0
    sub     r0, r0, #UND_Stack_Size

    @  Enter Abort Mode and set its Stack Pointer
    msr     cpsr_c, #Mode_ABT|I_Bit|F_Bit
    mov     sp, r0
    sub     r0, r0, #ABT_Stack_Size

    @  Enter FIQ Mode and set its Stack Pointer
    msr     cpsr_c, #Mode_FIQ|I_Bit|F_Bit
    mov     sp, r0
    sub     r0, r0, #RT_FIQ_STACK_PGSZ

    @  Enter IRQ Mode and set its Stack Pointer
    msr     cpsr_c, #Mode_IRQ|I_Bit|F_Bit
    mov     sp, r0
    sub     r0, r0, #RT_IRQ_STACK_PGSZ

    /* come back to SVC mode */
    msr     cpsr_c, #Mode_SVC|I_Bit|F_Bit
    bx      lr
```

与先前的 rt-thread 宏内核相比，整个 SMART 的启动过程主要多了对 MMU 的配置，这是因为 SMART 是一个区分用户态和内核态的操作系统。用户态进程与操作系统内核运行在各自私有的地址空间，为了实现这样的功能，要利用 MMU 提供的虚拟内存机制做更进一步的虚拟地址空间管理。

在 SMART 操作系统中，内存管理部分是比较复杂的，后续针对这一块还需要多多研究。

## 启动过程代码详解（ARMv8）

### 页表初始化过程

```assembly
.L__in_el1:                        /* 清空早期使用的两个页表 */
    adr x1, __start                /* 将代码段的地址写入 x1，在 x1 中暂存该地址，该地址也是栈的顶端 */
    ldr x0, =~0x1fffff             /* 向 x0 存入 2M 地址对齐掩码  */
    and x0, x1, x0                 /* 将 x1 中存放的地址向下 2M 对齐，存入 x0 */     
    add x2, x0, #0x2000            /* 将 x0 增加 8k并存入 x2，注意：早期版本这里使用了 x1 存放计算后的值，会导致在本段代码最后一行读取 x1 的值作为栈时，导致设置了错误的栈，进而导致后续调用 c 函数的过程中程序运行错误 */
.L__clean_pd_loop:
    str     xzr, [x0], #8          /* 清空从 x0 - x2 之间 8k 的地址空间，用作页表*/
    cmp     x0, x2
    bne     .L__clean_pd_loop

    adr     x19, .L__in_el1
    ldr     x8, =.L__in_el1
    sub     x19, x19, x8            /* get PV_OFFSET            */
    mov     sp, x1                  /* in EL1. Set sp to _start */
```

ARMv8 架构的 MMU 初始化过程与上述内容稍有不同，原因是在 ARMv8 中可以利用  TTBR0_EL1 和 TTBR1_EL1 寄存器来区分对高位地址和低位地址的访问。在操作系统启动初期。

### MMU 初始化

对 MMU 进行初始化，同 32 位 SMART 启动一样，也要进行两次映射，这样才能保证在开启 MMU 之后，后续指令可以正常执行。

```c
void rt_hw_mmu_setup_early(unsigned long *tbl0, unsigned long *tbl1, unsigned long size, unsigned long pv_off)
{
    int ret;
    unsigned long va = KERNEL_VADDR_START;
    unsigned long count = (size + ARCH_SECTION_MASK) >> ARCH_SECTION_SHIFT;
    unsigned long normal_attr = MMU_MAP_CUSTOM(MMU_AP_KAUN, NORMAL_MEM);

    /* 创建从高位虚拟地址（以 0xffff 起始的地址）到物理地址的映射 */
    ret = armv8_init_map_2M(tbl1 , va, va + pv_off, count, normal_attr);
    if (ret != 0)
    {
        while (1);
    }

    /* 创建物理地址到物理地址的一一对应映射，保证在跳转到链接高位的函数前，指令仍然可以继续执行 */
    ret = armv8_init_map_2M(tbl0, va + pv_off, va + pv_off, count, normal_attr);
    if (ret != 0)
    {
        while (1);
    }
}
```

### 切换到虚拟地址运行

初始化的目的是，使得系统可以从物理地址运行逐步迁移到以 0xFFFF 起始的虚拟地址运行，这需要一个过度的过程，可以观察如下代码了解如何实现从物理地址到虚拟地址的切换。

```asm
/* jump to C code, should not return */
.L__jump_to_entry:
    bl get_free_page           /* 获取空闲页，用于存放页表 */
    mov x21, x0
    bl get_free_page           /* 获取空闲页，用于存放页表 */
    mov x20, x0

    mov x1, x21
    bl mmu_tcr_init            /* 配置 MMU 的基础属性，如虚拟地址位数、页大小、页属性等 */

    mov x0, x20
    mov x1, x21

    msr ttbr0_el1, x0          /* 将页表放入 ttbr0_el1 寄存器，配置低位地址映射 */
    msr ttbr1_el1, x1          /* 将页表放入 ttbr1_el1 寄存器，配置高位地址映射 */
    dsb sy                     /* 数据同步内存屏障，确保在下一个指令执行前，所有的存储器访问都已经完成 */

    ldr x2, =0x40000000        /* 为内核映射 1G 内存空间          */
    ldr x3, =0x1000060000000   /* 设置 PV_OFFSET 到 x3 寄存器    */
    bl rt_hw_mmu_setup_early   /* 调用 MMU 配置函数，初始化页表    */

    ldr x30, =after_mmu_enable /* 将 after_mmu_enable 函数的地址存入 LR 寄存器，这是一个高位虚拟地址 */
                              
    mrs x1, sctlr_el1         
    bic x1, x1, #(3 << 3)       /* dis SA, SA0 */
    bic x1, x1, #(1 << 1)       /* dis A */
    orr x1, x1, #(1 << 12)      /* I */
    orr x1, x1, #(1 << 2)       /* C */
    orr x1, x1, #(1 << 0)       /* M */
    msr sctlr_el1, x1           /* 使能 MMU，还可以执行下一条指令，因为低位地址进行了一一映射 */

    dsb sy                      /* 使能 MMU 后 */
    isb sy                      /* 指令屏障指令，在执行下一条指令前，所有的指令都已经完成 */
    ic ialluis                  /* 无效所有的指令缓存 */                       
    dsb sy
    isb sy
    tlbi vmalle1                /* 无效所有 el1 的 tlb 转换表 */
    dsb sy
    isb sy
    ret                         /* 返回到虚拟地址的 after_mmu_enable 函数，自此操作系统完成到虚拟地址的切换  */

after_mmu_enable:
    mrs x0, tcr_el1             /* 关闭 ttbr0 上的映射，操作系统将不再访问低位地址 */
    orr x0, x0, #(1 << 7)
    msr tcr_el1, x0
    msr ttbr0_el1, xzr
    dsb sy

    mov     x0, #1
    msr     spsel, x0
    adr     x1, __start
    mov     sp, x1              /* 设置 sp_el1 为 _start 符号地址 */

    b  rtthread_startup
```

