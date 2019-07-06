# 树的预备知识

分层次组织在管理上有更高的效率。

对于同一棵树而言，它的高度和深度是同一概念，但是对树上的结点来说，是不同的概念，因为同一节点的高度和深度可能不同。深度是从树根到这个结点的边的条数，高度是这个结点到下面叶节点的边条数的最大值。

数据管理的基本操作之一：查找

查找：根据某个给定的关键字k，从集合R中找出关键字与K相同的记录

- 静态查找：集合中的记录是固定的  
没有插入和删除操作

- 动态查找：集合中的记录是动态变化的  
除了查找，还可能发生插入和删除

##  静态查找

- 方法一：顺序查找

```c
/*在表Tbl[1]~Tbl[n]中查找关键字为K的数据元素*/
int SequentialSearch(StaticTable *Tbl, ElementType K) { 
	int i;
	Tbl->Element[0] = K; /*建立哨兵*/
	for (i = Tbl->Length; Tbl->Element[i] != K; i--)
		;
	return i; /*查找成功返回所在单元下标；不成功返回0*/
}
```

使用哨兵的好处是可以提高函数的性能，原因是不必在循环中判断数组是否越界。

顺序查找算法的时间复杂度为O(n)。

- 方法二：二分查找

```c
int BinarySearch(StaticTable * Tbl, ElementType K) { /*在表Tbl中查找关键字为K的数据元素*/
    int left, right, mid, NoFound = -1;
    left = 1;
    right = Tbl->Length;
    while (left <= right) {
        mid = (left + right) / 2; /*计算中间元素坐标*/
        if( K < Tbl->Element[mid])
    }
    return NotFound;
}
```

二分查找的时间复杂度为 O(logN)，如果查找的个数为偶数，(left + right) /2 向下取整即可。  







