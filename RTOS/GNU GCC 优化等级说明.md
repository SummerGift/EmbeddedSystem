# GNU GCC 优化等级说明

## GCC 编译器优化说明

关于 GCC 编译器优化，首先要注意的是不同版本的编译器其优化配置项是不同的，对于一个相同的 O2 优化等级，其实际优化的配置项是不同的，因此在查看编译器优化选项的时候，记得要找到对应的版本，而不是在网上随便找一个文档就当做是自己受伤工具链合适的文档。

例如，不同版本 GCC 的 O3 优化等级添加的优化项目是不同的：

- GCC 8.4 O3 
```
-finline-functions 
-funswitch-loops 
-fpredictive-commoning 
-fgcse-after-reload 
-ftree-loop-vectorize 
-ftree-loop-distribution 
-ftree-loop-distribute-patterns 
-floop-interchange 
-floop-unroll-and-jam 
-fsplit-paths 
-ftree-slp-vectorize 
-fvect-cost-model 
-ftree-partial-pre 
-fpeel-loops 
-fipa-cp-clone
```

-  GCC 7.5 O3
```
-finline-functions
-funswitch-loops
-fpredictive-commoning
-fgcse-after-reload
-ftree-loop-vectorize
-ftree-loop-distribute-patterns
-fsplit-paths
-ftree-slp-vectorize
-fvect-cost-model
-ftree-partial-pre
-fpeel-loops
-fipa-cp-clone 
```

不同版本 GCC 的说明文档地址如下：

```
https://gcc.gnu.org/onlinedocs/
```

### 网络性能与编译器优化

不同的编译等级以及选项对 SMP 下网络性能的影响：

| 优化等级| 其他选项 |elf 体积|网络速度 (Mbps)|
| ------ | ------ |------|------|
| O0| non |3,620,228  bytes |1-2|
| O0| -finline-functions|3,620,252  bytes|0.6-2.2|
| O2| -fno-inline|4,118,416  bytes|1.8-3.3|
| O2| -finline-functions |4,607,800  bytes | 36-61|
| O2| `-finline-functions -funswitch-loops` |4,621,236  bytes | 3-7 |
| O2| `-finline-functions -funswitch-loops -fgcse-after-reload` |4,620,896  bytes | 2-5 |
| O2| -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-loop-vectorize -ftree-loop-distribution -ftree-loop-distribute-patterns -floop-interchange -floop-unroll-and-jam -fsplit-paths -ftree-slp-vectorize -fvect-cost-model -ftree-partial-pre -fpeel-loops -fipa-cp-clone (GCC 8.4) |4,725,288  bytes | 23-40 |
|O2| -finline-functions -funswitch-loops -fpredictive-commoning -fgcse-after-reload -ftree-loop-vectorize -ftree-loop-distribute-patterns -fsplit-paths -ftree-slp-vectorize -fvect-cost-model -ftree-partial-pre -fpeel-loops -fipa-cp-clone (GCC 7.5) |4,725,156  bytes | 5-10 |
| O3| non  |4,728,412  bytes|95-100|
| O3| -fno-inline |4,223,332  bytes|60-80|
| O3| -fno-inline-functions |4,454,700  bytes| 87-97 |
| O3| 关闭一些优化选项 -fno-gcse-after-reload -fno-ipa-cp-clone -fno-loop-interchange -fno-loop-unroll-and-jam -fno-peel-loops -fno-predictive-commoning -fno-split-loops -fno-split-paths -fno-tree-loop-distribution -fno-tree-loop-vectorize -fno-tree-partial-pre -fno-tree-slp-vectorize -fno-unswitch-loops -fno-vect-cost-model |4,696,224  bytes| 100 - 107 |
| O3| 关闭一些优化选项 -fno-inline-functions -fno-unswitch-loops -fno-predictive-commoning -fno-gcse-after-reload -fno-tree-loop-vectorize -fno-tree-loop-distribution -fno-tree-loop-distribute-patterns -fno-loop-interchange -fno-loop-unroll-and-jam -fno-split-paths -fno-tree-slp-vectorize -fno-vect-cost-model -fno-tree-partial-pre -fno-peel-loops -fno-ipa-cp-clone |4,409,336  bytes| 73-95 |

可以从上面的表格，对比起来可以看出，在 O2 优化下，如果开启 inline 优化，网络的性能有明显提升，最高达到了 `60 Mbps`。同时也可以看出， 网络性能的提升是由多种优化项组合影响的，而且，这种影响并不是简单的线性叠加，而是互有补充，这就给分析影响性能的优化因素带来了更多的复杂性。

在测试编译器优化项目的过程中，我体会到现代编译器的优化能力已经非常优秀，可以帮助软件开发人员很大程度地优化代码速度与体积，并且可以灵活地根据配置项来平衡代码运行速度与体积，这一点让我印象深刻。我认为以后开发程序应当以可读性和可拓展性为先，优化可以很大程度上放心地交给编译器，等到必须的时候，再优化程序的性能。

### 内联优化的影响因素

`inline function` 优化指的是当需要函数调用时，将被调函数的代码在原地展开，相当于拷贝过来，直接执行，进而节省了一些系统开销。而实际上，现代编译器对 inline 的优化分为了很多种类，而不仅仅是函数 inline，而仅仅是关于 inline 的编译器配置项就有 6-8 项之多，可见其中有很多细节可以挖掘。

关于 inline 函数加速程序运行的原因，我能想到的点可能如下：

1. 节省了频繁调用函数压栈的开销(1.可能的参数压入 2. 压入 lr 3. 使用过程中非 r0-r3 的寄存器需要保存)
2. 因为函数跳转导致的 CPU 流水线损失，bl 和 ret 的时候都将出现流水线指令损失
3. 因为原地展开，程序的局部性更好，使得可以更好地利用指令 cache，减少访存时间
4. 函数原地展开后，可以根据函数具体的使用的上下文情况，优化的更适合于这个函数的使用方式


## Reference

- [GNU Optimize Options](https://gcc.gnu.org/onlinedocs/ )