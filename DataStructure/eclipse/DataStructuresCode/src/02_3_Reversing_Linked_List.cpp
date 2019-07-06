/*
 * Reversing_Linked_List.cpp
 *
 *  Created on: 2018年4月6日
 *      Author: Administrator
 */

/*
02-线性结构3 Reversing Linked List（25 分）

Given a constant K and a singly linked list L, you are supposed to reverse the links of every K elements on L.
 For example, given L being 1→2→3→4→5→6, if K = 3, then you must output 3→2→1→6→5→4; if K = 4,
 you must output 4→3→2→1→5→6.

Input Specification:

Each input file contains one test case. For each case, the first line contains the address of the first node,
a positive N (<= 105) which is the total number of nodes,
and a positive K (<=N) which is the length of the sublist to be reversed.
The address of a node is a 5-digit nonnegative integer, and NULL is represented by -1.

Then N lines follow, each describes a node in the format:

Address Data Next

where Address is the position of the node, Data is an integer, and Next is the position of the next node.

Output Specification:

For each case, output the resulting ordered linked list. Each node occupies a line,
and is printed in the same format as in the input.

Sample Input:
00100 6 4
00000 4 99999
00100 1 12309
68237 6 -1
33218 3 00000
99999 5 68237
12309 2 33218
Sample Output:
00000 4 33218
33218 3 12309
12309 2 00100
00100 1 99999
99999 5 68237
68237 6 -1
 */


#include<stdio.h>
#include<stdlib.h>

struct DataNode {
    int addr;
    int data;
    int nextaddr;
    struct DataNode *link;
};
typedef struct DataNode *DataList;

// 系数 指数 需要将节点插入链表的尾部指针
static void Attach(int addr, int data, int nextaddr, DataList *pRear) {
    DataList P;

    P = (DataList) malloc(sizeof(struct DataNode));
    P->addr = addr;
    P->data = data;
    P->nextaddr = nextaddr;
    P->link = NULL;
    (*pRear)->link = P; //让尾节点的指针指向新的节点
    *pRear = P;         //更新尾指针指向的位置
}

int first_addr, data_num, k;

//1、将数据读入一个链表内
DataList DataNode_read() {

    int addr, data, nextaddr , datanumber;
    DataList P, Rear, t;

    scanf("%d %d %d", &first_addr, &data_num, &k);
    datanumber = data_num;

    P = (DataList) malloc(sizeof(struct DataNode));
    P->link = NULL;
    Rear = P;

    while (datanumber--) {
        scanf("%d %d %d", &addr, &data, &nextaddr);
        Attach(addr, data, nextaddr, &Rear);
    }

    t = P;
    P = P->link;
    free(t);

    return P;
}

//2、对链表进行重新排序，查找之前生成的链表中第一个地址的节点插入
//然后根据下一个地址查找后插入

DataList DataNode_rearrangement(DataList List, int firstaddr, int data_num) {
    DataList P, Rear, t;
    P = (DataList) malloc(sizeof(struct DataNode));
    P->link = NULL;
    Rear = P;
    DataList List_t = List;

    while (data_num--) {
        while (List_t) {
            if (List_t->addr == firstaddr) {
                Attach(List_t->addr, List_t->data, List_t->nextaddr, &Rear);
                firstaddr = List_t->nextaddr;
                break;
            } else {
                List_t = List_t->link;
            }
        }
        List_t = List;
    }

    t = P;
    P = P->link;
    free(t);

    return P;
}

//链表的反转
DataList ListReversing(DataList List) {
    DataList p = List, newH = NULL;
    while (p != NULL)                 //一直迭代到链尾
    {
        if (p->link != NULL) {
            p->link->nextaddr = p->addr;
        }

        DataList tmp = p->link;       //暂存p下一个地址，防止变化指针指向后找不到后续的数
        p->link = newH;               //p->next指向前一个空间
        newH = p;                     //新链表的头移动到p，扩长一步链表
        p = tmp;                   //p指向原始链表p指向的下一个空间
    }

    return newH;
}

void DataListprint(DataList List);

DataList DataListReversing(DataList List, int data_num, int k) {
    int z,remainder;
    int i = 1;
    int flag = 1;
    z = data_num / k;
    remainder = data_num % k;

    if(k == 1)
        return List;

    DataList P, Rear, t;
    P = (DataList) malloc(sizeof(struct DataNode));
    P->link = NULL;
    Rear = P;

    DataList List_t = List;
    int times_inter, times_out;
    DataList list_last_time = NULL, save_addr = NULL, next_addr = NULL;

    //如果刚好余数为0，那么要每次截取  K 个来反转，截取 z 次
    while (i <= z) {
        times_inter = k;

        while (--times_inter) {
            List_t = List_t->link;
        }

        if (List_t->link != NULL) {
            next_addr = List_t->link;   //将下一个链表的第一个元素的地址存起来 这里可能等于 NULL
        }

        List_t->link = NULL;

        if (i == 1) {
            List_t = List;
        } else {
            List_t = next_addr;
        }

        //因为要反转链表 所以这时候放进去的 List_t 会是反转后的最后一个元素的指针
        save_addr = List_t;         //这一轮没有反转前的最后一个元素的地址，用到最后表示链表的最后一个元素
        list_last_time = ListReversing(List_t);

        Rear->link = list_last_time;                 //将新的反转好的链表加入到新链表中
        Rear->nextaddr = list_last_time->addr;

        Rear = save_addr;                            //Rear指针指向新链表最后一个元素

        List_t = next_addr;                          //

        i++;
    }

    if (!remainder) {
        Rear->nextaddr = -1;   //这里需要反转后的最后一个元素
    } else {
        Rear->link = next_addr;
        Rear->nextaddr = next_addr->addr;
    }

    t = P;
    P = P->link;
    free(t);

    return P;
}

void DataListprint(DataList List) {
    while (List) {
        if (List->nextaddr != -1) {
            printf("%05d %d %05d\n", List->addr, List->data, List->nextaddr, List);
        } else {
            printf("%05d %d %d\n", List->addr, List->data, List->nextaddr, List);
        }
        List = List->link;
    }
}

//int main() {
//    DataList save, p, p_rearrangement, p_output;
//    int count = 0;
//    //1、读入链表
//    p = DataNode_read();
////  printf("读入的链表如下： \n");
////  DataListprint(p);
//    //2、重新排序
//    p_rearrangement = DataNode_rearrangement(p, first_addr, data_num);
//    save = p_rearrangement;
////  printf("重新排序后的链表如下： \n");
////  DataListprint(p_rearrangement);
//
//    while (p_rearrangement) {
//        count++;
//        p_rearrangement = p_rearrangement->link;
//    }
//
//    //printf("反转后的链表如下： p_output = %p \n", p_output);
//    //3、反转链表
//    p_output = DataListReversing(save, count, k);
//    //4、输出链表
////  printf("反转后的链表如下： p_output = %p \n", p_output);
//    DataListprint(p_output);
//
//    return 0;
//}


