# RT-Smart 页表初始化过程详解

想要对 RT-Smart 的页表内存管理功能有所了解，需要熟悉相关代码：

- RT-Smart 页初始化相关功能
- 物理页分配算法伙伴系统的实现

## 物理页内存管理

在系统初始化早期，会先执行 `rt_page_init` 函数来对物理页管理所需要的数据结构进行初始化，下面是对这段代码的详细解释：

```c
#define ARCH_PAGE_SHIFT     12
#define ARCH_PAGE_SIZE      (1 << ARCH_PAGE_SHIFT)
#define ARCH_PAGE_MASK      (ARCH_PAGE_SIZE - 1) /* b 1111 1111 1111  */

/* 从这 PAGE_START PAGE_END 可以看出，分配给物理页的地址是从 KERNEL_VADDR_START 开始的第 16M 到 128M 之间  */
#define HEAP_END        (void*)(KERNEL_VADDR_START + 16 * 1024 * 1024)
#define PAGE_START      HEAP_END
#define PAGE_END        (void*)(KERNEL_VADDR_START + 128 * 1024 * 1024)

static struct page *page_list[ARCH_PAGE_LIST_SIZE];

/* 传入给页初始化函数的结构体，存储了物理页管理的地址范围 */
rt_region_t init_page_region = {
    (uint32_t)PAGE_START,
    (uint32_t)PAGE_END,
};

/* 物理页管理数据结构 */
struct page
{
    struct page *next;  /* same level next */
    struct page *pre;   /* same level pre  */
    uint32_t size_bits; /* if is ARCH_ADDRESS_WIDTH_BITS, means not free */
    int ref_cnt;        /* page group ref count */
};

static struct page* page_start;
static void*  page_addr;
static size_t page_nr;

/* 实际执行物理页管理数据结构的初始化，默认物理页大小为 4K */
void rt_page_init(rt_region_t reg)
{
    int i;

    LOG_D("split 0x%08x 0x%08x\n", reg.start, reg.end);

    /* 调整物理内存的起始地址为 4K 对齐 */
    reg.start += ARCH_PAGE_MASK;
    reg.start &= ~ARCH_PAGE_MASK;

    reg.end &= ~ARCH_PAGE_MASK;

    /* 计算管理物理页所需数据结构所占用的内存空间，以及可以有多少可以被分配的物理页 */
    {
        int nr = ARCH_PAGE_SIZE / sizeof(struct page); /* 计算一个物理页也就是 4k 可以存放多少个 page 结构体 */
        int total = (reg.end - reg.start) >> ARCH_PAGE_SHIFT;  /* 判断总共有多少个可用物理页 */
        int mnr = (total + nr) / (nr + 1);      /* 计算需要多少个页用于管理所有的内存也 */ 

        LOG_D("nr = 0x%08x\n", nr);
        LOG_D("total = 0x%08x\n", total);
        LOG_D("mnr = 0x%08x\n", mnr);

        page_start = (struct page*)reg.start;
        reg.start += (mnr << ARCH_PAGE_SHIFT);  /* 计算除去用于管理的内存页，可用于物理页分配的起始地址 */ 
        page_addr = (void*)reg.start;
        page_nr = (reg.end - reg.start) >> ARCH_PAGE_SHIFT; /* 计算有多少个物理页可供分配 */ 
    }

    LOG_D("align 0x%08x 0x%08x\n", reg.start, reg.end);

    /* 初始化 page 分配链表，用户存放已经*/ 
    /* init free list */
    for (i = 0; i < ARCH_PAGE_LIST_SIZE; i++)
    {
        page_list[i] = 0;
    }

    /* 初始化可供分配的物理页管理结构体 */
    /* init page struct */
    for (i = 0; i < page_nr; i++)
    {
        page_start[i].size_bits = ARCH_ADDRESS_WIDTH_BITS;
        page_start[i].ref_cnt = 1;
    }

    /* 将所有的空闲页释放到管理列表 */
    /* add to free list */
    for (i = 0; i < page_nr; i++)
    {
        _pages_free(page_start + i, 0);
    }
}
```

