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
- modinfo（module information，模块信息），功能是打印出一个内核模块的自带信息，用法是 modinfo xxx.ko。
- rmmod（remove module，卸载模块），功能是从当前内核中卸载一个已经安装了的模块，用法是 rmmod xxx（注意卸载模块时只需要输入模块名即可，不能加.ko后缀）

### 2.2 模块的安装

先 lsmod 再 insmod 看安装前后系统内模块记录。实践测试标明内核会将最新安装的模块放在lsmod 显示的最前面。

- insmod与module_init宏

模块源代码中用module_init宏声明了一个函数（在我们这个例子里是chrdev_init函数），作用就是指定 chrdev_init 这个函数和 insmod 命令绑定起来，也就是说当我们 insmod module_test.ko 时，insmod 命令内部实际执行的操作就是帮我们调用 chrdev_init 函数。

照此分析，那 insmod 时就应该能看到 chrdev_init 中使用 printk 打印出来的一个 chrdev_init 字符串，但是实际没看到。原因是ubuntu中拦截了，在 ubuntu 中使用 dmesg 命令就可以看到了。

模块安装时 insmod 内部除了帮我们调用 module_init 宏所声明的函数外，实际还做了一些别的事（譬如 lsmod 能看到多了一个模块也是 insmod 帮我们在内部做了记录），但是我们就不用管了。

- 模块的版本信息

使用modinfo查看模块的版本信息，内核 zImage 中也有一个确定的版本信息。

insmod 时模块的 vermagic 必须和内核的相同，否则不能安装，报错信息为：insmod: ERROR: could not insert module module_test.ko: Invalid module format

模块的版本信息是为了保证模块和内核的兼容性，是一种安全措施。如何保证模块的 vermagic和内核的 vermagic 一致？编译模块的内核源码树就是我们编译正在运行的这个内核的那个内核源码树即可。说白了就是模块和内核要同出一门。

- 模块卸载
module_exit 和 rmmod 的对应关系，lsmod 查看 rmmod 前后系统的模块记录变化。

- 模块中常用宏

MODULE_LICENSE，模块的许可证。一般声明为 GPL 许可证，而且最好不要少，否则可能会出现莫名其妙的错误（譬如一些明显存在的函数提升找不到）。

MODULE_AUTHOR

MODULE_DESCRIPTION

MODULE_ALIAS

- 函数修饰符

`__init`，本质上是个宏定义，在内核源代码中就有`#define __init xxxx`。这个`__init` 的作用就是将被他修饰的函数放入 `.init.text` 段中去（本来默认情况下函数是被放入.text段中）。

整个内核中的所有的这类函数都会被链接器链接放入 `.init.text` 段中，所以所有的内核模块的 `__init` 修饰的函数其实是被统一放在一起的。内核启动时统一会加载 `.init.text` 段中的这些模块安装函数，加载完后就会把这个段给释放掉以节省内存。

### 2.3 printk 函数详解

printk 在内核源码中用来打印信息的函数，用法和 printf 非常相似。

printk 和 printf 最大的差别：printf 是 C 库函数，是在应用层编程中使用的，不能在 linux 内核源代码中使用；printk 是 linux 内核源代码中自己封装出来的一个打印函数，是内核源码中的一个普通函数，只能在内核源码范围内使用，不能在应用编程中使用。

printk 相比 printf 来说还多了个**打印级别的设置**。printk 的打印级别是用来控制 printk 打印的这条信息是否在终端上显示的。应用程序中的调试信息要么全部打开要么全部关闭，一般用条件编译来实现（DEBUG 宏），但是在内核中，因为内核非常庞大，打印信息非常多，有时候整体调试内核时打印信息要么太多找不到想要的要么一个没有没法调试。所以才有了打印级别这个概念。

操作系统的命令行中也有一个打印信息级别属性，值为 0-7。当前操作系统中执行 printk 的时候会去对比 printk 中的打印级别和我的命令行中设置的打印级别，小于我的命令行设置级别的信息会被放行打印出来，大于的就被拦截的。譬如我的 ubuntu 中的打印级别默认是 4，那么 printk 中设置的级别比 4 小的就能打印出来，比 4 大的就不能打印出来。

ubuntu 中这个 printk 的打印级别控制没法实践，ubuntu 中不管你把级别怎么设置都不能直接打印出来，必须 dmesg 命令去查看。

### 2.4 驱动模块中的头文件

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
### 3.2 应用程序调用驱动

- 创建驱动设备文件

何为设备文件：设备文件的关键信息是：设备号 = 主设备号 + 次设备号，使用 `ls -l` 去查看设备文件，就可以得到这个设备文件对应的主次设备号。

使用 `mknod` 创建设备文件：`mknod /dev/xxx c 主设备号 次设备号`

- 编写应用来测试驱动

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE    "/dev/test"         // 刚才mknod创建的设备文件名

int main(void)
{
    int fd = -1;

    fd = open(FILE, O_RDWR);
    if (fd < 0)
    {
        printf("open %s error.\n", FILE);
        return -1;
    }
    printf("open %s success..\n", FILE);

    // 读写文件

    // 关闭文件
    close(fd);
    
    return 0;
}
```

### 3.3 添加驱动读写接口

- 添加读写接口

应用和驱动之间的数据交换，使用 copy_from_user，用来将数据从用户空间复制到内核空间 copy_to_user 。

copy_from_user 函数的返回值定义，和常规有点不同。返回值如果成功复制则返回 0，如果 不成功复制则返回尚未成功复制剩下的字节数。

注意：复制是和 mmap 的映射相对应去区分的。

- 应用程序 app

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define FILE    "/dev/test"         // 刚才mknod创建的设备文件名

char buf[100];

int main(void)
{
    int fd = -1;
    
    fd = open(FILE, O_RDWR);
    if (fd < 0)
    {
        printf("open %s error.\n", FILE);
        return -1;
    }
    printf("open %s success..\n", FILE);
    
    // 读写文件
    write(fd, "helloworld2222", 14);
    read(fd, buf, 100);
    printf("get str: %s.\n", buf);
    
    // 关闭文件
    close(fd);
    
    return 0;
}
```

- 驱动程序

```c
#include <linux/module.h>       // module_init  module_exit
#include <linux/init.h>         // __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>



#define MYMAJOR     200
#define MYNAME      "testchar"

int mymajor;

char kbuf[100];         // 内核空间的buf


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

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_read\n");
    
    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");
    
    
    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    //memcpy(kbuf, ubuf);       // 不行，因为2个不在一个地址空间中
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");

    // 真正的驱动中，数据从应用层复制到驱动中后，我们就要根据这个数据
    // 去写硬件完成硬件的操作。所以这下面就应该是操作硬件的代码
    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner      = THIS_MODULE,              // 惯例，直接写即可
    
    .open       = test_chrdev_open,         // 将来应用open打开这个设备时实际调用的
    .release    = test_chrdev_release,      // 就是这个.open对应的函数
    .write      = test_chrdev_write,
    .read       = test_chrdev_read,
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

// MODULE_xxx 这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");              // 描述模块的许可证
MODULE_AUTHOR("SummerGift");        // 描述模块的作者
MODULE_DESCRIPTION("module test");  // 描述模块的介绍信息
MODULE_ALIAS("alias xxx");          // 描述模块的别名信息
```

### 3.4 在驱动中操控硬件

- 和裸机中操控硬件不变的地方

硬件物理原理不变、硬件操作接口（寄存器）不变、硬件操作代码不变。

- 变化的地方

寄存器地址不同，原来是直接用物理地址，现在需要用该物理地址在内核虚拟地址空间相对应的虚拟地址。寄存器的物理地址是 CPU 设计时决定的，从 datasheet 中查找到的。

编程方法不同，裸机中习惯直接用函数指针操作寄存器地址，而 kernel 中习惯用封装好的 io 读写函数来操作寄存器，以实现最大程度可移植性。

- 内核的虚拟地址映射方法

内核中有2套虚拟地址映射方法：动态和静态。

静态映射方法的特点是，内核移植时以代码的形式硬编码，如果要更改必须改源代码后重新编译内核。在内核启动时建立静态映射表，到内核关机时销毁，中间对于移植好的内核一直有效，你用不用他都在那里。

动态映射方法的特点是，驱动程序根据需要随时动态的建立映射、使用、销毁映射，映射是短期临时的。

- 如何选择虚拟地址映射方法

两种映射并不排他，可以同时使用。

静态映射类似于 C 语言中全局变量，动态方式类似于 C 语言中 malloc 堆内存。静态映射的好处是执行效率高，坏处是始终占用虚拟地址空间。动态映射的好处是按需使用虚拟地址空间，坏处是每次使用前后都需要代码去建立映射&销毁映射（还得学会使用那些内核函数的使用）。

### 3.5 静态映射操作 LED

#### 3.5.1 静态映射基本概念

- 不同版本内核中静态映射表位置、文件名可能不同
- 不同SoC的静态映射表位置、文件名可能不同
- 所谓映射表其实就是头文件中的宏定义

#### 3.5.2 内核静态映射表

主映射表位于：`arch/arm/plat-s5p/include/plat/map-s5p.h`。

CPU 在安排寄存器地址时不是随意乱序分布的，而是按照模块去区分的。每一个模块内部的很多个寄存器的地址是连续的。所以内核在定义寄存器地址时都是先找到基地址，然后再用基地址+偏移量来寻找具体的一个寄存器。

map-s5p.h中定义的就是要用到的几个模块的寄存器基地址。map-s5p.h 中定义的是模块的寄存器基地址的虚拟地址。

虚拟地址基地址定义在：`arch/arm/plat-samsung/include/plat/map-base.h`

`#define S3C_ADDR_BASE	(0xFD000000)`	


移植时确定的静态映射表的基地址，表中的所有虚拟地址都是以这个地址+偏移量来指定的。

GPIO相关的主映射表位于：`arch/arm/mach-s5pv210/include/mach/regs-gpio.h`，表中是GPIO的各个端口的基地址的定义。

GPIO 的具体寄存器定义位于：`arch/arm/mach-s5pv210/include/mach/gpio-bank.h`。

#### 3.5.2 代码实战

- app

```c
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define FILE	"/dev/test"			// 刚才mknod创建的设备文件名

char buf[100];

int main(void)
{
	int fd = -1;
	int i = 0;

	fd = open(FILE, O_RDWR);
	if (fd < 0)
	{
		printf("open %s error.\n", FILE);
		return -1;
	}
	printf("open %s success..\n", FILE);

	while (1)
	{
		memset(buf, 0 , sizeof(buf));
		printf("please type on | off \n");
		scanf("%s", buf);
		if (!strcmp(buf, "on"))
		{
			write(fd, "1", 1);
		}
		else if (!strcmp(buf, "off"))
		{
			write(fd, "0", 1);
		}
		else if (!strcmp(buf, "flash"))
		{
			for (i=0; i<3; i++)
			{
				write(fd, "1", 1);
				sleep(1);
				write(fd, "0", 1);
				sleep(1);
			}
		}	
		else if (!strcmp(buf, "quit"))
		{
			break;
		}
	}

	// 关闭文件
	close(fd);
	
	return 0;
}
```

- 驱动

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>


#define MYMAJOR		200
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

int mymajor;

char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
	// 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
	// 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
	printk(KERN_INFO "test_chrdev_open\n");
	
	rGPJ0CON = 0x11111111;
	rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
	
	return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_release\n");
	
	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	
	return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_read\n");
	
	ret = copy_to_user(ubuf, kbuf, count);
	if (ret)
	{
		printk(KERN_ERR "copy_to_user fail\n");
		return -EINVAL;
	}
	printk(KERN_INFO "copy_to_user success..\n");
	
	
	return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
	size_t count, loff_t *ppos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_write\n");

	// 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
	//memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
	memset(kbuf, 0, sizeof(kbuf));
	ret = copy_from_user(kbuf, ubuf, count);
	if (ret)
	{
		printk(KERN_ERR "copy_from_user fail\n");
		return -EINVAL;
	}
	printk(KERN_INFO "copy_from_user success..\n");
	
	if (kbuf[0] == '1')
	{
		rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
	}
	else if (kbuf[0] == '0')
	{
		rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	}
	
	return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
	.owner		= THIS_MODULE,				// 惯例，直接写即可
	
	.open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
	.release	= test_chrdev_release,		// 就是这个.open对应的函数
	.write 		= test_chrdev_write,
	.read		= test_chrdev_read,
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
	
//	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");		// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

### 3.6 动态映射操作 LED

#### 3.6.1 建立动态映射

- request_mem_region，向内核申请（报告）需要映射的内存资源。
- ioremap，真正用来实现映射，传给他物理地址他给你映射返回一个虚拟地址

#### 3.6.2 销毁动态映射

- iounmap
- release_mem_region

注意：映射建立时，是要先申请再映射，然后使用。使用完要解除映射时要先解除映射再释放申请。在实际编写代码时，既可以一个一个寄存器进行独立映射，也可以多个寄存器一起映射。

#### 3.6.3 代码实践
```
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>


#define MYMAJOR		200
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA	0xe0200240
#define GPJ0DAT_PA 	0xe0200244

unsigned int *pGPJ0CON;
unsigned int *pGPJ0DAT;

int mymajor;
char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
    // 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    // 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
    printk(KERN_INFO "test_chrdev_open\n");
    
    rGPJ0CON = 0x11111111;
    rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
    
    return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");
    
    rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_read\n");
    
    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");
    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    //memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");
    
    if (kbuf[0] == '1')
    {
        rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
    }
    else if (kbuf[0] == '0')
    {
        rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    }

    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner		= THIS_MODULE,				// 惯例，直接写即可
    
    .open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
    .release	= test_chrdev_release,		// 就是这个.open对应的函数
    .write 		= test_chrdev_write,
    .read		= test_chrdev_read,
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
    
    
    // 使用动态映射的方式来操作寄存器
    if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
        return -EINVAL;
    if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0CON"))
        return -EINVAL;
    
    pGPJ0CON = ioremap(GPJ0CON_PA, 4);
    pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);
    
    *pGPJ0CON = 0x11111111;
    *pGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮

    return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");

    *pGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));	
    
    // 解除映射
    iounmap(pGPJ0CON);
    iounmap(pGPJ0DAT);
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);
    
    // 在module_exit宏调用的函数中去注销字符设备驱动
    unregister_chrdev(mymajor, MYNAME);
    
//	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");        // 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

## 4. 字符设备驱动高级

### 4.1 字符设备新注册接口
- 新接口与老接口

老接口：register_chrdev
新接口：register_chrdev_region/alloc_chrdev_region + cdev

- cdev

相关函数：cdev_alloc、cdev_init、cdev_add、cdev_del

- 设备号

主设备号和次设备号、dev_t 类型、MKDEV、MAJOR、MINOR 三个宏。

- 代码实践

使用 `register_chrdev_region + cdev_init + cdev_add` 进行字符设备驱动注册。

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/cdev.h>

#define MYMAJOR		200
#define MYCNT		1
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA	0xe0200240
#define GPJ0DAT_PA 	0xe0200244

unsigned int *pGPJ0CON;
unsigned int *pGPJ0DAT;

int mymajor;
static dev_t mydev;
static struct cdev test_cdev;

char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
    // 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    // 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
    printk(KERN_INFO "test_chrdev_open\n");
    
    rGPJ0CON = 0x11111111;
    rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
    
    return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");
    
    rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_read\n");
    
    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");

    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    //memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");
    
    if (kbuf[0] == '1')
    {
        rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
    }
    else if (kbuf[0] == '0')
    {
        rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    }

    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner		= THIS_MODULE,				// 惯例，直接写即可
    .open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
    .release	= test_chrdev_release,		// 就是这个.open对应的函数
    .write 		= test_chrdev_write,
    .read		= test_chrdev_read,
};

// 模块安装函数
static int __init chrdev_init(void)
{	
    int retval;
    
    printk(KERN_INFO "chrdev_init helloworld init\n");

    // 使用新的cdev接口来注册字符设备驱动
    // 新的接口注册字符设备驱动需要2步
    
    // 第1步：注册/分配主次设备号
    mydev = MKDEV(MYMAJOR, 0);
    retval = register_chrdev_region(mydev, MYCNT, MYNAME);
    if (retval) {
        printk(KERN_ERR "Unable to register minors for %s\n", MYNAME);
        return -EINVAL;
    }
    printk(KERN_INFO "register_chrdev_region success\n");
    // 第2步：注册字符设备驱动
    cdev_init(&test_cdev, &test_fops);
    retval = cdev_add(&test_cdev, mydev, MYCNT);
    if (retval) {
        printk(KERN_ERR "Unable to cdev_add\n");
        return -EINVAL;
    }
    printk(KERN_INFO "cdev_add success\n");

    // 使用动态映射的方式来操作寄存器
    if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
        return -EINVAL;
    if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0CON"))
        return -EINVAL;
    
    pGPJ0CON = ioremap(GPJ0CON_PA, 4);
    pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);
    
    *pGPJ0CON = 0x11111111;
    *pGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
    
    return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");

    *pGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));	
    
    // 解除映射
    iounmap(pGPJ0CON);
    iounmap(pGPJ0DAT);
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);

/*	
    // 在module_exit宏调用的函数中去注销字符设备驱动
    unregister_chrdev(mymajor, MYNAME);
*/	

    // 使用新的接口来注销字符设备驱动
    // 注销分2步：
    // 第一步真正注销字符设备驱动用cdev_del
    cdev_del(&test_cdev);
    // 第二步去注销申请的主次设备号
    unregister_chrdev_region(mydev, MYCNT);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");		// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

### 4.2 自动分配设备号

- register_chrdev_region 

事先知道要使用的主、次设备号时使用。要先查看 `cat /proc/devices` 去查看没有使用的。更简便、更智能的方法是让内核给我们自动分配一个主设备号，使用 `alloc_chrdev_region` 就可以自动分配了。

自动分配的设备号，我们必须去知道他的主次设备号，否则后面没法去 mknod 创建他对应的设备文件。

- 得到分配的主设备号和次设备号

使用 MAJOR 宏和 MINOR 宏从 dev_t 得到 major 和 minor，反过来使用 MKDEV 宏从 major 和 minor 得到dev_t，使用这些宏的代码具有可移植性。

- 中途出错的倒影式错误处理方法

内核中很多函数中包含了很多个操作，这些操作每一步都有可能出错，而且出错后后面的步骤就没有进行下去的必要性了。

- 使用 cdev_alloc

从内存角度体会 cdev_alloc 用与不用的差别，这就是非面向对象的语言和面向对象的代码。

- 代码实践

```c
#include <linux/module.h>       // module_init  module_exit
#include <linux/init.h>         // __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>     // arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/cdev.h>

#define MYCNT       1
#define MYNAME      "testchar"

#define GPJ0CON     S5PV210_GPJ0CON
#define GPJ0DAT     S5PV210_GPJ0DAT

#define rGPJ0CON    *((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT    *((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA  0xe0200240
#define GPJ0DAT_PA  0xe0200244

unsigned int *pGPJ0CON;
unsigned int *pGPJ0DAT;

//int mymajor;
static dev_t mydev;
static struct cdev test_cdev;

char kbuf[100];         // 内核空间的buf


static int test_chrdev_open(struct inode *inode, struct file *file)
{
    // 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    // 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
    printk(KERN_INFO "test_chrdev_open\n");
    
    rGPJ0CON = 0x11111111;
    rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));      // 亮
    
    return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");
    
    rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_read\n");
    
    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");
    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    // memcpy(kbuf, ubuf);      // 不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");
    
    if (kbuf[0] == '1')
    {
        rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
    }
    else if (kbuf[0] == '0')
    {
        rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    }

    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner      = THIS_MODULE,              // 惯例，直接写即可
    
    .open       = test_chrdev_open,         // 将来应用open打开这个设备时实际调用的
    .release    = test_chrdev_release,      // 就是这个.open对应的函数
    .write      = test_chrdev_write,
    .read       = test_chrdev_read,
};

// 模块安装函数
static int __init chrdev_init(void)
{   
    int retval;
    
    printk(KERN_INFO "chrdev_init helloworld init\n");

    // 使用新的cdev接口来注册字符设备驱动
    // 新的接口注册字符设备驱动需要2步
    
    // 第1步：分配主次设备号
    retval = alloc_chrdev_region(&mydev, 12, MYCNT, MYNAME);
    if (retval < 0) 
    {
        printk(KERN_ERR "Unable to alloc minors for %s\n", MYNAME);
        goto flag1;
    }
    printk(KERN_INFO "alloc_chrdev_region success\n");
    printk(KERN_INFO "major = %d, minor = %d.\n", MAJOR(mydev), MINOR(mydev));

    // 第2步：注册字符设备驱动
    cdev_init(&test_cdev, &test_fops);
    retval = cdev_add(&test_cdev, mydev, MYCNT);
    if (retval) {
        printk(KERN_ERR "Unable to cdev_add\n");
        goto flag2;
    }
    printk(KERN_INFO "cdev_add success\n");

    // 使用动态映射的方式来操作寄存器
    if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
        goto flag3;

    if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0CON"))
        goto flag3;

    pGPJ0CON = ioremap(GPJ0CON_PA, 4);
    pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);

    *pGPJ0CON = 0x11111111;
    *pGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));     // 亮

    return 0;

// 如果第4步才出错跳转到这里来   
flag4:
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);

// 如果第3步才出错跳转到这里来
flag3:
    cdev_del(&test_cdev);

// 如果第2步才出错跳转到这里来
flag2:
// 在这里把第1步做成功的东西给注销掉
    unregister_chrdev_region(mydev, MYCNT);
// 如果第1步才出错跳转到这里来
flag1:  
    return -EINVAL;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");

    *pGPJ0DAT = ((1<<3) | (1<<4) | (1<<5)); 

    // 解除映射
    iounmap(pGPJ0CON);
    iounmap(pGPJ0DAT);
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);

/*  
    // 在module_exit宏调用的函数中去注销字符设备驱动
    unregister_chrdev(mymajor, MYNAME);
*/  

    // 使用新的接口来注销字符设备驱动
    // 注销分2步：
    // 第一步真正注销字符设备驱动用cdev_del
    cdev_del(&test_cdev);
    // 第二步去注销申请的主次设备号
    unregister_chrdev_region(mydev, MYCNT);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");              // 描述模块的许可证
MODULE_AUTHOR("SummerGift");        // 描述模块的作者
MODULE_DESCRIPTION("module test");  // 描述模块的介绍信息
MODULE_ALIAS("alias xxx");          // 描述模块的别名信息
```

### 4.3 自动创建设备文件

在先前的测试中，都需要手动创建字符设备文件，这样有些麻烦，其实可以编写代码来自动创建字符设备文件。当卸载驱动模块的时候，自动删除字符设备文件。

解决方案是 udev（嵌入式中用的是 mdev），他是应用层的一个应用程序，内核驱动和应用层 udev 之间有一套信息传输机制（netlink协议），应用层启用 udev，内核驱动中使用相应接口。

驱动注册和注销时信息会被传给 udev，由 udev 在应用层进行设备文件的创建和删除。

内核驱动设备类相关函数：class_create、device_create。

- 代码实战

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>
#include <linux/cdev.h>
#include <linux/device.h>

//#define MYMAJOR		200
#define MYCNT		1
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA	0xe0200240
#define GPJ0DAT_PA 	0xe0200244

unsigned int *pGPJ0CON;
unsigned int *pGPJ0DAT;

//int mymajor;
static dev_t mydev;
//static struct cdev test_cdev;
static struct cdev *pcdev;
static struct class *test_class;

char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
    // 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    // 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
    printk(KERN_INFO "test_chrdev_open\n");

    rGPJ0CON = 0x11111111;
    rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮

    return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");

    rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));

    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;

    printk(KERN_INFO "test_chrdev_read\n");

    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");


    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;

    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    //memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");

    if (kbuf[0] == '1')
    {
        rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
    }
    else if (kbuf[0] == '0')
    {
        rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    }

    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner		= THIS_MODULE,				// 惯例，直接写即可

    .open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
    .release	= test_chrdev_release,		// 就是这个.open对应的函数
    .write 		= test_chrdev_write,
    .read		= test_chrdev_read,
};

// 模块安装函数
static int __init chrdev_init(void)
{
    int retval;

    printk(KERN_INFO "chrdev_init helloworld init\n");

    // 使用新的cdev接口来注册字符设备驱动
    // 新的接口注册字符设备驱动需要2步

    // 第1步：分配主次设备号
    retval = alloc_chrdev_region(&mydev, 12, MYCNT, MYNAME);
    if (retval < 0)
    {
        printk(KERN_ERR "Unable to alloc minors for %s\n", MYNAME);
        goto flag1;
    }
    printk(KERN_INFO "alloc_chrdev_region success\n");
    printk(KERN_INFO "major = %d, minor = %d.\n", MAJOR(mydev), MINOR(mydev));


    // 第2步：注册字符设备驱动
    pcdev = cdev_alloc();			// 给pcdev分配内存，指针实例化
    //cdev_init(pcdev, &test_fops);
    pcdev->owner = THIS_MODULE;
    pcdev->ops = &test_fops;

    retval = cdev_add(pcdev, mydev, MYCNT);
    if (retval) {
        printk(KERN_ERR "Unable to cdev_add\n");
        goto flag2;
    }
    printk(KERN_INFO "cdev_add success\n");

    // 注册字符设备驱动完成后，添加设备类的操作，以让内核帮我们发信息
    // 给udev，让udev自动创建和删除设备文件
    test_class = class_create(THIS_MODULE, "aston_class");
    if (IS_ERR(test_class))
        return -EINVAL;
    // 最后1个参数字符串，就是我们将来要在/dev目录下创建的设备文件的名字
    // 所以我们这里要的文件名是/dev/test
    device_create(test_class, NULL, mydev, NULL, "test111");

    // 使用动态映射的方式来操作寄存器
    if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
//		return -EINVAL;
        goto flag3;
    if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0CON"))
//		return -EINVAL;
        goto flag3;

    pGPJ0CON = ioremap(GPJ0CON_PA, 4);
    pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);

    *pGPJ0CON = 0x11111111;
    *pGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮

    //goto flag0:
    return 0;

// 如果第4步才出错跳转到这里来
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);

// 如果第3步才出错跳转到这里来
flag3:
    cdev_del(pcdev);

// 如果第2步才出错跳转到这里来
flag2:
    // 在这里把第1步做成功的东西给注销掉
    unregister_chrdev_region(mydev, MYCNT);
// 如果第1步才出错跳转到这里来
flag1:
    return -EINVAL;
//flag0:
//	return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");

    *pGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));

    // 解除映射
    iounmap(pGPJ0CON);
    iounmap(pGPJ0DAT);
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);

/*
    // 在module_exit宏调用的函数中去注销字符设备驱动
    unregister_chrdev(mymajor, MYNAME);
*/

    device_destroy(test_class, mydev);
    class_destroy(test_class);

    // 使用新的接口来注销字符设备驱动
    // 注销分2步：
    // 第一步真正注销字符设备驱动用cdev_del
    cdev_del(pcdev);
    // 第二步去注销申请的主次设备号
    unregister_chrdev_region(mydev, MYCNT);
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");		// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

### 4.4 设备类代码分析

可以通过 /sys/class/xxx/ 中的文件来获取设备类的相关信息。

- 设备类创建

```
class_create
	__class_create
		__class_register
			kset_register
				kobject_uevent
```

- 通过设备类创建设备

```
device_create
	device_create_vargs
		kobject_set_name_vargs
		device_register
			device_add
				kobject_add
					device_create_file
					device_create_sys_dev_entry
					devtmpfs_create_node
					device_add_class_symlinks
					device_add_attrs
					device_pm_add
					kobject_uevent
```

### 4.5 静态映射表建立过程

- 建立映射表的三个关键部分

映射表具体物理地址和虚拟地址的值相关的宏定义。

- 映射表建立函数。

该函数负责由(1)中的映射表来建立linux内核的页表映射关系。

在 `kernel/arch/arm/mach-s5pv210/mach-smdkc110.c` 中的 `smdkc110_map_io` 函数
```
  smdkc110_map_io
  	s5p_init_io
  		iotable_init
```

经过分析，真正的内核移植时给定的静态映射表在 `arch/arm/plat-s5p/cpu.c` 中的`s5p_iodesc`，本质是一个结构体数组，数组中每一个元素就是一个映射，这个映射描述了一段物理地址到虚拟地址之间的映射。这个结构体数组所记录的几个映射关系被 `iotable_init` 所使用，该函数负责将这个结构体数组格式的表建立成 MMU 所能识别的页表映射关系，这样在开机后可以直接使用相对应的虚拟地址来访问对应的物理地址。

- 开机时调用映射表建立函数
开机时（kernel启动时）`smdkc110_map_io` 怎么被调用的？
```
start_kernel
	setup_arch
		paging_init
			devicemaps_init
```

```
if (mdesc->map_io)
		mdesc->map_io();
```

### 4.6 常规寄存器操作方式

- 使用结构体封装的方式来操作寄存器

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>

typedef struct GPJ0REG
{
    volatile unsigned int gpj0con;
    volatile unsigned int gpj0dat;
}gpj0_reg_t;


#define MYMAJOR		200
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

//#define GPJ0CON_PA	0xe0200240
//#define GPJ0DAT_PA 	0xe0200244
#define GPJ0_REGBASE	0xe0200240

//unsigned int *pGPJ0CON;
//unsigned int *pGPJ0DAT;
gpj0_reg_t *pGPJ0REG;
int mymajor;
char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
    // 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
    // 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
    printk(KERN_INFO "test_chrdev_open\n");
    
    rGPJ0CON = 0x11111111;
    rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
    
    return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
    printk(KERN_INFO "test_chrdev_release\n");
    
    rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    
    return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_read\n");
    
    ret = copy_to_user(ubuf, kbuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_to_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_to_user success..\n");
    
    
    return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
    size_t count, loff_t *ppos)
{
    int ret = -1;
    
    printk(KERN_INFO "test_chrdev_write\n");

    // 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
    //memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
    memset(kbuf, 0, sizeof(kbuf));
    ret = copy_from_user(kbuf, ubuf, count);
    if (ret)
    {
        printk(KERN_ERR "copy_from_user fail\n");
        return -EINVAL;
    }
    printk(KERN_INFO "copy_from_user success..\n");
    
    if (kbuf[0] == '1')
    {
        rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
    }
    else if (kbuf[0] == '0')
    {
        rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
    }
    return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
    .owner		= THIS_MODULE,				// 惯例，直接写即可
    
    .open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
    .release	= test_chrdev_release,		// 就是这个.open对应的函数
    .write 		= test_chrdev_write,
    .read		= test_chrdev_read,
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
    
/*	
    // 使用动态映射的方式来操作寄存器
    if (!request_mem_region(GPJ0CON_PA, 4, "GPJ0CON"))
        return -EINVAL;
    if (!request_mem_region(GPJ0DAT_PA, 4, "GPJ0CON"))
        return -EINVAL;
    
    pGPJ0CON = ioremap(GPJ0CON_PA, 4);
    pGPJ0DAT = ioremap(GPJ0DAT_PA, 4);
    
    *pGPJ0CON = 0x11111111;
    *pGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
*/
    // 2步完成了映射
    if (!request_mem_region(GPJ0_REGBASE, sizeof(gpj0_reg_t), "GPJ0REG"))
        return -EINVAL;
    pGPJ0REG = ioremap(GPJ0_REGBASE, sizeof(gpj0_reg_t));
    // 映射之后用指向结构体的指针来进行操作
    // 指针使用->结构体内元素的方式来操作各个寄存器
    pGPJ0REG->gpj0con = 0x11111111;
    pGPJ0REG->gpj0dat = ((0<<3) | (0<<4) | (0<<5));		// 亮

    return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
    printk(KERN_INFO "chrdev_exit helloworld exit\n");

//	*pGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));	
    pGPJ0REG->gpj0dat = ((1<<3) | (1<<4) | (1<<5));	
    
    // 解除映射
/*
    iounmap(pGPJ0CON);
    iounmap(pGPJ0DAT);
    release_mem_region(GPJ0CON_PA, 4);
    release_mem_region(GPJ0DAT_PA, 4);
*/
    iounmap(pGPJ0REG);
    release_mem_region(GPJ0_REGBASE, sizeof(gpj0_reg_t));
    
    // 在module_exit宏调用的函数中去注销字符设备驱动
    unregister_chrdev(mymajor, MYNAME);
    
//	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

- 使用内核内核提供的读写寄存器接口，writel 和 readl ，iowrite32 和 ioread32

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>		// arch/arm/mach-s5pv210/include/mach/gpio-bank.h
#include <linux/string.h>
#include <linux/io.h>
#include <linux/ioport.h>

#define MYMAJOR		200
#define MYNAME		"testchar"

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

#define rGPJ0CON	*((volatile unsigned int *)GPJ0CON)
#define rGPJ0DAT	*((volatile unsigned int *)GPJ0DAT)

#define GPJ0CON_PA	0xe0200240
#define GPJ0DAT_PA 	0xe0200244

#define S5P_GPJ0REG(x)		(x)
#define S5P_GPJ0CON			S5P_GPJ0REG(0)
#define S5P_GPJ0DAT			S5P_GPJ0REG(4)

unsigned int *pGPJ0CON;
unsigned int *pGPJ0DAT;

static void __iomem *baseaddr;			// 寄存器的虚拟地址的基地址

int mymajor;

char kbuf[100];			// 内核空间的buf

static int test_chrdev_open(struct inode *inode, struct file *file)
{
	// 这个函数中真正应该放置的是打开这个设备的硬件操作代码部分
	// 但是现在暂时我们写不了这么多，所以用一个printk打印个信息来做代表。
	printk(KERN_INFO "test_chrdev_open\n");
	
	rGPJ0CON = 0x11111111;
	rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));		// 亮
	
	return 0;
}

static int test_chrdev_release(struct inode *inode, struct file *file)
{
	printk(KERN_INFO "test_chrdev_release\n");
	
	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	
	return 0;
}

ssize_t test_chrdev_read(struct file *file, char __user *ubuf, size_t count, loff_t *ppos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_read\n");
	
	ret = copy_to_user(ubuf, kbuf, count);
	if (ret)
	{
		printk(KERN_ERR "copy_to_user fail\n");
		return -EINVAL;
	}
	printk(KERN_INFO "copy_to_user success..\n");
	
	
	return 0;
}

// 写函数的本质就是将应用层传递过来的数据先复制到内核中，然后将之以正确的方式写入硬件完成操作。
static ssize_t test_chrdev_write(struct file *file, const char __user *ubuf,
	size_t count, loff_t *ppos)
{
	int ret = -1;
	
	printk(KERN_INFO "test_chrdev_write\n");

	// 使用该函数将应用层传过来的ubuf中的内容拷贝到驱动空间中的一个buf中
	//memcpy(kbuf, ubuf);		// 不行，因为2个不在一个地址空间中
	memset(kbuf, 0, sizeof(kbuf));
	ret = copy_from_user(kbuf, ubuf, count);
	if (ret)
	{
		printk(KERN_ERR "copy_from_user fail\n");
		return -EINVAL;
	}
	printk(KERN_INFO "copy_from_user success..\n");
	
	if (kbuf[0] == '1')
	{
		rGPJ0DAT = ((0<<3) | (0<<4) | (0<<5));
	}
	else if (kbuf[0] == '0')
	{
		rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
	}
	
	return 0;
}

// 自定义一个file_operations结构体变量，并且去填充
static const struct file_operations test_fops = {
	.owner		= THIS_MODULE,				// 惯例，直接写即可
	
	.open		= test_chrdev_open,			// 将来应用open打开这个设备时实际调用的
	.release	= test_chrdev_release,		// 就是这个.open对应的函数
	.write 		= test_chrdev_write,
	.read		= test_chrdev_read,
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
	
	// 测试3：用1次ioremap映射多个寄存器得到虚拟地址，测试成功
	if (!request_mem_region(GPJ0CON_PA, 8, "GPJ0BASE"))
		return -EINVAL;

	baseaddr = ioremap(GPJ0CON_PA, 8);
	
	writel(0x11111111, baseaddr + S5P_GPJ0CON);
	writel(((0<<3) | (0<<4) | (0<<5)), baseaddr + S5P_GPJ0DAT);

	return 0;
}

// 模块下载函数
static void __exit chrdev_exit(void)
{
	printk(KERN_INFO "chrdev_exit helloworld exit\n");

	writel(((1<<3) | (1<<4) | (1<<5)), baseaddr + S5P_GPJ0DAT);	
	
	iounmap(baseaddr);
	release_mem_region(baseaddr, 8);
	
	// 在module_exit宏调用的函数中去注销字符设备驱动
	unregister_chrdev(mymajor, MYNAME);
	
//	rGPJ0DAT = ((1<<3) | (1<<4) | (1<<5));
}

module_init(chrdev_init);
module_exit(chrdev_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");				// 描述模块的许可证
MODULE_AUTHOR("SummerGift");				// 描述模块的作者
MODULE_DESCRIPTION("module test");	// 描述模块的介绍信息
MODULE_ALIAS("alias xxx");			// 描述模块的别名信息
```

## 5. LED 驱动框架

### 5.1 什么是驱动框架

内核中驱动部分维护者针对每个种类的驱动设计一套成熟的、标准的、典型的驱动实现，然后把不同厂家的同类硬件驱动中相同的部分抽出来自己实现好，再把不同部分留出接口给具体的驱动开发工程师来实现，这就叫驱动框架。

内核维护者在内核中设计了一些统一管控系统资源的体系，这些体系让内核能够对资源在各个驱动之间的使用统一协调和分配，保证整个内核的稳定健康运行。譬如系统中所有的 GPIO 就属于系统资源，每个驱动模块如果要使用某个 GPIO 就要先调用特殊的接口先申请，申请到后使用，使用完后要释放。又譬如中断号也是一种资源，驱动在使用前也必须去申请。这也是驱动框架的组成部分。

一些特定的接口函数、一些特定的数据结构，这些是驱动框架的直接表现。

### 5.2 LED 内核驱动框架
#### 5.2.1 相关文件

- drivers/leds 目录，这个目录就是驱动框架规定的 LED 这种硬件的驱动应该待的地方。

- led-class.c 和 led-core.c，这两个文件加起来属于 LED 驱动框架的第一部分，这两个文件是内核开发者提供的，他们描述的是内核中所有厂家的不同 LED 硬件的相同部分的逻辑。

- leds-xxxx.c，这个文件是 LED 驱动框架的第 2 部分，是由不同厂商的驱动工程师编写添加的，厂商驱动工程师结合自己公司的硬件的不同情况来对 LED 进行操作，使用第一部分提供的接口来和驱动框架进行交互，最终实现驱动的功能。

九鼎实际未使用内核推荐的led驱动框架，drivers/char/led/x210-led.c。

#### 5.2.2 驱动框架的使用

以 leds-s3c24xx.c 为例，leds-s3c24xx.c 中通过调用 led_classdev_register 来完成 LED 驱动的注册，而 `led_classdev_register` 是在 `drivers/leds/led-class.c` 中定义的。所以其实 SoC 厂商的驱动工程师是调用内核开发者在驱动框架中提供的接口来实现自己的驱动的。

驱动框架的关键点就是：分清楚内核开发者提供了什么，驱动开发者自己要提供什么。

- 典型的驱动开发行业现状

内核开发者对驱动框架进行开发和维护、升级，对应 led-class.c 和 led-core.c。SoC 厂商的驱动工程师对设备驱动源码进行编写、调试，提供参考版本，对应 leds-s3c24xx.c。做产品的厂商的驱动工程师以 SoC 厂商提供的驱动源码为基础，来做移植和调试。

#### 5.2.3 初步分析驱动框架

- 涉及到的文件

```
led-core.c
led-class.c
```

- subsys_initcall

经过基本分析，发现LED驱动框架中内核开发者实现的部分主要是 led-class.c。led-class.c 就是一个内核模块，对led-class.c分析应该从下往上，遵从对模块的基本分析方法。

为什么LED驱动框架中内核开发者实现的部分要实现成一个模块？因为内核开发者希望这个驱动框架是可以被装载/卸载的。这样当我们内核使用者不需要这个驱动框架时可以完全去掉，需要时可以随时加上。

`subsys_initcall` 是一个宏，定义在 `linux/init.h` 中。经过对这个宏进行展开，发现这个宏的功能是：将其声明的函数放到一个特定的段：`.initcall4.init`。

```
subsys_initcall
	__define_initcall("4",fn,4)
```

分析 `module_init` 宏，可以看出它将函数放到了 `.initcall6.init` 段中。

```
module_init
	__initcall
		device_initcall
			__define_initcall("6",fn,6)
```

内核在启动过程中需要顺序的做很多事，内核如何实现按照先后顺序去做很多初始化操作。内核的解决方案就是给内核启动时要调用的所有函数归类，然后每个类按照一定的次序去调用执行。这些分类名就叫 `.initcalln.init`。n 的值从 1 到 8。内核开发者在编写内核代码时只要将函数设置合适的级别，这些函数就会被链接的时候放入特定的段，内核启动时再按照段顺序去依次执行各个段即可。

经过分析，可以看出，`subsys_initcall` 和 `module_init` 的作用是一样的，只不过前者所声明的函数要比后者在内核启动时的执行顺序更早。

- led_class_attrs

什么是 attribute，对应将来 `/sys/class/leds/` 目录里的内容，一般是文件和文件夹。这些文件其实就是 `sysfs` 开放给应用层的一些操作接口（非常类似于/dev/目录下的那些设备文件）。

attribute 的作用就是让应用程序可以通过 `/sys/class/leds/` 目录下面的属性文件来操作驱动进而操作硬件设备。attribute 其实是另一条驱动实现的路线。有区别于之前讲的 `file_operations` 那条线。

- led_classdev_register

```
led_classdev_register
	device_create
```

分析可知，`ed_classdev_register` 这个函数其实就是去创建一个属于 `leds` 这个类的一个设备。其实就是去注册一个设备。所以这个函数其实就是 `led` 驱动框架中内核开发者提供给SoC 厂家驱动开发者的一个注册驱动的接口。

当我们使用 `led` 驱动框架去编写驱动的时候，这个 `led_classdev_register` 函数的作用类似于我们之前使用 `file_operations` 方式去注册字符设备驱动时的 `register_chrdev` 函数。

### 5.3 实现独立控制 LED

```c
#include <linux/module.h>		// module_init  module_exit
#include <linux/init.h>			// __init   __exit
#include <linux/fs.h>
#include <linux/leds.h>
#include <mach/regs-gpio.h>
#include <mach/gpio-bank.h>
#include <linux/io.h>
#include <linux/ioport.h>

#define GPJ0CON		S5PV210_GPJ0CON
#define GPJ0DAT		S5PV210_GPJ0DAT

static struct led_classdev mydev1;			// 定义结构体变量
static struct led_classdev mydev2;			// 定义结构体变量
static struct led_classdev mydev3;			// 定义结构体变量

// 这个函数就是要去完成具体的硬件读写任务的
static void s5pv210_led1_set(struct led_classdev *led_cdev,
                enum led_brightness value)
{
    printk(KERN_INFO "s5pv210_led1_set\n");
    
    writel(0x11111111, GPJ0CON);
    
    // 在这里根据用户设置的值来操作硬件
    // 用户设置的值就是value
    if (value == LED_OFF)
    {
        // 用户给了个0，希望LED灭
        //writel(0x11111111, GPJ0CON);
        // 读改写三部曲
        writel((readl(GPJ0DAT) | (1<<3)), GPJ0DAT);
    }
    else
    {
        // 用户给的是非0，希望LED亮
        //writel(0x11111111, GPJ0CON);
        writel((readl(GPJ0DAT) & ~(1<<3)), GPJ0DAT);
    }
}

static void s5pv210_led2_set(struct led_classdev *led_cdev,
                enum led_brightness value)
{
    printk(KERN_INFO "s5pv2102_led_set\n");
    
    writel(0x11111111, GPJ0CON);
    
    // 在这里根据用户设置的值来操作硬件
    // 用户设置的值就是value
    if (value == LED_OFF)
    {
        // 用户给了个0，希望LED灭
        //writel(0x11111111, GPJ0CON);
        // 读改写三部曲
        writel((readl(GPJ0DAT) | (1<<4)), GPJ0DAT);
    }
    else
    {
        // 用户给的是非0，希望LED亮
        //writel(0x11111111, GPJ0CON);
        writel((readl(GPJ0DAT) & ~(1<<4)), GPJ0DAT);
    }
}

static void s5pv210_led3_set(struct led_classdev *led_cdev,
                enum led_brightness value)
{
    printk(KERN_INFO "s5pv210_led3_set\n");
    
    writel(0x11111111, GPJ0CON);
    
    // 在这里根据用户设置的值来操作硬件
    // 用户设置的值就是value
    if (value == LED_OFF)
    {
        // 用户给了个0，希望LED灭
        //writel(0x11111111, GPJ0CON);
        // 读改写三部曲
        writel((readl(GPJ0DAT) | (1<<5)), GPJ0DAT);
    }
    else
    {
        // 用户给的是非0，希望LED亮
        //writel(0x11111111, GPJ0CON);
        writel((readl(GPJ0DAT) & ~(1<<5)), GPJ0DAT);
    }
}


static int __init s5pv210_led_init(void)
{
    // 用户insmod安装驱动模块时会调用该函数
    // 该函数的主要任务就是去使用led驱动框架提供的设备注册函数来注册一个设备
    int ret = -1;
    
    // led1
    mydev1.name = "led1";
    mydev1.brightness = 255;	
    mydev1.brightness_set = s5pv210_led1_set;
    
    ret = led_classdev_register(NULL, &mydev1);
    if (ret < 0) {
        printk(KERN_ERR "led_classdev_register failed\n");
        return ret;
    }
    
    // led2
    mydev2.name = "led2";
    mydev2.brightness = 255;	
    mydev2.brightness_set = s5pv210_led2_set;
    
    ret = led_classdev_register(NULL, &mydev2);
    if (ret < 0) {
        printk(KERN_ERR "led_classdev_register failed\n");
        return ret;
    }
    
    // led3
    mydev3.name = "led3";
    mydev3.brightness = 255;	
    mydev3.brightness_set = s5pv210_led3_set;
    
    ret = led_classdev_register(NULL, &mydev3);
    if (ret < 0) {
        printk(KERN_ERR "led_classdev_register failed\n");
        return ret;
    }
    
    return 0;
}

static void __exit s5pv210_led_exit(void)
{
    led_classdev_unregister(&mydev1);
    led_classdev_unregister(&mydev2);
    led_classdev_unregister(&mydev3);
}

module_init(s5pv210_led_init);
module_exit(s5pv210_led_exit);

// MODULE_xxx这种宏作用是用来添加模块描述信息
MODULE_LICENSE("GPL");                          // 描述模块的许可证
MODULE_AUTHOR("SummerGift>");                   // 描述模块的作者
MODULE_DESCRIPTION("s5pv210 led driver");       // 描述模块的介绍信息
MODULE_ALIAS("s5pv210_led");                    // 描述模块的别名信息
```

进入系统的 `sys/class/leds` 目录下可以看到驱动注册的三个 led 设备，分别为 led1、led2、led3，然后通过 `echo 0 > brightness` 命令来控制 led 的亮灭。

### 5.4 gpiolib 

gpiplib 用于统一管理系统中的 gpio 资源，避免驱动因为互相影响导致系统异常。

- struct s3c_gpio_chip

这个结构体是一个 GPIO 端口的抽象,这个结构体的一个变量就可以完全的描述一个 IO 端口。

端口和 IO 口是两个概念，S5PV210 有很多个IO口（160个左右），这些 IO 口首先被分成 N 个端口（port group），然后每个端口中又包含了 M 个 IO 口。譬如 GPA0 是一个端口，里面包含了 8 个 IO 口，我们一般记作：GPA0_0（或GPA0.0）、GPA0_1。

内核中为每个 GPIO 分配了一个编号，编号是一个数字（譬如一共有 160 个 IO 时编号就可以从 1 到160 连续分布），编号可以让程序很方便的去识别每一个 GPIO。

- s5pv210_gpio_4bit

这是一个结构体数组，数组中包含了很多个 struct s3c_gpio_chip 类型的变量。

- S5PV210_GPA0 宏

这个宏的返回值就是 GPA0 端口的某一个 IO 口的编号值，传参就是我们这个 IO 口在 GPA0 端口中的局部编号。

`samsung_gpiolib_add_4bit_chips` 这个函数才是具体进行 gpiolib 的注册的。这个函数接收的参数是我们当前文件中定义好的结构体数组 s5pv210_gpio_4bit（其实 2 个参数分别是数组名和数组元素个数），这个数组中其实就包含了当前系统中所有的 IO 端口的信息（这些信息包含：端口的名字、端口中所有 GPIO 的编号、端口操作寄存器组的虚拟地址基地。

## 5. Linux 设备驱动模型

### 5.1 基本概念

- 什么是设备驱动模型？
    - 类 class、总线 bus、设备 device、驱动 driver
    - kobject 和对象生命周期
    - sysfs
    - udev

- 为什么需要设备驱动模型？

早期内核（2.4之前）没有统一的设备驱动模型，但照样可以用。2.6 版本中正式引入设备驱动模型，目的是在设备越来越多，功耗要求等新特性要求的情况下让驱动体系更易用、更优秀。

设备驱动模型负责统一实现和维护一些特性，诸如：电源管理、热插拔、对象生命周期、用户空间和驱动空间的交互等基础设施。

设备驱动模型目的是简化驱动程序编写，但是客观上设备驱动模型本身设计和实现很复杂。

- 驱动开发的 2 个重点：

驱动源码本身编写、调试，重点在于对硬件的了解。

驱动什么时候被安装、驱动中的函数什么时候被调用。跟硬件无关，完全和设备驱动模型有关。

### 5.2 设备驱动模型的底层架构
- kobject
    - 定义在 linux/kobject.h 中
    - 各种对象最基本单元，提供一些公用型服务如：对象引用计数、维护对象链表、对象上锁、对用户空间的表示。
    - 设备驱动模型中的各种对象其内部都会包含一个kobject
    - 地位相当于面向对象体系架构中的总基类

- kobj_type
    - 很多书中简称为 ktype，每一个 kobject 都需要绑定一个 ktype 来提供相应功能
    - 关键点1：sysfs_ops，提供该对象在sysfs中的操作方法（show 和 store）
    - 关键点2：attribute，提供在sysfs中以文件形式存在的属性，其实就是应用接口

- kset
    - kset 的主要作用是做顶层 kobject 的容器类
    - kset 的主要目的是将各个 kobject（代表着各个对象）组织出目录层次架构
    - 可以认为 kset 就是为了在 sysfs 中弄出目录，从而让设备驱动模型中的多个对象能够有层次有逻辑性的组织在一起

### 5.3 总线式设备驱动组织方式
- 总线
    - 物理上的真实总线及其作用（英文 bus）
    - 驱动框架中的总线式设计
    - bus_type 结构体，关键是 match 函数和 uevent 函数

- 设备
    - struct device 是硬件设备在内核驱动框架中的抽象
    - device_register 用于向内核驱动框架注册一个设备
    - 通常 device 不会单独使用，而是被包含在一个具体设备结构体中，如 struct usb_device

- 驱动
    - struct device_driver 是驱动程序在内核驱动框架中的抽象
    - 关键元素1：name，驱动程序的名字，很重要，经常被用来作为驱动和设备的匹配依据
    - 关键元素2：probe，驱动程序的探测函数，用来检测一个设备是否可以被该驱动所管理

- 类
    - 相关结构体：struct class 和 struct class_device
    - udev 的使用离不开 class
    - class 的真正意义在于作为同属于一个 class 的多个设备的容器。也就是说，class 是一种人造概念，目的就是为了对各种设备进行分类管理。当然，class在分类的同时还对每个类贴上了一些标签，这也是设备驱动模型为我们写驱动提供的基础设施。

模型思想很重要，其实就是面向对象的思想，全是结构体套结构体，对基本功要求较高。

### 5.4 platform 平台总线工作原理
#### 5.4.1 何为平台总线
相对于 usb、pci、i2c 等物理总线来说，platform 总线是虚拟的、抽象出来的。在裸机中，CPU与外部通信的 2 种方式：地址总线式连接和专用接口式连接。平台总线对应地址总线式连接设备，也就是 SoC 内部集成的各种内部外设。

- 平台总线下管理的2员大将
    - platform 工作体系都定义在 `drivers/base/platform.c` 中
    - 两个结构体：platform_device 和 platform_driver
    - 两个接口函数：platform_device_register 和 platform_driver_register

```c
struct platform_device {
	const char	* name;			// 平台总线下设备的名字
	int		id;
	struct device	dev;		// 所有设备通用的属性部分
	u32		num_resources;		// 设备使用到的resource的个数
	struct resource	* resource;	// 设备使用到的资源数组的首地址

	const struct platform_device_id	*id_entry;	// 设备ID表
	
	/* arch specific additions */
	struct pdev_archdata	archdata;			// 自留地，用来提供扩展性的
};

struct platform_driver {
	int (*probe)(struct platform_device *);		// 驱动探测函数
	int (*remove)(struct platform_device *);	// 去掉一个设备
	void (*shutdown)(struct platform_device *);	// 关闭一个设备
	int (*suspend)(struct platform_device *, pm_message_t state);
	int (*resume)(struct platform_device *);
	struct device_driver driver;				// 所有设备共用的一些属性
	const struct platform_device_id *id_table;	// 设备 ID 表
};
```


#### 5.4.2 平台总线体系的工作流程

1. 第一步：系统启动时在 bus 系统中注册 platform
2. 第二步：内核移植的人负责提供 platform_device
3. 第三步：写驱动的人负责提供 platform_driver
4. 第四步：platform 的 match 函数发现 driver 和 device 匹配后，调用 driver 的 probe 函数来完成驱动的初始化和安装，然后设备就工作起来了

代码分析：platform 本身注册:

- 每种总线（不光是 platform，usb、i2c 那些也是）都会带一个 match 方法，match 方法用来对总线下的 device 和 driver 进行匹配。理论上每种总线的匹配算法是不同的，但是实际上一般都是看name 的。

- platform_match 函数就是平台总线的匹配方法。该函数的工作方法是：如果有 id_table 就说明驱动可能支持多个设备，所以这时候要去对比 id_table 中所有的 name，只要找到一个相同的就匹配上了不再找了，如果找完 id_table 都还没找到就说明每匹配上；如果没有 id_table 或者每匹配上，那就直接对比 device 和 driver 的 name，如果匹配上就匹配上了，如果还没匹配上那就匹配失败。


#### 5.4.3 platform设备和驱动的注册过程

- 以 leds-s3c24xx.c 为例来分析 platform 设备和驱动的注册过程
    - platform_driver_register
    - platform_device_register

- platdata 的用途

platdata 其实就是设备注册时提供的设备有关的一些数据（譬如设备对应的gpio、使用到的中断号、设备名称等）。

这些数据在设备和驱动 match 之后，会由设备方转给驱动方。驱动拿到这些数据后，通过这些数据得知设备的具体信息，然后来操作设备。这样做的好处是：驱动源码中不携带数据，只负责算法（对硬件的操作方法）。现代驱动设计理念就是算法和数据分离，这样最大程度保持驱动的独立性和适应性。

将对设备的操作方法（也就是驱动）和设备本身的数据分离，可以写出通用性更强的驱动程序，不会和具体的设备耦合，也就有了更好的可移植性。

## 6. misc 类设备
### 6.1 misc类设备介绍
中文名：杂项设备\杂散设备，`sys/class/misc`，是一种典型的字符设备，有一套驱动框架，内核实现一部分（misc.c），驱动实现一部分（x210-buzzer.c）。

misc 是对原始的字符设备注册接口的一个类层次的封装，很多典型字符设备都可以归类到misc类中，使用misc驱动框架来管理。

- misc类设备驱动架构

内核开发者实现部分，关键点有2个：一个是类的创建，另一个是开放给驱动开发者的接口，具体设备驱动工程师实现部分。

### 6.2 misc 驱动框架源码分析

- misc 源码框架基础

misc 源码框架本身也是一个模块，内核启动时自动加载。源码框架的主要工作：注册misc类，使用老接口注册字符设备驱动（主设备号10），开放 device 注册的接口 misc_register 给驱动工程师。

- misc 类设备的注册

驱动工程师需要借助 misc 来加载自己的驱动时，只需要调用 misc_register 接口注册自己的设备即可，其余均不用管。

- misc_list 链表的作用

内核定义了一个 misc_list 链表用来记录所有内核中注册了的杂散类设备。当我们向内核注册一个misc类设备时，内核就会向misc_list链表中insert一个节点。

```
#define LIST_HEAD_INIT(name) { &(name), &(name) }
#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)
```

原式子：static LIST_HEAD(misc_list);
展开后：static struct list_head misc_list = { &(misc_list), &(misc_list) }

- 内核互斥锁

何为互斥锁，定义：DEFINE_MUTEX。上锁 mutex_lock 和解锁 mutex_unlock。内核防止竞争状态的手段：原子访问、自旋锁、互斥锁、信号量。互斥锁和信号量很相似（其实就是计数值为 1 的信号量），互斥锁的出现比信号量晚，实现上比信号量优秀，尽量使用互斥锁。

## 7. framebuffer 设备

### 7.1 什么是 framebuffer
- framebuffer 帧缓冲（简称fb）是 linux 内核中虚拟出的一个设备
- framebuffer 向应用层提供一个统一标准接口的显示设备
- 从驱动来看，fb是一个典型的字符设备，而且创建了一个类 `/sys/class/graphics`

### 7.2 framebuffer 的使用
1. 设备文件 `/dev/fb0`
2. 获取设备信息 `#include <linux/fb.h>`
3. mmap 做映射
4. 填充 framebuffer

### 7.3 FB 驱动框架总览
#### 7.3.1 驱动框架部分
- drivers/video/fbmem.c
    创建 graphics 类、注册 FB 的字符设备驱动、提供 register_framebuffer 接口给具体 framebuffer 驱动编写着来注册 fb 设备的。本文件相对于 fb 来说，地位和作用和 misc.c 文件相对于杂散类设备来说一样的，结构和分析方法也是类似的。
- drivers/video/fbsys.c
    这个文件是处理fb在/sys目录下的一些属性文件的。
- drivers/video/modedb.c
    这个文件是管理显示模式（譬如VGA、720P等就是显示模式）的
- drivers/video/fb_notify.c

#### 7.3.2 驱动部分
- drivers/video/samsung/s3cfb.c，驱动主体
- drivers/video/samsung/s3cfb_fimd6x.c，里面有很多LCD硬件操作的函数
- arch/arm/mach-s5pv210/mach-x210.c，负责提供platform_device的
- arch/arm/plat-s5p/devs.c，为platform_device提供一些硬件描述信息的

### 7.3 驱动框架分析

- fbmem_init 函数
    - `#ifdef MODULE`
    - fb_proc_fops 和 fb 在 proc 文件系统中的表现
    - register_chrdev 注册 fb 设备
    - class_create 创建 graphics 类
    - fbmem_exit 的对应

- fb_fops
    - read/write/mmap/ioctl
    - registered_fb 和 num_registered_fb
    - struct fb_info

- register_framebuffer，fb 驱动框架开放给驱动编写着的注册接口
    - fb_check_foreignness
    - remove_conflicting_framebuffers
    - device_create
    - fb_init_device

- fb在sysfs中的接口
device_attrs、dev_set_drvdata 和 dev_get_drvdata。

- fb 的 mode，工作模式
`b_var_to_videomode`，`fb_add_videomode`。

- 注册登记该fb设备
`registered_fb[i] = fb_info`，结合fb_read等函数中对fb_info的使用，关键点：数据如何封装、数据由谁准备由谁消费、数据如何传递。

- fb_notifier_call_chain，通知已经注册了的 FB 设备有事件发生

### 7.4 framebuffer 驱动分析

- s3cfb.c
    注意目录结构的组织，s3cfb_driver。
    
- s3c_device_fb
    1. mach-x210.c 中，被使用
    2. devs.c 中
    3. resource 的定义和作用

- probe 函数分析
struct s3c_platform_fb，这个结构体是 fb 的 platform_data 结构体，这个结构体变量就是platform 设备的私有数据，这个数据在 platform_device.device.platform_data 中存储。在 mach文件中去准备并填充这些数据，在 probe 函数中通过传参的 platform_device 指针取出来。

- struct s3cfb_global		
这个结构体主要作用是在驱动部分的2个文件（s3cfb.c和s3cfb_fimd6x.c）的函数中做数据传递用的。

- struct resource
- regulator

- platform_data的传递过程
  1. to_fb_plat
  2. s3cfb_set_platdata
  3. smdkc110_machine_init

- resource的处理
  - platform_device 中提供 resource 结构体数组
  - probe 中 platform_get_resource 取出 resource 并且按 FLAG 分头处理

- 一些硬件操作
  - s3cfb_set_vsync_interrupt
  - s3cfb_set_global_interrupt

- s3cfb_init_global

- 向框架注册该fb设备
  - s3cfb_alloc_framebuffer
  - s3cfb_register_framebuffer

- 一些硬件操作
    - s3cfb_set_clock
    - s3cfb_set_window
    - s3cfb_display_on

- 驱动中处理中断
    - platform_get_irq
    - request_irq

- logo显示
```
  s3cfb_probe
  	fb_prepare_logo
  		fb_find_logo			真正查找logo文件
    fb_show_logo			
  		fb_show_logo_line		真正显示logo
  			fb_do_show_logo
          info->fbops->fb_imageblit		实际操作硬件 fb 进行显示工作的函数
```

## 8. input 子系统

### 8.1 什么是 input 子系统

linux 系统中有各种各样的输入设备，为了统一不同的输入设备，需要使用一个统一的模型来描述。

- struct input_event
- input 子系统分4个部分：应用层 + input event + input core + 硬件驱动

### 8.2 input设备应用层编程实践
- 确定设备文件名
    - 应用层操作驱动有2条路：/dev目录下的设备文件，/sys目录下的属性文件
    - input 子系统用的 /dev 目录下的设备文件，具体一般都是在 /dev/input/eventn
    - 用 cat 命令来确认某个设备文件名对应哪个具体设备。我在自己的 ubuntu 中实测的键盘是event1，而鼠标是 event3.

- 标准接口打开并读取文件
- 解析struct input_event

### 8.3 input 子系统架构
- input子系统分为三层
    - 最上层：输入事件驱动层，evdev.c 和 mousedev.c 和 joydev.c 属于这一层
    - 中间层：输入核心层，input.c 属于这一层
    - 最下层：输入设备驱动层，drivers/input/xxx 文件夹下

- input类设备驱动开发方法
    - 输入事件驱动层和输入核心层不需要动，只需要编写设备驱动层
    - 设备驱动层编写的接口和调用模式已定义好，驱动工程师的核心工作量是对具体输入设备硬件的操作和性能调优

- 核心模块注册 input_init
    - class_register
    - input_proc_init
    - register_chrdev

- 设备驱动层的接口函数
    - input_allocate_device
    - input_set_capability
    - input_register_device

- handler 和 device的匹配
    - input_attach_handler
    - input_match_device		匹配 device 和 handler
    - handler->connect(handler, dev, id)		连接 device 和 handler

- 事件驱动层的接口函数
    - input_register_handler
    - input_register_handle

- 输入事件驱动层源码
    - input_handler
    - evdev_connect
    - evdev_event

## 9. IIC 总线

### 9.1 内核中的竞争状态和互斥
- 一些概念
    - 竞争状态（简称竟态）
    - 临界段、互斥锁、死锁
    - 同步（多CPU、多任务、中断）
- 解决竟态的方法
    - 原子操作 automic_t
    - 信号量、互斥锁
    - 自旋锁
- 自旋锁和信号量的使用要点
    - 自旋锁不能递归
    - 自旋锁可以用在中断上下文（信号量不可以，因为可能睡眠），但是在中断上下文中获取自旋锁之前要先禁用本地中断
    - 自旋锁的核心要求是：拥有自旋锁的代码必须不能睡眠，要一直持有CPU直到释放自旋锁
    - 信号量和读写信号量适合于保持时间较长的情况，它们会导致调用者睡眠，因此只能在进程上下文使用，而自旋锁适合于保持时间非常短的情况，它可以在任何上下文使用。如果被保护的共享资源只在进程上下文访问，使用信号量保护该共享资源非常合适，如果对共享资源的访问时间非常短，自旋锁也可以。但是如果被保护的共享资源需要在中断上下文访问（包括底半部即中断处理句柄和顶半部即软中断），就必须使用自旋锁。自旋锁保持期间是抢占失效的，而信号量和读写信号量保持期间是可以被抢占的。自旋锁只有在内核可抢占或SMP（多处理器）的情况下才真正需要，在单CPU且不可抢占的内核下，自旋锁的所有操作都是空操作。

### 9.2 中断的上下半部

- 中断处理的注意点
    - 中断上下文，不能和用户空间数据交互
    - 不能交出CPU（不能休眠、不能schedule）
    - ISR运行时间尽可能短，越长则系统响应特性越差
- 中断下半部2种解决方案
    - 为什么要分上半部（top half，又叫顶半部）和下半部（bottom half，又叫底半部）
    - 下半部处理策略1：tasklet（小任务）
    - 下半部处理策略2：workqueue（工作队列）

- workqueue实战演示
    - workqueue的突出特点是下半部会交给worker thead，因此下半部处于进程上下文，可以被调度，因此可以睡眠。
    - include/linux/workqueue.h

- 中断上下半部处理原则
    - 必须立即进行紧急处理的极少量任务放入在中断的顶半部中，此时屏蔽了与自己同类型的中断，由于任务量少，所以可以迅速不受打扰地处理完紧急任务。
    - 需要较少时间的中等数量的急迫任务放在 tasklet中。此时不会屏蔽任何中断（包括与自己的顶半部同类型的中断），所以不影响顶半部对紧急事务的处理；同时又不会进行用户进程调度，从而保证了自己急迫任务得以迅速完成。
    - 需要较多时间且并不急迫（允许被操作系统剥夺运行权）的大量任务放在 workqueue 中。此时操作系统会尽量快速处理完这个任务，但如果任务量太大，期间操作系统也会有机会调度别的用户进程运行，从而保证不会因为这个任务需要运行时间将其它用户进程无法进行。
    - 可能引起睡眠的任务放在 workqueue 中。因为在 workqueue 中睡眠是安全的。在需要获得大量的内存时、在需要获取信号量时，在需要执行阻塞式的 I/O 操作时，用 workqueue 很合适。

## 10. 块设备驱动

- 块设备和字符设备的差异
    - 块和字符是两种不同的访问设备的策略
    - 同一个设备可以同时支持块和字符两种访问策略
    - 设备本身的物理特性决定了哪一种访问策略更适合
    - 块设备本身驱动层支持缓冲区，而字符设备驱动层没有缓冲
    - 块设备驱动最适合存储设备

- 块设备驱动的特点
    - 字符设备只能顺序访问（如串口发送数据顺序），而块设备可以随机访问（不连续块访问）
    - 传统的机械式块设备（如硬盘、DVD）虽然可以随机访问，但是连续访问效率更高，因此块设备驱动中有排序逻辑将用户的随机访问重新调整成尽量连续访问以提升效率
    - Nand、SD卡等随机访问效率等同于顺序访问

- 块设备相关的几个单位
    - 扇区（Sector），概念来自于早期磁盘，在硬盘、DVD中还有用，在Nand/SD中已经没意义了，扇区是块设备本身的特性，大小一般为512的整数倍，因为历史原因很多时候都向前兼容定义为512.
    - 块（block），概念来自于文件系统，是内核对文件系统数据处理的基本单位，大小为若干个扇区，常见有512B、1KB、4KB等
    - 段（Section），概念来自于内核，是内核的内存管理中一个页或者部分页，由若干个连续为块组成。
    - 页（Page），概念来自于内核，是内核内存映射管理的基本单位。linux内核的页式内存映射名称来源于此。

总结：块设备驱动对下以Sector为单位管理块设备，对上以Block为单位和文件系统交互。
注意：块设备驱动和字符设备驱动不同，应用层对块设备驱动的访问一般不是直接操作设备文件（/dev/block/xxx，或者/dev/sdax），而是通过文件系统来简洁操作。

## 11.网络设备驱动

## 12. 结语

很多时候，我们总是会低估完成一件事情所需要消耗的时间和精力，最终的结果往往是事情的完成被延期或者遥遥无期。 `Embedded Linux` 学习计划，本应在一两年前就完成，但是因为换了新的工作内容，暂时用不到的知识学习就被暂时搁置了，殊不知计算机基础知识才是嵌入式入门从业者急需补充的知识。

捡起之前的计划需要再次熟悉相关的环境，重新理清思路，这样做必然要比一次性完成一项计划要耗费更多的时间。但是不管怎么样，多少件没有完成的也比不上一件圆满完成的计划。

在积累了一些工作经验之后，我逐渐认识到那句话的背后的意义，**优秀的人首先追求完成，然后再追求完美，完成比完美更重要**。做一件事如果在计划内的时间里做不完，那么可能这辈子都再也做不完了，最终收获的结果会大打折扣。想要获得持续进步并不难，只需要少做一些事情，然后把必要的事情做得卓有成效就好了。

