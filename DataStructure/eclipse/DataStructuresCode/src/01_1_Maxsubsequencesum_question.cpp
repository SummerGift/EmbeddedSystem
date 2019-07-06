/*
 * Maxsubsequencesum_1.cpp
 *
 *  Created on: 2018年4月7日
 *      Author: Summer
 */

/*
01-复杂度1 最大子列和问题（20 分）

给定K个整数组成的序列{ N1, N2, ..., NK }，“连续子列”被定义为{ Ni, Ni+1, ..., Nj }，
其中 1 <= i <= j <= K。“最大子列和”则被定义为所有连续子列元素的和中最大者。
例如给定序列{ -2, 11, -4, 13, -5, -2 }，其连续子列{ 11, -4, 13 }有最大的和20。
现要求你编写程序，计算给定整数序列的最大子列和。

本题旨在测试各种不同的算法在各种数据情况下的表现。各组测试数据特点如下：

数据1：与样例等价，测试基本正确性；
数据2：102个随机整数；
数据3：103个随机整数；
数据4：104个随机整数；
数据5：105个随机整数；
输入格式:

输入第1行给出正整数K (≤100000)；第2行给出K个整数，其间以空格分隔。

输出格式:

在一行中输出最大子列和。如果序列中所有整数皆为负数，则输出0。

输入样例:

6
-2 11 -4 13 -5 -2
输出样例:

20

 */

#include <stdio.h>

static int MaxSubseqSum1( int A[], int N )
{   int ThisSum, MaxSum;
    int i;
    ThisSum = MaxSum = 0;
    for( i = 0; i < N; i++ ) {
          ThisSum += A[i]; /* 向右累加 */
          if( ThisSum > MaxSum )
                  MaxSum = ThisSum; /* 发现更大和则更新当前结果 */
          else if( ThisSum < 0 ) /* 如果当前子列和为负 */
                  ThisSum = 0; /* 则不可能使后面的部分和增大，抛弃之 */
    }
    return MaxSum;
}

//int main()
//{
//    int i, *a;
//    int arraycount;
//    int result;
//    scanf("%d",&arraycount);
//
//    a = malloc(sizeof(int) * arraycount); /*分配内存*/
//
//    for(i = 0; i < arraycount; i++)
//        scanf("%d", a + i);
//
//    result = MaxSubseqSum1(a, arraycount);
//    printf("%d",result);
//
//  return 0;
//}



