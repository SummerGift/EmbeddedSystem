/*
 * Tree_isomorphism.cpp
 *
 *  Created on: 2018年4月14日
 *      Author: SummerGift
 */

/*
03-树1 树的同构（25 分）

给定两棵树T1和T2。如果T1可以通过若干次左右孩子互换就变成T2，则我们称两棵树是“同构”的。
例如图1给出的两棵树就是同构的，因为我们把其中一棵树的结点A、B、G的左右孩子互换后，就得到另外一棵树。而图2就不是同构的。

现给定两棵树，请你判断它们是否是同构的。
输入格式:

输入给出2棵二叉树树的信息。对于每棵树，首先在一行中给出一个非负整数N (≤10)，即该树的结点数（此时假设结点从0到N−1编号）；
随后N行，第i行对应编号第i个结点，给出该结点中存储的1个英文大写字母、其左孩子结点的编号、右孩子结点的编号。
如果孩子结点为空，则在相应位置上给出“-”。给出的数据间用一个空格分隔。注意：题目保证每个结点中存储的字母是不同的。

输出格式:

如果两棵树是同构的，输出“Yes”，否则输出“No”。

输入样例1（对应图1）：

8
A 1 2
B 3 4
C 5 -
D - -
E 6 -
G 7 -
F - -
H - -
8
G - 4
B 7 6
F - -
A 5 1
H - -
C 0 -
D - -
E 2 -
输出样例1:

Yes
输入样例2（对应图2）：

8
B 5 7
F - -
A 0 3
C 6 -
H - -
D - -
G 4 -
E 1 -
8
D 6 -
B 5 -
E - -
H - -
C 0 2
G - 3
F - -
A 1 4
输出样例2:

No
*/

#include<stdio.h>
#include<stdlib.h>

#define MaxTree 10
#define ElementType char
#define Tree int
#define Null -1

struct TreeNode
{
	ElementType data;
	Tree Left;
	Tree Right;
} T1[MaxTree], T2[MaxTree];

static Tree buildtree(struct TreeNode T[]) {
	int N, check[MaxTree], root = Null;
	char cl, cr;

	scanf("%d\n", &N);

	if (N) {
		for (int i = 0; i < N; i++)
			check[i] = 0;

		for (int i = 0; i < N; i++) {
			scanf("%c %c %c\n", &T[i].data, &cl, &cr); //将数据输入，建立树，查找 ROOT

			if (cl != '-') {
				T[i].Left = cl - '0';
				check[T[i].Left] = 1;
			} else {
				T[i].Left = Null;
			}
			if (cr != '-') {
				T[i].Right = cr - '0';
				check[T[i].Right] = 1;
			} else {
				T[i].Right = Null;
			}
		}

		for (int i = 0; i < N; i++)
			if (!check[i]) {
				root = i;                   //T[i]中沒有任何节点的left和 right指向他，就是根节点
				break;
			}
	}
	return root;
}

static int Isomorphic( Tree R1, Tree R2) {
	if ((R1 == Null) && (R2 == Null))  //都是空树，同构
		return 1;
	if (((R1 == Null) && (R2 != Null)) || ((R1 != Null) && (R2 == Null)))
		return 0;                      //有一个是空树，不是同构
	if (T1[R1].data != T2[R2].data)
		return 0;                      //树根不一样，不是同构
	if ((T1[R1].Left == Null) && (T2[R2].Left == Null))   //都没有左树，判断右树是否相同
		return Isomorphic(T1[R1].Right, T2[R2].Right);

	if ((T1[R1].Left != Null) && (T2[R2].Left != Null)
			&& (T1[T1[R1].Left].data == T2[T2[R2].Left].data))   //两树左子树皆不空，且值相等
		return (Isomorphic(T1[R1].Left, T2[R2].Left)
				&& Isomorphic(T1[R1].Right, T2[R2].Right));     //判断其子树
	else {
		return (Isomorphic(T1[R1].Left, T2[R2].Right) &&        //交换左右子树判断
				Isomorphic(T1[R1].Right, T2[R2].Left));
	}
}

//int main()
//{
//	Tree r1, r2;
//	r1 = buildtree(T1);
//	r2 = buildtree(T2);
//
//	if (Isomorphic(r1, r2)) {
//		printf("Yes\n");
//	} else {
//		printf("No\n");
//	}
//
//	return 0;
//}


