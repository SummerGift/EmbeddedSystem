/*
 * BinarySearch.cpp
 *
 *  Created on: 2018年4月7日
 *      Author: Summer
 */


/*
 * 01-复杂度3 二分查找（20 分）
 本题要求实现二分查找算法。

 函数接口定义：

 Position BinarySearch( List L, ElementType X );
 其中List结构定义如下：

 typedef int Position;
 typedef struct LNode *List;
 struct LNode {
	 ElementType Data[MAXSIZE];
	 Position Last;
 };

 L是用户传入的一个线性表，其中ElementType元素可以通过>、==、<进行比较，
 并且题目保证传入的数据是递增有序的。函数BinarySearch要查找X在Data中的位置，
 即数组下标（注意：元素从下标1开始存储）。
 找到则返回下标，否则返回一个特殊的失败标记NotFound。

 裁判测试程序样例：

#include <stdio.h>
#include <stdlib.h>

#define MAXSIZE 10
#define NotFound 0
typedef int ElementType;

typedef int Position;
typedef struct LNode *List;
struct LNode {
    ElementType Data[MAXSIZE];
    Position Last;
};

List ReadInput(); //
Position BinarySearch( List L, ElementType X );

int main()
{
    List L;
    ElementType X;
    Position P;

    L = ReadInput();
    scanf("%d", &X);
    P = BinarySearch( L, X );
    printf("%d\n", P);

    return 0;
}

// 你的代码将被嵌在这里

输入样例1：

5
12 31 55 89 101
31
输出样例1：

2
输入样例2：

3
26 78 233
31
输出样例2：

0

*/

#define MAXSIZE 10
#define NotFound 0
typedef int ElementType;

typedef int Position;
typedef struct LNode *List;
struct LNode {
    ElementType Data[MAXSIZE];
    Position Last;
};

Position BinarySearch( List L, ElementType X )
{
  int low, mid, high;
  low = 1;
  high = L->Last;
  while( low <= high  )
  {
    mid = (low + high)/2;
    if ( L->Data[mid] < X )
        low = mid + 1;
    else if( L->Data[mid] > X )
    {
        high = mid - 1;
    }else{
        return mid;
    }
   }
  return NotFound;
}

