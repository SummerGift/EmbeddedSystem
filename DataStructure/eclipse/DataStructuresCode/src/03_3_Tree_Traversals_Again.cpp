/*
 * 03_3_Tree_Traversals_Again.cpp
 *
 *  Created on: 2018年4月22日
 *      Author: SummerGift
 */

/*
03-树3 Tree Traversals Again（25 分）
An inorder binary tree traversal can be implemented in a non-recursive way with a stack.
For example, suppose that when a 6-node binary tree (with the keys numbered from 1 to 6) is traversed,
the stack operations are: push(1); push(2); push(3); pop(); pop(); push(4); pop(); pop(); push(5); push(6); pop(); pop().
Then a unique binary tree (shown in Figure 1) can be generated from this sequence of operations.
Your task is to give the postorder traversal sequence of this tree.

一个中序遍历的二叉树可以使用一个栈结构来非递归遍历。比如说一个六个节点的二叉树被遍历，带着值 1 - 6。
栈的操作为 P1  P2 P3 POP POP P4 POP POP P5 P6 POP POP
接下来特殊的二叉树可以被这一系列的操作产生。
你的任务是给出这棵树的后序遍历序列。

Figure 1
Input Specification:

Each input file contains one test case. For each case,
the first line contains a positive integer N (≤30) which is the total number of nodes in a tree (and hence the nodes are numbered from 1 to N).
Then 2N lines follow, each describes a stack operation in the format: "Push X" where X is the index of the node being pushed onto the stack;
or "Pop" meaning to pop one node from the stack.

第一行包括一个小于等于30的正整数，代表树总共的结点数。接下来会有2N行，每一行描述栈操作 PUSH X 指的是将这个节点压入栈中，POP 操作的意思是将节点从栈中弹出。

Output Specification:

For each test case, print the postorder traversal sequence of the corresponding tree in one line.
A solution is guaranteed to exist. All the numbers must be separated by exactly one space,
and there must be no extra space at the end of the line.

用一行打出相关树的后序输出。

Sample Input:

6
Push 1
Push 2
Push 3
Pop
Pop
Push 4
Pop
Pop
Push 5
Push 6
Pop
Pop

Sample Output:

3 4 2 6 5 1
*/

/*
 * 递归遍历和非递归中序遍历树，递归遍历是先递归的访问树的左子树，然后打印出根节点，然后递归访问右子树。
 * 而非递归中序遍历的情况，是我们自己创建一个栈，先将根节点入栈，然后访问左子树，然后把子树的根节点压入栈。
 * 在非递归中序遍历 PUSH 的顺序是先序遍历的顺序，POP 的顺序相当于是中序遍历的结果。
 * 这个题目就变成了知道了一个树的先序和中序遍历结果，求后续遍历结果。
*/

#include<iostream>
#include<string>
#include<vector>
#include<stack>

using namespace std;


//将输入的内容纪录下来，分别存入先序遍历数组和中序遍历数组
vector<vector<int>> getorder(int n) {
	vector<int> preorder(n, 0);
	vector<int> inorder(n, 0);
	stack<int> st;
	int prel = 0, inl = 0;

	for (int i = 0; i < 2 * n; i++) {
		string str;
		int tmp;
		cin >> str;
		if (str == "Push") {
			cin >> tmp;
			preorder[prel++] = tmp;
			st.push(tmp);
		} else if (str == "Pop") {
			inorder[inl++] = st.top();
			st.pop();
		}
	}

	return {preorder, inorder};
}

//输入先序遍历数组和中序遍历数组，返回后序遍历数组

void getpostorder(vector<int> preorder, int prel, vector<int> inorder, int inl,
		vector<int> &postorder, int postl, int n) {
	if (n == 0)
		return;
	if (n == 1) {
		postorder[postl] = preorder[prel];
		return;
	}

	auto root = preorder[prel];        //从先序遍历的第一个节点拿到根节点
	postorder[postl + n - 1] = root;    //放在后序遍历的最后一个位置上

	//在中序遍历数组上找出 ROOT 的位置
	int i = 0;
	while (i < n) {
		if (inorder[inl + i] == root)
			break;
		i++;
	}

	//计算出 ROOT 节点左右子树节点的个数
	int L = i, R = n - i - 1;

	getpostorder(preorder, prel + 1, inorder, inl, postorder, postl, L);
	getpostorder(preorder, prel + L + 1, inorder, inl + L + 1, postorder,
			postl + L, R);
}



/*
 * 先将先序和中序遍历的输出存入数组，然后再从中得到后序遍历的输出。
 */

int main() {
	int n;
	cin >> n;
	auto res = getorder(n);
	vector<int> postorder(n, 0);
	getpostorder(res[0], 0, res[1], 0, postorder, 0, n);

	int i = 0;
	for (; i < n - 1; i++) {
		cout << postorder[i] << " ";
	}
	cout << postorder[i] << endl;
	return 0;
}

















