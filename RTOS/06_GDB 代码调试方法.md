# GDB 代码调试记录

在进行某些开发时，没有图形化开发环境，因此需要使用 GDB 直接进行代码调试，以下说明以 RT-Thread QEMU  

`qemu-vexpress-a9` BSP 为例，使用 GDB 进行代码调试。

## 准备步骤

- scons 编译工程
- 运行 `qemu-dbg.bat` 启动 QEMU 模拟

可以使用 arm-none-eabi-gdb 或者 gdb-multiarch 作为 gdb 服务端。

### 使用 `gdb-multiarch` 

- 安装 `gdb-multiarch` 作为 gdb 服务端 
- `gdb-multiarch rtthread.elf -ex "tar ext localhost:1234"` 连接到 QEMU 进行代码调试

### 使用 arm-none-eabi-gdb

- `arm-none-eabi-gdb rtthread.elf -ex "tar ext localhost:1234"` 连接到 QEMU 进行代码调试

### 使用图形化界面

如果想要以 GUI 模式启动 gdb，可以在命令行中添加参数 `-tui`，如下面的命令：

- `gdb-multiarch rtthread.elf -ex "tar ext localhost:1234" -tui`

### 查看反汇编

1. 按下 ctrl + x 然后再按下 2 将会切换视图，例如汇编视图和寄存器视图
2. 按下 ctrl + x 然后再按下 1 将会聚焦选择其中一个视图

## GDB 常用命令

| 命令    | 说明                                                       |
| ------- | ---------------------------------------------------------- |
| r       | Start debugged program                                     |
| c       | Continue program being debugged                            |
| n       | Step program                                               |
| ni      | 运行到下一条汇编指令，但是不进行 bl 跳转                   |
| s       | Step program until it reaches a different source line      |
| si      | 运行到下一条汇编指令，会跳转到下一条汇编执行，跟进 bl 命令 |
| b       | Set breakpoint at specified line or function               |
| display | Print value of expression EXP each time the program stops  |
| p       | Print value of expression EXP                              |
| q       | 退出 gdb                                                   |

### 断点操作命令

| 命令           | 说明                                 |
| -------------- | ------------------------------------ |
| info b         | 查询当前系统中断点命令，简写为 `i b` |
| dis + 断点编号 | 使指定编号的断点 disable             |
| ena + 断点编号 | 使指定编号的断点 enable              |
| del + 断点编号 | 删除指定编号的断点                   |

### 打开和关闭 gdb

| 命令                     | 说明           |
| ------------------------ | -------------- |
| ctrl + d                 | 退出 GDB       |
| ctrl + a，松开后再按下 x | 退出 QEMU 调试 |

## 问题

1. 栈内存被写穿的情况，由于线程栈太小，导致在函数较深调用时，导致栈溢出，破坏了系统中其他的数据结构。如果在调试的时候发现，某个变量在没有主动修改的时候突然发生改变，可以怀疑是否出现了栈溢出。


## RT-Smart 调试配置

1. 在 `~/.bachrc` 配置好工具链地址
```
export RTT_CC=gcc
export RTT_EXEC_PATH=your_musleabi_toolchain_path/bin
export RTT_CC_PREFIX=arm-linux-musleabi-
export PATH=$PATH:$RTT_EXEC_PATH:$RTT_EXEC_PATH/../arm-linux-musleabi/bin
```

2. 使用 gdb 进行调试

- `gdb-multiarch rtthread.elf -ex "tar ext localhost:1234" -tui`
