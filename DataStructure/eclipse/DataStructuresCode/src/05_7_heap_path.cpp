/*
 * 05_7_heap_path.cpp
 *
 *  Created on: 2018年5月16日
 *      Author: SummerGift
 */

/*
05-树7 堆中的路径（25 分）
将一系列给定数字插入一个初始为空的小顶堆H[]。随后对任意给定的下标i，打印从H[i]到根结点的路径。

输入格式:
每组测试第1行包含2个正整数N和M(≤1000)，分别是插入元素的个数、以及需要打印的路径条数。
下一行给出区间[-10000, 10000]内的N个要被插入一个初始为空的小顶堆的整数。
最后一行给出M个下标。

输出格式:
对输入中给出的每个下标i，在一行中输出从H[i]到根结点的路径上的数据。数字间以1个空格分隔，行末不得有多余空格。

输入样例:
5 3
46 23 26 24 10
5 4 3

输出样例:
24 23 10
46 23 10
26 10
*/

#include <stdio.h>
#define MAX_NUM_HEAP 1001
#define MIN_VALUE -10001

int heap[MAX_NUM_HEAP], size;

void create_heap(int *heap, int *heap_size) {
    *heap_size = 0;
    heap[0] = MIN_VALUE;
}

void heap_insert(int x) {
    int i;
    for (i = ++size; heap[i / 2] > x; i /= 2)
        heap[i] = heap[i / 2];
    heap[i] = x;
}

int main() {
    int n, m, x, i, output_index;

    scanf("%d %d", &n, &m);

    create_heap(heap, &size);
    for (i = 0; i < n; i++) {
        scanf("%d", &x);
        heap_insert(x);
    }

    for (i = 0; i < m; i++) {
        scanf("%d", &output_index);
        printf("%d", heap[output_index]);
        while (output_index > 1) {
            output_index /= 2;
            printf(" %d", heap[output_index]);
        }
        printf("\n");
    }
    return 0;
}
