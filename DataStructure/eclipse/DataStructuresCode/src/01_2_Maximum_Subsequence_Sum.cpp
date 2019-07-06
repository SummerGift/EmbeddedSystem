/*
 * Maxsubsequencesum.cpp
 *
 *  Created on: 2018年3月8日
 *      Author: Administrator
 */

/*
01-复杂度2 Maximum Subsequence Sum（25 分）

Given a sequence of K integers { N1, N2, ..., NK }.
A continuous subsequence is defined to be { Ni, Ni+1, ..., Nj } where 1 <= i <= j <= K.
The Maximum Subsequence is the continuous subsequence which has the largest sum of its elements.
For example, given sequence { -2, 11, -4, 13, -5, -2 }, its maximum subsequence is { 11, -4, 13 } with the largest sum being 20.
Now you are supposed to find the largest sum, together with the first and the last numbers of the maximum subsequence.
Input Specification:
Each input file contains one test case. Each case occupies two lines.
The first line contains a positive integer K (<= 10000). The second line contains K numbers, separated by a space.
Output Specification:
For each test case, output in one line the largest sum,
together with the first and the last numbers of the maximum subsequence.
The numbers must be separated by one space, but there must be no extra space at the end of a line.
In case that the maximum subsequence is not unique, output the one with the smallest indices i and j (as shown by the sample case).
If all the K numbers are negative, then its maximum sum is defined to be 0,
and you are supposed to output the first and the last numbers of the whole sequence.

Sample Input:
10 -10 1 2 3 4 -5 -23 3 7 -21
Sample Output:
10 1 4
 */

#include <stdio.h>



static int MaxSubseqSum2( int A[], int N )
{   int ThisSum, MaxSum;
    int i,array_end;
    ThisSum = MaxSum = 0;
    array_end = 0;
    int array_begin = 0;
    int allnative = 1;
    int zero_count = 1;
    int save_array_begin = 0;

    for( i = 0; i < N; i++ )
    {
        ThisSum += A[i]; /* 向右累加 */

        if( A[i] >= 0 )
            allnative = 0;

        if( ThisSum > MaxSum )
        {
            MaxSum = ThisSum; /* 发现更大和则更新当前结果 */
            zero_count++;
            array_end = i;
            save_array_begin = array_begin;
        }else
        if( ThisSum < 0 ) /* 如果当前子列和为负 */
        {
            ThisSum = 0 ; /* 则不可能使后面的部分和增大，抛弃之 */
            array_begin = i + 1;
        }
    }

    if ( allnative )
    {
        printf("0 %d %d",A[0], A[ N - 1 ]);
        return 0;
    }

    if( MaxSum == 0 )
    {
        printf("0 0 0");
    }else
    {
        printf("%d %d %d",MaxSum,A[save_array_begin],A[array_end]);
    }

    return 0;
}


//int main()
//{
//    int i, *a;
//    int arraycount;
//    scanf("%d",&arraycount);
//
//    a = (int *)malloc(sizeof(int) * arraycount); /*分配内存*/
//
//    for(i = 0; i < arraycount; i++)
//        scanf("%d", a + i);
//
//    MaxSubseqSum2(a, arraycount);
//    free(a);
//    return 0;
//}
