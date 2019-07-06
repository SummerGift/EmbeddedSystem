#include <stdio.h>
#include <stdlib.h>

static struct test_struct {
    int data1;
    int data2;
}zzz1,zzz2;

//间接引用操作符可以直接获得整个结构体大小的内存数据，这取决于所引用的指针变量的类型
int indirection_operator() {
    setbuf(stdout,NULL);
    zzz1.data1 = 15;
    zzz1.data2 = 55;
    zzz2 = *((struct test_struct *)&zzz1);

    printf("zzz1.data1: %d, zzz1.data2 = %d\n",zzz1.data1,zzz1.data2);
    printf("zzz1.data1: %d, zzz1.data2 = %d\n",zzz1.data1,zzz1.data2);
    printf("zzz1.data1: %d, zzz1.data2 = %d\n",zzz1.data1,zzz1.data2);

    return 0;
}
