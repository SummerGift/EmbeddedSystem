# 深入探索 container_of 宏原理

`container_of` 宏往往用于通过结构体的成员变量地址来获取该结构体的首地址，其实现如下：


```c
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
```

该宏右半部分的原理如下：

1. 将数字 0 强制类型转换为一个 type * 类型的指针
2. 访问其成员变量的地址
3. 将该地址转换为 unsigned long 类型的数字，来获取该成员变量距离结构体首地址的偏移量

该宏左半部分的原理如下：

1. 将成员变量的指针转换为 char * 类型，转换为 char * 类型的原因是，该类型减去一个数字，`sizeof(char) `刚好是一个字节

将强转为 char * 类型的指针减去一个 unsigned long 的数字，刚好会获得该结构体的首地址，然后将该地址转换为 type * 类型，就完成了最终的计算。

## 详细分析

测试的过程中，在 64 位 `GCC` 环境下写了一些中间函数，如下所示：

```c
struct list_head
{
    struct list_hread *next, *prev;
};

struct file_member
{
    char *para1;
    int para2;
    struct list_head list;
    double hello;
};

struct file_member test_hello;

int main()
{
    printf("test_hello ptr: %p \n", &test_hello);
    printf("test_hello ptr: %u \n", sizeof(struct file_member));
    printf("double size ptr: %u \n", sizeof(double));
    printf("double size ptr: %u \n", sizeof(char *));

    printf("test_hello->list ptr: %p \n", &(test_hello.list));
    printf("&(test_hello.list): %p \n", &(test_hello.list));
    printf("&((struct file_member *)0)->list: %p \n", &((struct file_member *)0)->list);
    printf("&((struct file_member *)0)->list: %lu \n", (unsigned long)&((struct file_member *)0)->list);

    printf("entry1 : %p \n", &(test_hello.list) - &(((struct file_member *)0)->list));
    printf("entry2 : %p \n", (char *)&(test_hello.list) - (unsigned long)&((struct file_member *)0)->list);
    printf("entry3 : %p \n", &(test_hello.list) - 1);
    printf("entry3 : %p \n", (char *)&(test_hello.list) - 1);
    printf("entry: %p \n", rt_container_of(&(test_hello.list), struct file_member, list));
}
```

运行结果如下：

```c
test_hello ptr: 0x5613c41a1040 
test_hello ptr: 40 
double size ptr: 8 
double size ptr: 8 
test_hello->list ptr: 0x5613c41a1050 
&(test_hello.list): 0x5613c41a1050 
&((struct file_member *)0)->list: 0x10 
&((struct file_member *)0)->list: 16 
test_hello entry1 : 0x5613c41a104   （这个数据令人疑惑，为什么小了这么多）
test_hello entry2 : 0x5613c41a1040 
test_hello entry3 : 0x5613c41a1040 
test_hello entry3 : 0x5613c41a104f 
test_hello entry: 0x5613c41a1040
```

`entry1` 的测试代码指针计算是像下面这样写的:

```c
 &(test_hello.list) - &(((struct file_member *)0)->list)
```

这样写犯了一个明显的错误，就是将两种不同类型的指针进行了减法操作，这就会产生二义性。被减去的这个指针导致是按照什么类型被减去呢，是某种类型的指针？还是一个常数？

从最终的结果 `0x5613c41a104` 来看，编译器实际执行的动作如下：

1. 将 `&(((struct file_member *)0)->list)` 转化成与 `&(test_hello.list)` 同样类型的指针，也就是 `struct list_head *` 类型
2. 将两个指针相减，获取指针之间的差值，然后除以 `sizeof(struct list_head)` ，算出两个指针之间间隔了多少个 `struct list_head` 大小的结构体
3. 所以最终的结果看起来就是 `（0x5613c41a1050 - 0x10）/ 0x10 = 0x5613c41a104`

这里面的原理如下所述：

 ```c
 int *a;
 int *b;
 
 a、b 是两个相同类型的指针。
 
 如果 b 等于 a + 4;
 那么 b 减去 a 也要等于 4，这其中的差值并不是地址之差，而是他们之间有多少个 int 类型的数据。
 ```



