# GDB 代码调试记录

在进行某些开发时，没有图形化开发环境，因此需要使用 GDB 直接进行代码调试，以下说明以 RT-Thread QEMU  

`qemu-vexpress-a9` BSP 为例，使用 GDB 进行代码调试。

## 准备步骤

-  scons 编译工程
- 运行 `qemu-dbg.bat` 启动 QEMU 模拟
- `arm-none-eabi-gdb rtthread.elf -ex "tar ext localhost:1234"` 连接到 QEMU 进行代码调试

## GDB 常用命令

| 命令    | 说明                                                      |
| ------- | --------------------------------------------------------- |
| r       | Start debugged program                                    |
| c       | Continue program being debugged                           |
| n       | Step program                                              |
| s       | Step program until it reaches a different source line     |
| b       | Set breakpoint at specified line or function              |
| display | Print value of expression EXP each time the program stops |
| p       | Print value of expression EXP                             |

## 问题

1. 栈内存被写穿的情况，由于线程栈太小，导致在函数较深调用时，导致栈溢出，破坏了系统中其他的数据结构。如果在调试的时候发现，某个变量在没有主动修改的时候突然发生改变，可以怀疑是否出现了栈溢出。