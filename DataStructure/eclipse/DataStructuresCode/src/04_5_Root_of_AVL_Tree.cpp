/*
 * Root_of_AVL_Tree.cpp
 *
 *  Created on: 2018年4月18日
 *      Author: Administrator
 */

/*
04-树5 Root of AVL Tree（25 分）
An AVL tree is a self-balancing binary search tree.
In an AVL tree, the heights of the two child subtrees of any node differ by at most one;
if at any time they differ by more than one, rebalancing is done to restore this property.
Figures 1-4 illustrate the rotation rules.

Now given a sequence of insertions, you are supposed to tell the root of the resulting AVL tree.
Input Specification:

Each input file contains one test case. For each case,
the first line contains a positive integer N (≤20) which is the total number of keys to be inserted.
Then N distinct integer keys are given in the next line.
All the numbers in a line are separated by a space.

Output Specification:

For each test case, print the root of the resulting AVL tree in one line.

Sample Input 1:

5
88 70 61 96 120
Sample Output 1:

70
Sample Input 2:

7
88 70 61 96 120 90 65
Sample Output 2:

88

题目翻译：

一个平衡二叉树是一个自平衡的二叉查找树。在一个平衡二叉树中，任何节点的两个子树的高度差最多为1。
任何时候平衡因子大于 1 的时候，那么重新平衡这棵树就可以恢复平衡二叉树的特性。

现在给你一组插入值，你应该给出平衡二叉搜索树的根。

输入格式：
每一个文件包含一个测试点，对于每个测试，第一行给出一个正数值说明有多少个插入点。
然后 N 个插入点的正数值会在下一行给出，所有在一行的数字都被空格隔开。

输出格式：
对于每一个测试点，打在一行中打印出平衡二叉搜索树的根。

Sample Input 1:

5
88 70 61 96 120
Sample Output 1:

70
Sample Input 2:

7
88 70 61 96 120 90 65
Sample Output 2:

88
*/

#include <stdio.h>
#include <stdlib.h>

typedef struct ALVNode *AVLTree;
typedef struct ALVNode *Position;
struct ALVNode{
    int Data;
    int Height;
    AVLTree Left;
    AVLTree Right;
};

AVLTree Insert(int X, AVLTree T);
int GetHeight(Position T);
int Max(int a, int b);
Position SingleLeft(Position K);
Position SingleRight(Position K);
Position DoubleLeft(Position K);
Position DoubleRight(Position K);

int GetHeight(Position T) {
    if (!T) {
        return -1;
    } else {
        return T->Height;
    }
}

int Max(int a, int b) {
    return (a > b) ? a : b;
}

Position SingleLeft(Position K) {
    Position Tmp;
    Tmp = K;
    K = K->Left;
    Tmp->Left = K->Right;
    K->Right = Tmp;
    Tmp->Height = Max(GetHeight(Tmp->Left), GetHeight(Tmp->Right)) + 1;
    K->Height = Max(GetHeight(K->Left), GetHeight(K->Right)) + 1;
    return K;
}

Position SingleRight(Position K) {
    Position Tmp;
    Tmp = K;
    K = K->Right;
    Tmp->Right = K->Left;
    K->Left = Tmp;
    Tmp->Height = Max(GetHeight(Tmp->Left), GetHeight(Tmp->Right)) + 1;
    K->Height = Max(GetHeight(K->Left), GetHeight(K->Right)) + 1;
    return K;
}

Position DoubleLeft(Position K) {
    K->Left = SingleRight(K->Left);
    return SingleLeft(K);
}

Position DoubleRight(Position K) {
    K->Right = SingleLeft(K->Right);
    return SingleRight(K);
}

AVLTree Insert(int X, AVLTree T) {
    if (T == NULL) {
        T = (AVLTree) malloc(sizeof(struct ALVNode));
        T->Data = X;
        T->Height = 0;
        T->Left = T->Right = NULL;
    } else if (X < T->Data) {
        T->Left = Insert(X, T->Left);
        if (GetHeight(T->Left) - GetHeight(T->Right) == 2) {
            if (X < T->Left->Data)
                T = SingleLeft(T);
            else
                T = DoubleLeft(T);
        }
    } else if (X > T->Data) {
        T->Right = Insert(X, T->Right);
        if (GetHeight(T->Right) - GetHeight(T->Left) == 2) {
            if (X > T->Right->Data)
                T = SingleRight(T);
            else
                T = DoubleRight(T);
        }
    }
    T->Height = Max(GetHeight(T->Left), GetHeight(T->Right)) + 1;
    return T;
}

/*
int main() {
    AVLTree T = NULL;
    int n;
    scanf("%d", &n);
    while (n--) {
        int x;
        scanf("%d", &x);
        T = Insert(x, T);
    }
    if (T) {
        printf("%d", T->Data);
    }

    return 0;
}
*/
