/*
 * 04_4_IsSameBinarySearchTree.cpp
 *
 *  Created on: 2018年4月27日
 *      Author: SummerGift
 */

/*
 * 04-树4 是否同一棵二叉搜索树（25 分）
给定一个插入序列就可以唯一确定一棵二叉搜索树。然而，一棵给定的二叉搜索树却可以由多种不同的插入序列得到。例如分别按照序列{2, 1, 3}和{2, 3, 1}插入初始为空的二叉搜索树，都得到一样的结果。于是对于输入的各种插入序列，你需要判断它们是否能生成一样的二叉搜索树。

输入格式:
输入包含若干组测试数据。每组数据的第1行给出两个正整数N (≤10)和L，分别是每个序列插入元素的个数和需要检查的序列个数。第2行给出N个以空格分隔的正整数，作为初始插入序列。最后L行，每行给出N个插入的元素，属于L个需要检查的序列。

简单起见，我们保证每个插入序列都是1到N的一个排列。当读到N为0时，标志输入结束，这组数据不要处理。

输出格式:
对每一组需要检查的序列，如果其生成的二叉搜索树跟对应的初始序列生成的一样，输出“Yes”，否则输出“No”。

输入样例:
4 2
3 1 4 2
3 4 1 2
3 2 4 1
2 1
2 1
1 2
0
输出样例:
Yes
No
No
*/

//解本题用到的方法是先建立一棵树，然后依照这个树分别判别后面的L个序列能否与T形成同一个搜索树并输出结果
#include <stdio.h>
#include <stdlib.h>

typedef struct treenode *tree;
struct treenode {
	int v;
	tree left, right;
	int flag;
};

//创建一个新的结点
tree new_node(int v) {
	tree t = (tree) malloc(sizeof(struct treenode));
	t->v = v;
	t->left = t->right = NULL;
	t->flag = 0;
	return t;
}

//向节点插入数据
tree insert_tree(tree t, int v) {
	if (!t)
		t = new_node(v);              //如果是一棵空树，那么创建一个新节点作为树根
	else {
		if (v > t->v) {
			t->right = insert_tree(t->right, v);
		} else {
			t->left = insert_tree(t->left, v);
		}
	}
	return t;
}

//创建一棵搜索树
tree make_tree(int n) {
	tree t;
	int i, v;

	scanf("%d", &v);

	t = new_node(v);
	for (i = 1; i < n; i++) {
		scanf("%d", &v);
		t = insert_tree(t, v);
	}
	return t;
}

//在树t中搜索整数v，看看v是否符合要求
int check(tree t, int v) {
	if (t->flag) {
		if (v < t->v)
			return check(t->left, v);
		else if (v > t->v)
			return check(t->right, v);
	} else {
		if (v == t->v) {
			t->flag = 1;
			return 1;
		} else {
			return 0;
		}

	}
}

//判断一组数据插入后是否和树t相同,返回1则相同，返回0则不相同
//判断flag的原因是，即使已经知道不一致了，但是如果不继续将后面的数据输入，会导致程序错误
int judge(tree t, int n) {
	int i, v, flag = 0;
	scanf("%d", &v);
	if (v != t->v)           //如果树根不相等，直接返回0，即不同
		flag = 1;
	else
		t->flag = 1;         //这个节点比较过

	for (i = 1; i < n; i++) {
		scanf("%d", &v);
		if ((!flag) && !check(t, v))
			flag = 1;
	}

	if (flag)
		return 0;
	else
		return 1;
}

void reset_tree(tree t) {
	if (t->left)
		reset_tree(t->left);
	if (t->right)
		reset_tree(t->right);
	t->flag = 0;
}

void free_tree(tree t) {
	if (t->left)
		free_tree(t->left);
	if (t->right)
		free_tree(t->right);
	free(t);
}

/*
读入 n 和 l
根据第一行序列建立树T
依据树T分别判断后面的L个序列是否能与T形成同一搜索树并输出结果
如何判别序列是否与树是一致的呢？
方法：在树T中按顺序搜索序列中的每个数
- 如果每次搜索所经过的节点在前面均为出现过，则一致
- 否则，如果某次搜索中遇到前面未出现的节点，则不一致
*/
int main() {
	int n, l, i;
	tree t;

	scanf("%d", &n);        //输入后面每个序列的节点数

	while (n) {
		scanf("%d", &l);   //获得后面有几组数据，然后分别判定每一组数据
		t = make_tree(n);
		for (i = 0; i < l; i++) {
			if (judge(t, n)) {
				printf("Yes\n");
			} else {
				printf("No\n");
			}
			reset_tree(t);
		}
		free_tree(t);
		scanf("%d", &n);    //读入下一个N也就是下一组数据的节点个数，开启下一个循环
	}

	return 0;
}




