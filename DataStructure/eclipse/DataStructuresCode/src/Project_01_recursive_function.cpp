#include<stdio.h>
#include<stdlib.h>
#include<iostream>
#include<string.h>

typedef int ElementType;
typedef struct Node *PtrToNode;
struct Node {
    ElementType Data;
    PtrToNode Next;
};
typedef PtrToNode List;

using namespace std;

//递归求和
unsigned int sum(int n) {
    if (n > 1)
        return n + sum(n - 1);
    else if (n == 1) {
        return 1;
    }
}

//求斐波那契数列的第N个值
unsigned int fac(int n) {
    if (n >= 3) {
        return fac(n - 1) + fac(n - 2);
    }

    if (n == 2)
        return 1;

    if (n == 1)
        return 1;
}

//求字符串的长度
unsigned int get_strlen(char *s) {
    if (*s != '\0')
        return 1 + get_strlen(s + 1);
    if (*s == '\0')
        return 0;
}

//链表的反转
List reverse(List list) {
    if ((list == NULL || list->Next == NULL)) {
        return list;
    } else {
        List guard = list->Next;
        List ret = reverse(list->Next);
        guard->Next = list;
        list->Next = NULL;
        return ret;
    }
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

//汉诺塔问题 从 a 到 c, b 为中转站
void HanoiTower(int n, char a, char b, char c) {
    if (n == 1) {
        cout << a << "-->" << c << endl;
    } else {
        HanoiTower(n - 1, a, c, b);
        HanoiTower(1, a, b, c);
        HanoiTower(n - 1, b, a, c);
    }
}

//全排列问题
void permutation(char *s, char *e) {
    if (*s == '\0') {
        cout << e << endl;
    } else {
        int len = strlen(s);

        for (int i = 0; i < len; i++) {
            if ((i == 0) || (s[0] != s[i])) {
                swap(s[0], s[i]);
                permutation(s + 1, e);
                swap(s[0], s[i]);
            }
        }
    }
}

void list_attach(int data,List *pRear) {
    List P;

    P = (List) malloc(sizeof(struct Node));
    P->Data = data;
    P->Next = NULL;

    (*pRear)->Next = P; //让尾节点的指针指向新的节点
    *pRear = P;         //更新尾指针指向的位置
}

List create_list(int n) {

    List P, Rear, t;

    P = (List) malloc(sizeof(struct Node));
    P->Next = NULL;
    Rear = P;

    for (int i = 0; i < n; i++) {
        list_attach(i, &Rear);
    }

    t = P;
    P = P->Next;
    free(t);

    return P;
}

void print_list(List P) {
    if (!P) {
        printf("null \n");
        return;
    }

    while (P) {
        printf("%d\n", P->Data);
        P = P->Next;
    }

    printf("\n");
}

//递归实现的反向输出链表中的偶数
void r_print_even(List list) {
    if (list != NULL) {
        r_print_even(list->Next);

        if ((list->Data % 2) == 0) {
            cout << list->Data << endl;
        }
    }
}


//*********************************************************************************
//     八皇后问题
//*********************************************************************************
static int g_chessboard[8] = { 0 }, gCount = 0;

void print() //输出每一种情况下棋盘中皇后的摆放情况
{
    for (int i = 0; i < 8; i++) {
        int inner;
        for (inner = 0; inner < g_chessboard[i]; inner++)
            cout << "0";
        cout << "#";
        for (inner = g_chessboard[i] + 1; inner < 8; inner++)
            cout << "0";
        cout << endl;
    }
    cout << "==========================\n";
}

int check_pos_valid(int loop, int value)    //检查是否存在有多个皇后在同一行/列/对角线的情况
        {
    int index;
    int data;
    for (index = 0; index < loop; index++) {
        data = g_chessboard[index];
        if (value == data)
            return 0;
        if ((index + data) == (loop + value))
            return 0;
        if ((index - data) == (loop - value))
            return 0;
    }
    return 1;
}

void eight_queen(int index) {
    int loop;
    for (loop = 0; loop < 8; loop++) {
        if (check_pos_valid(index, loop)) {
            g_chessboard[index] = loop;
            if (7 == index) {
                gCount++, print();
                g_chessboard[index] = 0;
                return;
            }
            eight_queen(index + 1);
            g_chessboard[index] = 0;
        }
    }
}

int main(int argc, char*argv[]) {
    eight_queen(0);
    cout << "total=" << gCount << endl;
    return 0;
}

//int main() {
//	cout << "sum :" << sum(100) << endl;
//
//	for(int i = 1;i <= 10;i++)
//		cout << "fac " << i << ":" << fac(i) << endl;
//
//	cout << "str len:" << get_strlen("1234") << endl;
//
//	HanoiTower(3, 'a', 'b', 'c');

//	char string[] = "aaa";
//	permutation(string,string);

//    List test = create_list(7);
//    print_list(test);
//    r_print_even(test);

//    return 0;
//}
