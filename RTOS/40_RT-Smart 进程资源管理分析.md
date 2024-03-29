# 进程资源管理分析

在 smart 操作系统中，进程是资源分配的实体，进程需要管理的资源如下所示：

- 用户进程 ID
- 用户线程 ID
- 内核对象的管理（包括 IPC 以及定时器等，进程退出时需要释放）
- 内存管理
- 用户线程本地存储（TLS）

下面分别就这些资源类型进行详细说明。

## 用户进程 ID 

系统中可能存在多核用户进程，smart 操作系统使用用户进程 ID（PID）作为进程的唯一标识符。当创建进程时，需要申请分配一个 PID 与之对应。当用户进程终止时，该 PID 会被回收。

存储 PID 的数据结构是一个数组和一个单链表：

```c
static struct lwp_avl_struct lwp_pid_ary[RT_LWP_MAX_NR];

/* the head node of single linked list to store free pid */

static struct lwp_avl_struct *lwp_pid_free_head = RT_NULL;
```

### 申请 PID

获取 lwp 的函数为 `lwp_pid_get`，其功能如下：

1. 尝试在空闲链表中获取可用 PID
2. 如果空闲链表中没有可用 PID 则尝试从 `lwp_pid_ary` 数组中获得 PID
3. 从数组中获取 PID 的情况下，将尝试获取比当前 PID 数值更大的 PID 号
4. 如果无法获取数值更大的 PID，则尝试从 1 开始遍历数组，获取一个较小的 PID 号码
5. 成功获取 PID 后，将其插入存储已用 pid 的二叉树 `lwp_pid_root`
6. 返回 PID

```c
/**
 * This function will allocate and return a unique lwp ID.
 *
 * @return a unique lwp ID on success, 0 on failure
 */
static pid_t lwp_pid_get(void)
{
    rt_base_t level = 0;
    struct lwp_avl_struct *p = RT_NULL;
    pid_t pid = 0;
    int found = 0;

    level = rt_hw_interrupt_disable();

    /* backup the pid free head */
    p = lwp_pid_free_head;

    /* check if there is a node available in the free list */
    if (p)
    {
        /* have a lwp node in free list, then remove it */
        lwp_pid_free_head = (struct lwp_avl_struct *)p->avl_right;
    }
    else
    {
        /* no free lwp node in free list, get a free node from pid array */
        if (lwp_pid_ary_alloced < RT_LWP_MAX_NR)
        {
            p = lwp_pid_ary + lwp_pid_ary_alloced;
            lwp_pid_ary_alloced++;
        }
    }

    if (p)
    {
        RT_ASSERT(p->data == RT_NULL);

        /* try to get a pid bigger than the current pid */
        for (pid = current_pid + 1; pid < PID_MAX; pid++)
        {
            /* check if the pid is no used */
            if (!lwp_avl_find(pid, lwp_pid_root))
            {
                found = 1;
                break;
            }
        }

        /* try to get a pid number from 1 to the current pid */
        if (!found)
        {
            for (pid = 1; pid <= current_pid; pid++)
            {
                if (!lwp_avl_find(pid, lwp_pid_root))
                {
                    found = 1;
                    break;
                }
            }
        }

        if(found)
        {
            /* find a free pid node */
            p->avl_key = pid;
            lwp_avl_insert(p, &lwp_pid_root);
            current_pid = pid;
        }
        else
        {
            /* can't find a free node */
            pid = 0;
        }
    }
    rt_hw_interrupt_enable(level);

    return pid;
}
```

### 释放 PID

释放 PID 的函数 `lwp_pid_put` 就比较简单了，其功能如下：

1. 在 `lwp_pid_root` 二叉树中查找到相应的 PID 节点
2. 将该节点从 `lwp_pid_root` 二叉树中移除
3. 将该节点插入到空闲 PID 链表中，便于后续快速获得 PID

代码实现如下所示：

```c
/**
 * This function will release the unique lwp ID.
 *
 * @param tid the unique lwp ID to be released
 */
static void lwp_pid_put(pid_t pid)
{
    rt_base_t level = 0;
    struct lwp_avl_struct *p = RT_NULL;

    level = rt_hw_interrupt_disable();
    p  = lwp_avl_find(pid, lwp_pid_root);
    if (p)
    {
        p->data = RT_NULL;
        lwp_avl_remove(p, &lwp_pid_root);
        p->avl_right = lwp_pid_free_head;
        lwp_pid_free_head = p;
    }
    rt_hw_interrupt_enable(level);
}
```

## 用户线程 ID

每个用户进程中可能包含 1 到多个用户线程，每个用户线程都有其唯一的标识编号，即用户线程 ID（TID）。当创建用户线程时，需分配一个用户线程 ID 与之对应。当终止用户线程时，需要释放并回收该线程 ID。

用户线程 ID 的申请和释放过程与用户进程 ID 的操作方法是一致的，用了同样的数据结构以及同样的操作算法，只不过他们分别管理的资源是完全独立的，这里就不再详细说明。

## 内核对象管理

用户进程可以通过系统调用申请使用内核对象，可能申请的对象包括各种 IPC、定时器等等。

### IPC 与定时器

在 lwp 结构中使用 `struct lwp_avl_struct *object_root;`  二叉树来存储进程所使用的过的内核对象。在进程使用系统调用创建内核对象时，会先将该内核对象插入到进程的对象管理树中。进程主动退出或者意外退出时，需要查找进程使用的内核对象，并释放和回收这些资源。

### 线程内核对象

thread 内核对象使用 `rt_list_t t_grp;` 数据结构管理，在初始化第一个用户线程时和创建新的用户线程时，会将该 thread 添加到指定进程的结构体 `rt_lwp->t_grp` 成员上。

等到进程退出时，就可以遍历这个链表，依次将相应的线程退出，并释放相应资源。

## 内存管理

每个进程都运行在自己独立的地址空间，在初始化进程的过程中，会调用 `lwp_user_space_init` 函数为该进程初始化页表，该函数会执行如下操作：

1. 申请一个 4k 的页作为页表
2. 清空该表中的内容，并刷新缓存
3. 初始化 lwp 中的 mmu 信息表，写入用户态相关地址空间信息

在 ARM64 位架构下，用户态虚拟地址空间分布如下：

| 宏定义             | 数值                      | 说明                    |
| ------------------ | ------------------------- | ----------------------- |
| USER_VADDR_START   | 0x00200000UL              | 用户空间起始地址为 2M   |
| USER_VADDR_TOP     | 0x0001000000000000UL      | 用户空间结束地址为 256T |
| USER_STACK_VSTART  | 0x0000FFFF70000000UL      | 用户空间栈起始地址      |
| USER_STACK_VEND    | 0x0000FFFF80000000UL      | 用户空间栈结束地址      |
| USER_HEAP_VADDR    | 0x0000FFFF80000000UL      | 用户空间堆起始地址      |
| USER_HEAP_VEND     | 0x0000FFFFB0000000UL      | 用户空间堆结束地址      |
| ARCH_SECTION_SHIFT | 21                        | 节大小移位值            |
| ARCH_SECTION_SIZE  | (1 << ARCH_SECTION_SHIFT) | 节大小为 2M             |
| ARCH_PAGE_SHIFT    | 12                        | 页大小移位值            |
| ARCH_PAGE_SIZE     | (1 << ARCH_PAGE_SHIFT)    | 页大小为 4K             |

后续在用户态应用程序加载的过程中，首先会遍历 elf 文件中所有的节头表，计算出程序加载需要的数据段和代码段的大小，然后调用 `lwp_map_user ` 函数来创建该进程的虚拟地址物理地址映射。

这里先不深入用户态进程空间的创建以及销毁过程的实现，只需要知道创建进程的过程中需要做哪些初始化操作即可。

## 线程本地存储

进程中的全局变量与函数内定义的静态（static）变量，是各个线程都可以访问的共享变量。在一个线程修改的内存内容，对所有线程都生效。这是一个优点也是一个缺点。说它是优点，线程的数据交换变得非常快捷。说它是缺点，一个线程死掉了，其它线程也性命不保。多个线程访问共享数据，需要昂贵的同步开销，也容易造成同步相关的 BUG。

如果需要在一个线程内部的各个函数调用都能访问、但其它线程不能访问的变量（被称为 static memory local to a thread 线程局部静态变量），就需要新的机制来实现，这就是TLS。

线程局部存储在不同的平台有不同的实现，可移植性不太好，但是要实现线程局部存储并不难，最简单的办法就是建立一个全局表，通过当前线程 ID 去查询相应的数据，因为各个线程的 ID 不同，查到的数据自然也不同了。

### TLS 功能

它主要是为了避免多个线程同时访存同一全局变量或者静态变量时所导致的冲突，尤其是多个线程同时需要修改这一变量时。为了解决这个问题，我们可以通过 TLS 机制，为每一个使用该全局变量的线程都提供一个变量值的副本，每一个线程均可以独立地改变自己的副本，而不会和其它线程的副本冲突。从线程的角度看，就好像每一个线程都完全拥有该变量。而从全局变量的角度上来看，就好像一个全局变量被克隆成了多份副本，而每一份副本都可以被一个线程独立地改变。

### TLS 实现

线程本地存储特性需要内核、编译器、线程库协同实现。内核主要负责 TLS 基地址寄存器的保存恢复、将 TLS 基地址写入TLS基地址寄存器。

线程结构体的成员 `rt_thread_t->thread_idr` 用于存储当前线程局部存储的基地址，在应用程序运行时该值存放在 `tpidr_el0` 寄存器中。

#### 存储 TLS 基地址

内核中提供了存储 TLS 基地址到线程结构体中的接口：

```c
/**
 * This function will save the software thread ID.
 *
 * @param thread the thread object handle
 */
void lwp_user_setting_save(rt_thread_t thread)
{
    if (thread)
    {
        thread->thread_idr = rt_cpu_get_thread_idr();
    }
}
```

```assembly
rt_cpu_get_thread_idr:
    mrs x0, tpidr_el0
    ret
```

#### 恢复 TLS 基地址

内核中提供了恢复 TLS 基地址到 `tpidr_el0` 寄存器的接口：

```c
/**
 * This function will restore the software thread ID.
 *
 * @param thread the thread object handle
 */
void lwp_user_setting_restore(rt_thread_t thread)
{
    if (thread)
    {
        rt_cpu_set_thread_idr(thread->thread_idr);
    }
}
```

```assembly
rt_cpu_set_thread_idr:
    msr tpidr_el0, x0
    ret
```
