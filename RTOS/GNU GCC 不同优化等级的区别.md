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

### **-Os**
Optimize for size. "-Os" enables all "-O2" optimizations that do not typically increase code size. It also performs further optimizations designed to reduce code size.

If you use multiple "-O" options, with or without level numbers, the last such option is the one that is effective.

## Reference

- [GNU Optimize Options](http://gcc.gnu.org/onlinedocs/gcc/Optimize-Options.html  )