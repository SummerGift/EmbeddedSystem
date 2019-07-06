/*
 * 05_8_File_Transfer.cpp
 *
 *  Created on: 2018年5月21日
 *      Author: Administrator
 */

/*
05-树8 File Transfer（25 分）
We have a network of computers and a list of bi-directional connections.
Each of these connections allows a file transfer from one computer to another.
Is it possible to send a file from any computer on the network to any other?

Input Specification:

Each input file contains one test case. For each test case, the first line contains N (2≤N≤10^4),
 the total number of computers in a network.
Each computer in the network is then represented by a positive integer between 1 and N.
Then in the following lines, the input is given in the format:

I c1 c2
where I stands for inputting a connection between c1 and c2; or

C c1 c2
where C stands for checking if it is possible to transfer files between c1 and c2; or

S

where S stands for stopping this case.

Output Specification:

For each C case, print in one line the word "yes" or "no" if it is possible or impossible to transfer files between c1 and c2,
respectively. At the end of each case, print in one line "The network is connected."
if there is a path between any pair of computers; or "There are k components."
where k is the number of connected components in this network.

Sample Input 1:

5
C 3 2
I 3 2
C 1 5
I 4 5
I 2 4
C 3 5
S
Sample Output 1:

no
no
yes
There are 2 components.
Sample Input 2:

5
C 3 2
I 3 2
C 1 5
I 4 5
I 2 4
C 3 5
I 1 3
C 1 5
S
Sample Output 2:

no
no
yes
yes
The network is connected.
*/
#include<stdio.h>
#include<stdlib.h>

typedef int element_type;
typedef int root_index;
typedef int* root_type;

//在本题中，关于集合的表示方法可以被简化，可以将N个元素映射到 0 - (N-1) 下标的数组中，
//这样就不用像一般表示法那样遍历整个数组来寻找元素所在的位置
//查找元素x所在位置的值，如果不是复数，那么就查找这个值对应位置的值，直到找到根结点所在的位置
root_index find(root_type S, element_type X) {
    for (; S[X] >= 0; X = S[X])
        ;
    return X;
}

void union_root(root_type S, root_index root1, root_index root2) {
    S[root1] = root2;
}

void senior_union_root(root_type S, root_index root1, root_index root2) {
    if (S[root2] < S[root1]) // root2 更深
        S[root1] = root2;    // 将 root2 作为合并后的新根
    else {
        if (S[root2] == S[root1])
            S[root1]--;      // 两棵树深度相同，选择一颗深度+1
        S[root2] = root1;    // 合并
    }
}

void initialization(root_type S, int n) {
    for (int i = 0; i < n; i++)
        S[i] = -1;
}

void input_connection(root_type S) {
    element_type u, v;
    root_index root1, root2;
    scanf("%d %d\n", &u, &v);
    root1 = find(S, u - 1);
    root2 = find(S, v - 1);
    if (root1 != root2)
        senior_union_root(S, root1, root2);
}

void check_connection(root_type S) {
    element_type u, v;
    root_index root1, root2;
    scanf("%d %d\n", &u, &v);
    root1 = find(S, u - 1);    //这里-1 是因为第u个元素在数组中的位置为u - 1
    root2 = find(S, v - 1);
    if (root1 == root2)
        printf("yes\n");
    else
        printf("no\n");
}

void check_network(root_type S, int n) {
    int i, counter = 0;
    for (i = 0; i < n; i++) {
        if (S[i] < 0)
            counter++;
    }

    if (counter == 1)
        printf("The network is connected.\n");
    else
        printf("There are %d components.\n", counter);
}

int main() {
    int *S;
    int n;
    char input_char;
    scanf("%d\n", &n);
    S = (int*) malloc(sizeof(int) * n);
    initialization(S, n);
    do {
        scanf("%c", &input_char);
        switch (input_char) {
        case 'I':
            input_connection(S);
            break;
        case 'C':
            check_connection(S);
            break;
        case 'S':
            check_network(S, n);
            break;
        }
    } while (input_char != 'S');
    free(S);
    return 0;
}
