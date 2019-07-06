/*
 * Pop_Sequence.cpp
 *
 *  Created on: 2018年4月6日
 *      Author: Administrator
 */

/*
02-线性结构4 Pop Sequence（25 分）

Given a stack which can keep M numbers at most. Push N numbers in the order of 1, 2, 3, ..., N and pop randomly.
You are supposed to tell if a given sequence of numbers is a possible pop sequence of the stack.
For example, if M is 5 and N is 7, we can obtain 1, 2, 3, 4, 5, 6, 7 from the stack, but not 3, 2, 1, 7, 5, 6, 4.

Input Specification:

Each input file contains one test case.
For each case, the first line contains 3 numbers (all no more than 1000):
M (the maximum capacity of the stack), N (the length of push sequence), and K (the number of pop sequences to be checked).
Then K lines follow, each contains a pop sequence of N numbers. All the numbers in a line are separated by a space.

Output Specification:

For each pop sequence, print in one line "YES" if it is indeed a possible pop sequence of the stack, or "NO" if not.

Sample Input:

5 7 5

1 2 3 4 5 6 7

3 2 1 7 5 6 4

7 6 5 4 3 2 1

5 6 4 3 7 2 1

1 7 6 5 4 3 2

Sample Output:

YES

NO

NO

YES

NO

*/


#include<cstdio>
#include<stack>
using namespace std;

//假设 1-(i-1) 都进入了栈，i 还没有进入栈。
//一个一个元素考虑出栈序列的元素x：
//1. i>x
//    这时X已经入过栈了。
//1.1 如果他不是栈顶那么就无法pop出x了
//1.2 如果x是栈顶，直接从堆栈pop就可以了，处理下一个出栈元素
//
//2. i<=x
//2.1 这时候X还没有入栈，要想要让X出栈，我们必须把从i到x都push到栈，这时候x在栈顶，Pop即可。
//注意：压入元素要保证栈里面的元素个数不超过给定的栈容量，否则也不可能将x弹出。

int pop_seqence_test() {
    stack<int> st;
    int stack_cap, test_seq_len, k;
    scanf("%d%d%d", &stack_cap, &test_seq_len, &k);    //依次输入栈的容量，输入测试序列的长度，测试序列的组数，
    while (k--) {                                   //对每一组测试序列分别进行测试
        int t = test_seq_len;
        int num = 1;                                //每次push到栈中的数字大小的记录
        bool flag = true;
        while (t--) {                               //对测试序列的每一个输入数据进行测试
            int x;
            scanf("%d", &x);                        //读入一个测试数据x
            while (num <= x)                        //x比当前栈顶下标要大
                st.push(num++);                     //一直push元素直到入栈的数字大小和x相等
            int last = st.top();                    //查看栈顶元素
            if (last != x || st.size() > stack_cap)    //如果栈顶元素不等于x,或者栈的大小已经超过栈容量则失败
                flag = false;
            st.pop();                               //栈顶元素和x相等，且不超容量则弹栈
        }
        if (flag)
            printf("YES\n");
        else
            printf("NO\n");
    }
    return 0;
}

