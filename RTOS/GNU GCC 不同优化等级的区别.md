# GNU GCC 不同优化等级的区别

## Solution

### **-O0**
Do not optimize.

### **-O1**

Optimize. Optimizing compilation takes somewhat more time, and a lot more memory for a large function.

Without "-O," the compiler's goal is to reduce the cost of compilation and to make debugging produce the expected results. Statements are independent: if you stop the program with a breakpoint between statements, you can then assign a new value to any variable or change the program counter to any other statement in the function and get exactly the results you would expect from the source code.

Without "-O," the compiler allocates only variables declared as registers in the registers. The resulting compiled code is a little worse than that produced by PCC without "-O."

With "-O," the compiler tries to reduce code size and execution time.

When you specify "-O," the compiler turns on "-fthread-jumps" and "-fdefer-pop" on all machines. The compiler turns on "-fdelayed-branch" on machines that have delay slots, and "-fomit-frame-pointer" on machines that can support debugging even without a frame pointer. On some machines, the compiler also turns on other flags.

### **-O2**

Optimize even more. GCC performs nearly all supported optimizations that do not involve a space-speed tradeoff. The compiler does not perform loop unrolling or function inlining when you specify "-O2." As compared to "-O," this option increases both compilation time and the performance of the generated code.

"-O2" turns on all optional optimizations except for loop unrolling, function inlining, and strict aliasing optimizations. It also turns on the "-fforce-mem" option on all machines and frame pointer elimination on machines where doing so does not interfere with debugging.

### **-O3**

Optimize yet more. "-O3" turns on all optimizations specified by "-O2' and also turns on the "inline-functions" option.

O3 优化下网络驱动性能更好的原因可能是添加了 inline function 的优化，inline function 优化指的是当需要函数调用时，将被调函数的代码在原地展开，相当于拷贝过来，直接执行，进而节省了一些系统开销，我能想到的点可能如下：

1. 节省了频繁调用函数压栈的开销（1.可能的参数压入 2. 压入 lr 3. 使用过程中非 r0-r3 的寄存器需要保存）
2. 因为函数跳转导致的 CPU 流水线损失，bl 和 ret 的时候都将出现流水线指令损失
3. 因为原地展开，程序的局部性更好，使得可以更好地利用指令 cache，减少访存时间
4. 函数原地展开后，可以根据函数具体的使用的上下文情况，优化的更适合于这个函数的使用方式（也就是从整体考虑使用寄存器等等）

### **-Os**
Optimize for size. "-Os" enables all "-O2" optimizations that do not typically increase code size. It also performs further optimizations designed to reduce code size.

If you use multiple "-O" options, with or without level numbers, the last such option is the one that is effective.

## Reference

- [GNU Optimize Options](http://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html  )