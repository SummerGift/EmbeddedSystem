# Linux 开发基础

## 内核配置

- make xxx_linux_virt_defconfig 选择一个预设的
- make menuconfig 进行配置
- make savedefconfig
- cp defconfig 存储到指定目录

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

## 重定位内核打印

```
su; echo 7777 > /proc/sys/kernel/printk
```

## 内存错误检查

### 内核排查技巧

### 用户排查技巧

