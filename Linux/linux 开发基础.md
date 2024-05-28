# Linux 开发基础

## 内核配置

### 设置 kconfig

- make xxx_linux_virt_defconfig 选择一个预设的
- make menuconfig 进行配置
- make savedefconfig
- cp defconfig 存储到指定目录

### 修改 kernel log level

内核通过`printk()` 输出的信息具有日志级别，日志级别是通过在`printk()` 输出的字符串前加一个带尖括号的整数来控制的，如`printk("<6>Hello, world!\n");`。内核中共提供了八种不同的日志级别，在 `linux/kernel.h` 中有相应的宏对应。

```
#define KERN_EMERG  "<0>"   /* systemis unusable */
#define KERN_ALERT  "<1>"   /* actionmust be taken immediately */
#define KERN_CRIT    "<2>"   /*critical conditions */
#define KERN_ERR     "<3>"   /* errorconditions */
#define KERN_WARNING "<4>"   /* warning conditions */
#define KERN_NOTICE  "<5>"   /* normalbut significant */
#define KERN_INFO    "<6>"   /*informational */
#define KERN_DEBUG   "<7>"   /*debug-level messages */
```

未指定日志级别的`printk()` 采用的默认级别是`DEFAULT_MESSAGE_LOGLEVEL`，这个宏在`kernel/printk.c` 中被定义为整数4，即对应`KERN_WARNING`。 **在宏定义中，数值越小，优先级越高，其紧急和严重程度就越高。**

1. 控制台日志级别：优先级高于该值的消息将被打印至控制台。 
2. 缺省的消息日志级别：将用该值来打印没有优先级的消息。
3. 最低的控制台日志级别：控制台日志级别可能被设置的最小值。
4. 缺省的控制台：控制台日志级别的缺省值。

这四个值是在`kernel/printk.c`中被定义的：

```
int console_printk[4] = {

                DEFAULT_CONSOLE_LOGLEVEL,       /* console_loglevel */

                DEFAULT_MESSAGE_LOGLEVEL,       /* default_message_loglevel */

                MINIMUM_CONSOLE_LOGLEVEL,     /* minimum_console_loglevel */

                DEFAULT_CONSOLE_LOGLEVEL,       /* default_console_loglevel */

};
```

修改 log 等级命令：

```
su;echo 8 8 8 8 > /proc/sys/kernel/printk
```

## 代码格式化 

可以先使用 Astyle 进行代码格式化，然后再使用 `checkpatch` 脚本检查代码是否符合规范要求。

### Astyle 格式化

```C
astyle --style=linux --indent=force-tab=8 --align-pointer=name --max-code-length=90 --break-after-logical -p -H -U drivers/folder/*.c  drivers/folder/*.h
```

### 文件夹格式化检查

需要基于 python3 安装 git python:

```
python3 -m pip install ply
python3 -m pip install gitpython
```

然后可以使用格式化检查：

```C
scripts/checkpatch.pl -f drivers/folder/*.c
```

### 检查后将修改存为 patch

```C
scripts/checkpatch.pl -f drivers/folder/*.c --fix
```

### 检查后在文件上修改

```C
scripts/checkpatch.pl -f drivers/folder/*.c --fix-inplace
```

### 将修改整理成 patch 后检查

```C
git format-patch -1
scripts/checkpatch.pl 0001-kernel-xxx-init.patch
```

或者直接使用如下命令，生成 patch 后直接将 patch 名称传递给 checkpatch 检查：

```
git format-patch --stdout -1 | scripts/checkpatch.pl --no-signoff --no-changeid -
```

## 编码细节

### 代码使能

使用 IS_ENABLED 宏来包裹代码，可以避免编译系统不识别 `kconfig` 选项为 m 的宏，比较稳定。

```c
#if IS_ENABLED(CONFIG_XHPC_NPC_SUPPORT_SAFETY_MONITOR)

// yourcode

#endif
```

### 错误指针处理

```c
#define MAX_ERRNO	4095

#define IS_ERR_VALUE(x) unlikely((x) >= (unsigned long)-MAX_ERRNO)

static inline void * __must_check ERR_PTR(long error)
{
	return (void *) error;
}

static inline long __must_check PTR_ERR(const void *ptr)
{
	return (long) ptr;
}

static inline long __must_check IS_ERR(const void *ptr)
{
	return IS_ERR_VALUE((unsigned long)ptr);
}

static inline long __must_check IS_ERR_OR_NULL(const void *ptr)
{
	return !ptr || IS_ERR_VALUE((unsigned long)ptr);
}
```

Linux内核提供了一组宏来帮助处理函数返回的错误指针，这些宏包括PTR_ERR、ERR_PTR和IS_ERR。它们在处理无法返回标准错误码（例如，-EINVAL）的函数时特别有用，因为这些函数需要返回一个指针。以下是对这三个宏的解释和使用场景的说明：

#### TR_ERR

**解释**：当函数返回错误指针时，PTR_ERR宏用于从错误指针中提取并返回错误码。
**使用场景**：如果你调用了一个返回指针的函数，并且使用IS_ERR检查指出该指针是一个错误指针，那么可以使用PTR_ERR来获取具体的错误码。例如，当ioremap失败时，使用PTR_ERR来获取错误码：

```c
  void *ptr = ioremap(paddr, size);
  if (IS_ERR(ptr)) {
      int err = PTR_ERR(ptr);
      pr_err("ioremap failed: %d\n", err);
  }
```

#### ERR_PTR

**解释**：ERR_PTR宏用于将错误码转换为错误指针。
**使用场景**：当你的函数需要返回一个指针，但在某些错误条件下需要返回错误码时，可以使用ERR_PTR来返回一个包含错误码的错误指针。例如，一个分配内存的函数，在无法分配时返回错误码：

```
  void *my_alloc(size_t size) {
      void *ptr = kmalloc(size, GFP_KERNEL);
      if (!ptr)
          return ERR_PTR(-ENOMEM); // 内存不足
      return ptr;
  }
```

#### IS_ERR

**解释**：IS_ERR宏用于检查一个指针是否是错误指针。
**使用场景**：当你调用一个返回指针的函数，并且这个函数可能返回错误指针时，使用IS_ERR来判断返回值是否表示错误。如果是，你可以进一步使用PTR_ERR来获取错误码。例如，检查 my_alloc 函数的返回值：

```
  void *ptr = my_alloc(size);
  if (IS_ERR(ptr)) {
      int err = PTR_ERR(ptr);
      pr_err("Allocation failed: %d\n", err);
  }
```

这三个宏共同提供了一种在需要返回指针的函数中处理错误的机制，使得函数能够在出错时返回具体的错误码，而不仅仅是NULL或任意非法指针。

#### IS_ERR_OR_NULL

用于判断指针是空指针或是错误指针，如果既要判断返回值是否为空，又要判断错误码，那么故障处理就会显得复杂一些。

```c
void *vaddr = ioremap(paddr, size);
if (!vaddr) {
    pr_err("ioremap returned NULL\n");
} else if (IS_ERR(vaddr)) {
    pr_err("ioremap failed with error: %ld\n", PTR_ERR(vaddr));
}
```

### 指针类型打印

在 `linux` 系统中直接使用 %p 打印指针会打印被 hash 的指针，无法打印出指针的真实值，原因是为了避免泄露系统的信息。

可参考文档：https://www.kernel.org/doc/Documentation/core-api/printk-formats.rst

```yaml
Pointer types
=============

A raw pointer value may be printed with %p which will hash the address
before printing. The kernel also supports extended specifiers for printing
pointers of different types.

Some of the extended specifiers print the data on the given address instead
of printing the address itself. In this case, the following error messages
might be printed instead of the unreachable information::

	(null)	 data on plain NULL address
	(efault) data on invalid address
	(einval) invalid data on a valid address

Plain Pointers
--------------

::

	%p	abcdef12 or 00000000abcdef12

Pointers printed without a specifier extension (i.e unadorned %p) are
hashed to prevent leaking information about the kernel memory layout. This
has the added benefit of providing a unique identifier. On 64-bit machines
the first 32 bits are zeroed. The kernel will print ``(ptrval)`` until it
gathers enough entropy.

When possible, use specialised modifiers such as %pS or %pB (described below)
to avoid the need of providing an unhashed address that has to be interpreted
post-hoc. If not possible, and the aim of printing the address is to provide
more information for debugging, use %p and boot the kernel with the
``no_hash_pointers`` parameter during debugging, which will print all %p
addresses unmodified. If you *really* always want the unmodified address, see
%px below.

If (and only if) you are printing addresses as a content of a virtual file in
e.g. procfs or sysfs (using e.g. seq_printf(), not printk()) read by a
userspace process, use the %pK modifier described below instead of %p or %px.
```

## 内存错误检查

### 内核模块

#### KASAN 简介

KASAN (Kernel Address Sanitizer) 是 Linux 内核中的一个动态内存错误检测工具，主要用于查找使用后释放（use-after-free）和越界（out-of-bounds）错误。这是一种非常有效的方法，可以帮助开发者在开发过程中发现潜在的内存错误，从而提高代码的稳定性和安全性。

#### 如何使用 KASAN

1. **配置内核**：要启用 KASAN，需要在内核配置时启用 CONFIG_KASAN 选项。此外，还可以选择 CONFIG_KASAN_OUTLINE 或 CONFIG_KASAN_INLINE 两种编译器插桩类型。CONFIG_KASAN_OUTLINE 生成的二进制文件较小，而 CONFIG_KASAN_INLINE 提供更快的性能（需要 GCC 5.0 或更高版本）。

  CONFIG_KASAN=y

  CONFIG_KASAN_INLINE=y # 或 CONFIG_KASAN_OUTLINE=y

2. **选择内存分配器**：KASAN 可以与 SLUB 或 SLAB 内存分配器一起工作。为了更好的错误检测和报告，建议启用 CONFIG_STACKTRACE。
3. **禁用特定文件的插桩**：如果需要，可以在特定的内核 Makefile 中禁用某些文件或目录的插桩。

  KASAN_SANITIZE_main.o := n

#### KASAN 可以达到的效果

使用 KASAN 可以帮助开发者：

**检测内存访问错误**：包括使用后释放和数组越界等常见的内存错误。

**提供详细的错误报告**：当检测到内存错误时，KASAN 会提供详细的错误报告，包括错误类型、发生错误的位置、相关的内存状态等信息。这些信息对于开发者定位和修复问题非常有帮助。

#### 错误报告示例

一个典型的越界访问错误报告可能如下所示：

> ==================================================================
>
> BUG: AddressSanitizer: out of bounds access in kmalloc_oob_right+0x65/0x75 [test_kasan] at addr ffff8800693bc5d3
>
> Write of size 1 by task modprobe/1689
>
> =============================================================================
>
> BUG kmalloc-128 (Not tainted): kasan error
>
> \-----------------------------------------------------------------------------

报告会详细描述哪种类型的访问（如写入）导致了错误，以及具体的内存地址和相关的调用栈信息。

#### 实现细节

KASAN 通过在编译时插入检查代码来实现内存访问的监控。它使用了一部分内核内存作为“影子内存”（shadow memory），用来记录每个内存字节的访问安全状态。每次内存访问都会通过影子内存来检查是否安全。

### 用户程序

