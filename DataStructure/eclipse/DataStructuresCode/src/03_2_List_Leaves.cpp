/*
 * 03_2_List_Leaves.cpp
 *
 *  Created on: 2018年4月19日
 *      Author: SummerGift
 */

/*
03-树2 List Leaves（25 分）
Given a tree, you are supposed to list all the leaves in the order of top down, and left to right.

Input Specification:

Each input file contains one test case. For each case,
the first line gives a positive integer N (≤10) which is the total number of nodes in the tree -- and hence the nodes are numbered from 0 to N−1.
Then N lines follow, each corresponds to a node,
and gives the indices of the left and right children of the node.
If the child does not exist, a "-" will be put at the position.
Any pair of children are separated by a space.

Output Specification:

For each test case, print in one line all the leaves' indices in the order of top down,
and left to right. There must be exactly one space between any adjacent numbers,
 and no extra space at the end of the line.


Sample Input:

8
1 -
- -
0 -
2 7
- -
- -
5 -
4 6
Sample Output:

4 1 5
*/


//通过顺序输入，结构体数组存储的树，第一步是要找到这棵树的根节点
//根节点有一个特殊的性质就是没有人指向它，所以需要一个标记数组来记录节点是否有其他节点指向它

#include<iostream>
#include<vector>
#include<queue>

using namespace std;

typedef int tree;
#define Null -1

struct treenode {
	int data;
	tree left;
	tree right;
};

//接收数据，将数据存储起来，建树，返回树的根节点
tree buildtree(vector<treenode> &tree_in, int n) {
	tree root = -1;
	char cl, cr;
	vector<int> check(n, 0);            //定义check包含 n 个 int 类型的元素，每个元素都被初始化为0

	for (int i = 0; i < n; i++) {
		cin >> cl >> cr;
		tree_in[i].data = i;
		if (cl != '-') {
			tree_in[i].left = (int) (cl - '0');
			check[tree_in[i].left] = 1;
		} else {
			tree_in[i].left = Null;
		}

		if (cr != '-') {
			tree_in[i].right = (int) (cr - '0');
			check[tree_in[i].right] = 1;
		} else {
			tree_in[i].right = Null;
		}
	}

	for (int i = 0; i < n; i++)
		if (!check[i]) {
			root = i;
			break;
		}

	return root;
}

/*
queue 的基本操作有：
入队，如例：q.push(x); 将x 接到队列的末端。
出队，如例：q.pop(); 弹出队列的第一个元素，注意，并不会返回被弹出元素的值。
访问队首元素，如例：q.front()，即最早被压入队列的元素。
访问队尾元素，如例：q.back()，即最后被压入队列的元素。
判断队列空，如例：q.empty()，当队列空时，返回true。
访问队列中的元素个数，如例：q.size()

push_back 容器类型在尾部加入数据。
*/

vector<int> findleaves(const vector<treenode> &tree_in, tree root) {
	vector<int> leaves;
	queue<treenode> save_quque;
	treenode node;

	if (root == Null)
		return {};
	save_quque.push(tree_in[root]);
	while (!save_quque.empty()) {
		node = save_quque.front();
		save_quque.pop();

		//如果该节点的左右儿子都不存在则为叶节点
		if ((node.left == Null) && (node.right == Null))
			leaves.push_back(node.data);
		if (node.left != Null)
			save_quque.push(tree_in[node.left]);
		if (node.right != Null)
			save_quque.push(tree_in[node.right]);
	}

	return leaves;
}

//decltype 查询表达式的数据类型

/*
int main() {
	int n;
	tree root;

	cin >> n;
	vector<treenode> Tree(10);
	root = buildtree(Tree, n);

	auto result = findleaves(Tree, root);   //接收返回的存储着叶子节点的队列，将队列遍历输出

	for (decltype(result.size()) i = 0; i != result.size(); i++) {
		cout << result[i];
		//注意：最后一个数字输出后没有空格
		i != result.size() - 1 ? cout << " " : cout << endl;
	}

	return 0;
}
*/
