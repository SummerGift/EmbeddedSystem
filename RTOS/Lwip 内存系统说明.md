# lwip 中内存系统错误说明

在 lwip 系统中的内存申请可以配置为从多个位置分配内存，例如从 lwip 自带的内存池中和内存堆中，

在 rt-thread 中接管了 lwip 的堆内存分配，因此如果调用 lwip 的接口尝试从堆中分配内存，那么将会从 rt-thread 的系统堆管理器中进行分配。但是 rt-thread 的堆管理器和 lwip 自带的堆管理器有所不同，当使用 lwip 的 mem.c  中的内存管理算法进行内存分配时，`LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT` 宏会影响分配器的行为。

```c
/**
 * Set this to 1 if you want to free PBUF_RAM pbufs (or call mem_free()) from
 * interrupt context (or another context that doesn't allow waiting for a
 * semaphore).
 * If set to 1, mem_malloc will be protected by a semaphore and SYS_ARCH_PROTECT,
 * while mem_free will only use SYS_ARCH_PROTECT. mem_malloc SYS_ARCH_UNPROTECTs
 * with each loop so that mem_free can run.
 *
 * ATTENTION: As you can see from the above description, this leads to dis-/
 * enabling interrupts often, which can be slow! Also, on low memory, mem_malloc
 * can need longer.
 *
 * If you don't want that, at least for NO_SYS=0, you can still use the following
 * functions to enqueue a deallocation call which then runs in the tcpip_thread
 * context:
 * - pbuf_free_callback(p);
 * - mem_free_callback(m);
 */

#define LWIP_ALLOW_MEM_FREE_FROM_OTHER_CONTEXT 0
```

当这个宏设置为 1 时，将会允许 lwip 系统在中断上下文中释放 PBUF_RAM 的内存，这一操作是通过在分配和释放内存过程前后加保护（**信号量和关中断**）来实现的。而在 rt-thread 操作系统中，堆内存分配过程并不允许在中断上下文中进行，对于内存的分配和释放过程只是使用了**信号量**来做保护。

## 错误复现情况

这就带来了潜在的重入风险，考虑如下这种情况：

1. 调用 lwip 发送接口发送一个从系统堆中申请内存的数据包
2. 发送完成后对该块内存进行释放
3. 在释放的过程中发生中断，在中断中再次释放该内存
4. 中断退出后，继续内存释放过程，此时重入发生，造成未知错误

## 解决方案

此时如果想要避免出现这种重入的情况，就不能在中断里尝试进行堆内存的释放，为了处理这种情况，lwip 提供了两种方法来解决。

## 方案一

使用 lwip 自带的堆管理器，该堆管理器在允许在中断上下文进行内存分配和释放的情况下，会在内存操作过程前后加上（**信号量和关中断**）来保护内存分配过程。这种问题就目前来说有两个缺点，一个是关闭中断次数过多，使得系统响应速度变慢，而是需要切换堆管理器，目前不可取。

### 方案二

使用 lwip 提供的一种延时释放机制，也就是替换相应的内存释放函数为  `pbuf_free_callback(p)` 和`mem_free_callback(m)` 。如果在中断里调用这两个函数来释放内存，则不会立刻释放，而是向 `tcpip` 线程发邮件，当中断退出后，在 `tcpip` 线程中收到邮件，然后执行 call back 进行内存释放。

使用这种方案解决问题比较简单，只需要将 `pbuf_free` 函数进行修改即可，修改后的代码如下所示：

```c
      {
        /* is this a pbuf from the pool? */
        if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF_POOL) {
          memp_free(MEMP_PBUF_POOL, p);
          /* is this a ROM or RAM referencing pbuf? */
        } else if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_MEMP_PBUF) {
          memp_free(MEMP_PBUF, p);
          /* type == PBUF_RAM */
        } else if (alloc_src == PBUF_TYPE_ALLOC_SRC_MASK_STD_HEAP) {
          mem_free_callback(p);   /* change mem_free to mem_free_callback */
        } else {
          /* @todo: support freeing other types */
          LWIP_ASSERT("invalid pbuf type", 0);
        }
      }
```

## 结论

如果对内存系统的原理搞不清楚，内存分配又会同时发生在线程环境和中断环境，此时就有内存分配过程被重入的危险（**当然都是中断去重入线程，因为中断优先级最高**），系统中出现的错误很多时候都是由于内存出了问题。重入问题由于其发生时的情况不定，错误时间不定，因此相对难于排查。

因此，需要对系统中的内存分配做到心中十分清楚，这一块的分配算法要十分熟悉才行。