# volatile 的使用说明


By declaring an object volatile, the compiler is informed that the value of the object can change beyond the compiler’s control. The compiler must also assume that any accesses can have side effects—thus all accesses to the volatile object must be preserved.

There are three main reasons for declaring an object volatile:

- Shared access; the object is shared between several tasks in a multitasking environment
- Trigger access; as for a memory-mapped SFR where the fact that an access occurs
has an effect
-  Modified access; where the contents of the object can change in ways not known to
the compiler.

## Shared access

the object is shared between several tasks in a multitasking environment。

当同一**全局变量**在多个线程之间被共享时，有可能会出现同步错误，编译器可能会将访问该全局变量的代码优化为访问某个寄存器，而不会再次访问相应的内存，导致程序运行错误。

## Trigger access

as for a memory-mapped SFR（特殊功能寄存器）where the fact that an access occurs has an effect。

当读取类似串口设备的数据寄存器时，一定要加上 volatile，因为该地址寄存器中的数值可能会发生改变，如果不加 volatile，可能会发现读取的数据是错误的。

## Modified access

where the contents of the object can change in ways not known to the compiler.

对象的内容可能会被以编译器不清楚的方式被修改，例如在内核态与用户态的程序在不同的虚拟地址访问同一块物理内存，此时如果不加上 volatile，则外部的修改无法被感知到，造成程序错误。

## 关于优化错误

如果系统在低优化等级能正常运行，但是在高优化的情况下的无法正常运行，首先怀疑两个方面：

- 是否是一些关键操作没有添加 volatile
- 是否是有内存写穿（因为不同的优化等级改变了内存排布导致写穿位置发生改变）

如果发现加上了 `printf` 打印，或者调用了某个外部函数，系统就正常运行了，也要怀疑是否出现了变量访问被优化的情况，因为如果加上了**外部函数**（非本文件中的函数或其他库中的函数）调用，则编译器无法确定被引用的变量是否被外部函数所改变，因而会自动从原有地址重新读取该变量的值。

## 结论

关于 volatile 关键字，最重要的是要认识到一点，即是否在编译器清楚的范围之外，所操作的变量有可能被改变，如果有这种可能性，则一定要添加上 volatile 关键字，以避免这种错误。

归根结底，是要确定代码在真实运行的状态下，当其访问某个变量时，是否真正地从这个变量所在的地址重新读取该变量的值，而不是直接使用上次存储在某个寄存器中的值。

