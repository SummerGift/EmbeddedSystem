# linux 驱动开发

标签（空格分隔）： linux 驱动开发

---

## 1. 驱动的基础概念

### 1.1 什么是驱动程序

软件层面的驱动广义上就是指：这一段代码操作了硬件，所以这一段代码就叫硬件的驱动程序。（本质上是电力提供了动力，而驱动程序提供了操作逻辑方法）

狭义上驱动程序就是专指操作系统中用来操控硬件的逻辑方法部分代码。

- linux体系架构

分层思想、驱动的上面是系统调用API、驱动的下面是硬件、驱动自己本身也是分层的。

### 1.2 模块化设计

- 微内核和宏内核

宏内核（又称为单内核）：将内核从整体上作为一个大过程实现，并同时运行在一个单独的地址空间。所有的内核服务都在一个地址空间运行，相互之间直接调用函数，简单高效。

微内核：功能被划分成独立的过程，过程间通过IPC进行通信。模块化程度高，一个服务失效不会影响另外一个服务，典型如 windows。

linux ：本质上是宏内核，但是又吸收了微内核的模块化特性，提现在2个层面：

静态模块化：在编译时实现可裁剪，特征是想要功能裁剪改变必须重新编译。

动态模块化：zImage可以不重新编译烧录，甚至可以不关机重启就实现模块的安装和卸载。

### 1.2 linux 设备驱动分类

- 字符设备驱动

准确的说应该叫字节设备，软件操作设备时是以字节为单位进行的。典型的如 LCD、串口、LED、蜂鸣器、触摸屏等。

- 块设备驱动

块设备是相对于字符设备定义的，块设备被软件操作时是以块（多个字节构成的一个单位）为单位的。设备的块大小是设备本身设计时定义好的，软件是不能去更改的，不同设备的块大小可以不一样。常见的块设备都是存储类设备，如：硬盘、NandFlash、iNand、SD 等。

- 网络设备驱动

网络设备是专为网卡设计的驱动模型，linux 中网络设备驱动主要目的是为了支持 API 中 socket 相关的那些函数工作。

### 1.3 驱动程序的安全性要求

- 驱动是内核的一部分
驱动已经成为内核中最庞大的组成部分，内核会直接以函数调用的方式调用驱动代码，驱动的动态安装和卸载都会更改内核。

- 驱动对内核的影响

驱动程序崩溃甚至会导致内核崩溃，驱动的效率会影响内核的整体效率，驱动的漏洞会造成内核安全漏洞。

- 常见驱动安全性问题

未初始化指针、恶意用户程序、缓冲区溢出、竞争状态等。

## 2. 简单驱动源码分析

### 2.1 常用的模块操作命令

- lsmod(list module,将模块列表显示)，功能是打印出当前内核中已经安装的模块列表
- insmod（install module，安装模块），功能是向当前内核中去安装一个模块，用法是insmod xxx.ko
- modinfo（module information，模块信息），功能是打印出一个内核模块的自带信息。，用法是modinfo xxx.ko
- rmmod（remove module，卸载模块），功能是从当前内核中卸载一个已经安装了的模块，用法是rmmod xxx（注意卸载模块时只需要输入模块名即可，不能加.ko后缀）

### 2.2 模块的安装

先 lsmod 再 insmod 看安装前后系统内模块记录。实践测试标明内核会将最新安装的模块放在lsmod 显示的最前面。

- insmod与module_init宏

模块源代码中用module_init宏声明了一个函数（在我们这个例子里是chrdev_init函数），作用就是指定 chrdev_init 这个函数和 insmod 命令绑定起来，也就是说当我们 insmod module_test.ko 时，insmod 命令内部实际执行的操作就是帮我们调用 chrdev_init 函数。

照此分析，那 insmod 时就应该能看到 chrdev_init 中使用printk打印出来的一个 chrdev_init 字符串，但是实际没看到。原因是ubuntu中拦截了，在 ubuntu 中使用 dmesg 命令就可以看到了。

模块安装时 insmod 内部除了帮我们调用 module_init 宏所声明的函数外，实际还做了一些别的事（譬如 lsmod 能看到多了一个模块也是 insmod 帮我们在内部做了记录），但是我们就不用管了。

- 模块的版本信息

使用modinfo查看模块的版本信息，内核 zImage 中也有一个确定的版本信息。

insmod 时模块的 vermagic 必须和内核的相同，否则不能安装，报错信息为：insmod: ERROR: could not insert module module_test.ko: Invalid module format

模块的版本信息是为了保证模块和内核的兼容性，是一种安全措施。如何保证模块的 vermagic和内核的 vermagic 一致？编译模块的内核源码树就是我们编译正在运行的这个内核的那个内核源码树即可。说白了就是模块和内核要同出一门。

- 模块卸载
module_exit 和 rmmod 的对应关系，lsmod查看rmmod前后系统的模块记录变化。

- 模块中常用宏

MODULE_LICENSE，模块的许可证。一般声明为GPL许可证，而且最好不要少，否则可能会出现莫名其妙的错误（譬如一些明显存在的函数提升找不到）。

MODULE_AUTHOR

MODULE_DESCRIPTION

MODULE_ALIAS

- 函数修饰符

`__init`，本质上是个宏定义，在内核源代码中就有`#define __init xxxx`。这个`__init` 的作用就是将被他修饰的函数放入 `.init.text` 段中去（本来默认情况下函数是被放入.text段中）。

整个内核中的所有的这类函数都会被链接器链接放入 `.init.text` 段中，所以所有的内核模块的 `__init` 修饰的函数其实是被统一放在一起的。内核启动时统一会加载 `.init.text` 段中的这些模块安装函数，加载完后就会把这个段给释放掉以节省内存。

### 2.3 printk函数详解

printk 在内核源码中用来打印信息的函数，用法和 printf 非常相似。

printk 和 printf 最大的差别：printf 是 C 库函数，是在应用层编程中使用的，不能在 linux 内核源代码中使用；printk 是 linux 内核源代码中自己封装出来的一个打印函数，是内核源码中的一个普通函数，只能在内核源码范围内使用，不能在应用编程中使用。

printk 相比 printf 来说还多了个**打印级别的设置**。printk 的打印级别是用来控制 printk 打印的这条信息是否在终端上显示的。应用程序中的调试信息要么全部打开要么全部关闭，一般用条件编译来实现（DEBUG宏），但是在内核中，因为内核非常庞大，打印信息非常多，有时候整体调试内核时打印信息要么太多找不到想要的要么一个没有没法调试。所以才有了打印级别这个概念。

操作系统的命令行中也有一个打印信息级别属性，值为 0-7。当前操作系统中执行 printk 的时候会去对比 printk 中的打印级别和我的命令行中设置的打印级别，小于我的命令行设置级别的信息会被放行打印出来，大于的就被拦截的。譬如我的ubuntu中的打印级别默认是 4，那么 printk 中设置的级别比 4 小的就能打印出来，比 4 大的就不能打印出来。

ubuntu 中这个 printk 的打印级别控制没法实践，ubuntu 中不管你把级别怎么设置都不能直接打印出来，必须 dmesg 命令去查看。

### 2.4 关于驱动模块中的头文件

驱动源代码中包含的头文件和原来应用编程程序中包含的头文件不是一回事。应用编程中包含的头文件是应用层的头文件，是应用程序的编译器带来的（譬如 gcc 的头文件路径在 `/usr/include` 下，这些东西是和操作系统无关的）。驱动源码属于内核源码的一部分，驱动源码中的头文件其实就是内核源代码目录下的 `include` 目录下的头文件。

### 2.5 驱动编译的 Makefile

- KERN_DIR

变量的值就是我们用来编译这个模块的内核源码树的目录。

- `obj-m += module_test.o`

这一行就表示我们要将 module_test.c 文件编译成一个模块。

- make -C $(KERN_DIR) M=`pwd` modules

这个命令用来实际编译模块，工作原理就是：利用 make -C 进入到我们指定的内核源码树目录下，然后在源码目录树下借用内核源码中定义的模块编译规则去编译这个模块，编译完成后把生成的文件还拷贝到当前目录下，完成编译。

- make clean

用来清除编译痕迹。

总结：模块的 makefile 非常简单，本身并不能完成模块的编译，而是通过 make -C 进入到内核源码树下借用内核源码的体系来完成模块的编译链接的。这个 Makefile 本身是非常模式化的，3 和 4 部分是永远不用动的，只有 1 和 2 需要动。1 是内核源码树的目录，必须根据自己的编译环境来配置。

## 3. 编写简单的设备驱动

### 3.1 设备注册与卸载
```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>

#define MYMAJOR		200
#define MYNAME		"testchar"

int mymajor;

static int test_chrdev_open(struct inode *inode, struct file *file)
{
	// 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
	// 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
	printk(KERN_INFO "test_chrdev_open\n");
	return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_release\n");
	return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {

	.owner		= THIS_MODULE,				// 惯例，直接写即可
	.open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
	.release	= test_chrdev_release,		// 就是这个.open对应的函数

};

// 模块安装函数
static int __init chrdev_init(void)
{	
	printk(KERN_INFO "chrdev_init helloworld init\n");
    
	// 在module_init宏调用的函数中去注册字符设备驱动
	// major传0进去表示要让内核帮我们自动分配一个合适的空白的没被使用的主设备号
	// 内核如果成功分配就会返回分配的主设备好；如果分配失败会返回负数

	mymajor = register_chrdev(0, MYNAME, &test_fops);

	if (mymajor < 0)
	{
        printk(KERN_ERR "register_chrdev fail\n");
        return -EINVAL;
	}

	printk(KERN_INFO "register_chrdev success... mymajor = %d.\n", mymajor);
	return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");
	// 在module_exit宏调用的函数中去注销字符设备驱动
	unregister_chrdev(mymajor, MYNAME);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```
