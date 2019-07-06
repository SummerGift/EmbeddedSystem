/*
 * 04_6_Complete_Binary_Search_Tree.cpp
 *
 *  Created on: 2018年5月14日
 *      Author: SummerGift
 */

/*
 *
04-树6 Complete Binary Search Tree（30 分）
A Binary Search Tree (BST) is recursively defined as a binary tree which has the following properties:

The left subtree of a node contains only nodes with keys less than the node's key.
The right subtree of a node contains only nodes with keys greater than or equal to the node's key.
Both the left and right subtrees must also be binary search trees.
A Complete Binary Tree (CBT) is a tree that is completely filled,
with the possible exception of the bottom level, which is filled from left to right.

Now given a sequence of distinct non-negative integer keys,
a unique BST can be constructed if it is required that the tree must also be a CBT.
You are supposed to output the level order traversal sequence of this BST.

Input Specification:
Each input file contains one test case. For each case,
the first line contains a positive integer N (≤1000).
Then N distinct non-negative integer keys are given in the next line.
All the numbers in a line are separated by a space and are no greater than 2000.

Output Specification:
For each test case, print in one line the level order traversal sequence of the corresponding complete binary search tree.
All the numbers in a line must be separated by a space, and there must be no extra space at the end of the line.

Sample Input:
10
1 2 3 4 5 6 7 8 9 0
Sample Output:
6 3 8 1 5 7 9 0 2 4

*/

#include <stdio.h>
#include <stdlib.h>

int tree_array[1005];
int j = 0;

int compare(const void *a, const void *b);
void mid_tree(int root, int n, int a[]);

int compare(const void *a, const void *b) {
    return *(int *) a - *(int *) b;
}

//递归实现查找根节点，并且将根节点依次存放到 tree_array 数组中
void mid_tree(int root, int N, int a[]) {
    if (root <= N) {
        mid_tree(2 * root, N, a);
        tree_array[root] = a[j++];
        mid_tree(2 * root + 1, N, a);
    }
}

/*
int main() {
    int N;
    scanf("%d", &N);
    int input_array[N];

    for (int i = 0; i <= N; i++) {
        scanf("%d", &input_array[i]);
    }

    qsort(input_array, N, sizeof(int), compare);

    mid_tree(1, N, input_array);

    printf("%d", tree_array[1]);

    for (int j = 2; j <= N; j++) {
        printf(" %d", tree_array[j]);
    }
}
*/





























