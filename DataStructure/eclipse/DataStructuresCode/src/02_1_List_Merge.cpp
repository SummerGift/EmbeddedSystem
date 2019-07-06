/*
 * List_Merge.cpp
 *
 *  Created on: 2018年4月7日
 *      Author: Summer
 */

/*
 *
02-线性结构1 两个有序链表序列的合并（15 分）
本题要求实现一个函数，将两个链表表示的递增整数序列合并为一个非递减的整数序列。

函数接口定义：

List Merge( List L1, List L2 );
其中List结构定义如下：

typedef struct Node *PtrToNode;
struct Node {
    ElementType Data; // 存储结点数据
    PtrToNode   Next; // 指向下一个结点的指针
};
typedef PtrToNode List; // 定义单链表类型
L1和L2是给定的带头结点的单链表，其结点存储的数据是递增有序的；函数Merge要将L1和L2合并为一个非递减的整数序列。应直接使用原序列中的结点，返回归并后的带头结点的链表头指针。

裁判测试程序样例：

#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;
typedef struct Node *PtrToNode;
struct Node {
    ElementType Data;
    PtrToNode   Next;
};
typedef PtrToNode List;

List Read(); // 细节在此不表
void Print( List L ); // 细节在此不表；空链表将输出NULL

List Merge( List L1, List L2 );

int main()
{
    List L1, L2, L;
    L1 = Read();
    L2 = Read();
    L = Merge(L1, L2);
    Print(L);
    Print(L1);
    Print(L2);
    return 0;
}

// 你的代码将被嵌在这里

输入样例：

3
1 3 5
5
2 4 6 8 10
输出样例：

1 2 3 4 5 6 8 10
NULL
NULL

 */
#include <stdio.h>
#include <stdlib.h>
typedef int ElementType;
typedef struct Node *PtrToNode;
struct Node {
    ElementType Data;
    PtrToNode   Next;
};
typedef PtrToNode List;


List Merge( List L1, List L2 )
{
	List p1 = L1->Next;
	List p2 = L2->Next;
	List L = (List)malloc(sizeof(struct Node));
	List p = L;

	while(p1 != NULL && p2 != NULL)
	{
		if(p1->Data < p2->Data)
		{
			p->Next = p1;
			p1 = p1->Next;
			L1->Next = p1;
			p = p->Next;
		} else
		{
			p->Next = p2;
			p2 = p2->Next;
			L2->Next = p2;
			p = p->Next;
		}
	}

if(p1 != NULL)
{
	p->Next = p1;
	L1->Next = NULL;
}

if(p2 != NULL)
{
	p->Next = p2;
	L2->Next = NULL;
}

return L;

}

//有序链表的合并
List List_Merge(List L1, List L2) {
	if (L1 == NULL) {
		return L2;
	} else if (L2 == NULL) {
		return L1;
	} else if (L1->Data <= L2->Data) {
		List list_1 = L1->Next;
		List list = List_Merge(list_1, L2);
		L1->Next = list;
		return L1;
	} else {
		List list_2 = L2->Next;
		List list = List_Merge(list_2, L1);
		L2->Next = list;
		return L2;
	}
}

List Recurse_Merge(List L1, List L2) {
	List L = (List) malloc(sizeof(struct Node));
	List L1_ = L1->Next;
	List L2_ = L2->Next;
	L1->Next = NULL;
	L2->Next = NULL;

	L->Next = List_Merge(L1_, L2_);
	return L;
}





