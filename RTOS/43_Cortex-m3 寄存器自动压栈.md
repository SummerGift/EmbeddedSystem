# CM3 寄存器自动压栈

在 CM3 架构上发生中断时，硬件会自动压栈部分寄存器：

- R0-R3
- R12（The Intra-Procedure-call scratch register）、
- R14（The Link Register）
- R15（The Program Counter），

那么其他的寄存器是否需要保存，答案是肯定的。引用 《Procedure Call Standard for the Arm Architecture》 中的原话如下:

A subroutine must preserve the contents of the registers r4-r8, r10, r11 and SP (and r9 in PCS variants that designate r9 as v6)

这句话的意思是，当程序调用一个 C 函数时，需要自己保存 r4-r8, r10, r11 and SP，这就意味着，如果你在中断处理函数里只编写了 C 函数，那么当函数调用结束时，这些 C 函数破坏了的上述寄存器组会被妥善的恢复。

那么谁来做这件事呢，答案是编译器，ARM 编译器会严格遵守上述规则来生成相应的汇编代码，保证你编写的 C 语言函数，破坏的寄存器都会被完全恢复。
