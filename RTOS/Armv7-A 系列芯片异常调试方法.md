# Armv7-A 系列芯片异常调试方法

本周解决了两个在 Armv7-A 系列芯片上出现的错误，包括一个由于软浮点配置导致的未定义指令错误以及一个由于数据未对齐导致的 data abort 错误，通过解决这两个问题，学会了如何在 A 系列芯片上根据错误调用栈来 debug，进而找出错误原因并修正。

## Undefined instruction

首先是一个 undefined instruction 错误，通过系统的 pc 指针减去 4 的位置（因为是 ARM 模式所以要 -4）找到出错位置的反汇编指令，发现是一个硬浮点指令，进而发现系统中没有开启芯片系统中的硬浮点处理功能。本来想开启该功能，后来发现在线程切换等地方没有对先关的硬件浮点寄存器压栈，因此这条路暂时走不通，因此在编译的时候重新指定使用软浮点来进行计算，就解决了这个 undefined instruction 的错误。出现这个问题的原因是系统的 cp15 协处理器的硬浮点计算功能没有开启，此时系统不支持该硬浮点运算指令，当系统看到该指令时，就会出现错误。

## Data abort

另外一个问题发生在当想把新开发版上开发的程序运行在 microzed 开发板上的时候，运行一会儿就会出现 data abort。

解决这个问题是通过查询 arm-v7 说明手册（1477 页）将 DFAR（数据错误地址寄存器）和 DFSR（数据错误状态寄存器）中的值通过汇编指令在异常出现时打印出来，指令如下：

```asm
asm volatile ("mrc p15, 0, %0, c6, c0, 0\n" : "=r" (data_addr)); Read DFAR into data_addr
```

发现错误地址是一个非四字节对齐的地址，猜想错误是由于非对齐取数据造成的。通过观察 DFSR 中的值，指令如下：

```asm
asm volatile ("MRC p15, 0, %0, c5, c0, 0\n" : "=r" (data_stat)); Read DFSR into data_stat
```

得出错误的原因是确实是数据未对齐（验证了先前的猜想）。

![data status](assets/1593397701371.png)

## 非对齐访问

```
-munaligned-access
-mno-unaligned-access
Enables (or disables) reading and writing of 16- and 32- bit values from addresses that are not 16- or 32- bit aligned. By default unaligned access is238 Using the GNU Compiler Collection (GCC) disabled for all pre-ARMv6, all ARMv6-M and for ARMv8-M Baseline architectures, and enabled for all other architectures. If unaligned access is not enabled then words in packed data structures are accessed a byte at a time.

The ARM attribute Tag_CPU_unaligned_access is set in the generated object file to either true or false, depending upon the setting of this option. If unaligned access is enabled then the preprocessor symbol __ARM_FEATURE_UNALIGNED is also defned.
```

系统中的结构体数据，如果添加了 `__packed` 属性，则会以紧凑的方式进行内存排布，此时其中的一些数据在内存中的排布就是非对齐的。在程序运行时，如果系统不允许非对齐访问，此时对该结构体中的非对齐数据进行访问，则会出现 data abort 的错误。

如果在编译和链接时添加 `-mno-unaligned-access` 不支持非对齐内存访问选项，将会告诉编译器，生成操作这些非对齐数据指令，需要一个字节一个字节地读取，然后将结果拼凑成最终的数据。用这种方式操作数据降低了数据的访问效率，但是可以避免出现非对齐访问错误。

在 armv7 中可以开启或者关闭非对齐访问检查，例如使用如下指令关闭非对齐访问检查：

```
    /* disable the data alignment check */
    mrc p15, 0, r1, c1, c0, 0
    bic r1, #(1<<1)
    mcr p15, 0, r1, c1, c0, 0
```
如果关闭了非对齐访问检查，此时 CPU 访问非对齐数据将不会报错，在底层硬件实现时，可能会将一次访问拆成多次对齐访问来实现，但是在软件层是不感知的。尽管如此，还是降低了数据的访问效率。另外，一些**强序内存**（例如设备内存）是不支持非对齐访问的。







