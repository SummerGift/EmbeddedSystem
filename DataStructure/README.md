# Readme

## About this repo

- This repo consists of my notes of datastructures classes taught in University of ZHEJIANG on MOOC.
- Most of the code that this branch contains will be written by C/C++.
- Record C/C++ data structure exercises.
- Summarize the learning content and put forward my own understanding.
___

单纯的学习数据结构本身是不够的，结合工作中实际工作项目来实践才能所学的知识融会贯通。因此我决定添加数据结构工作应用章节。在 RT-Thread 中用到了许多常用的数据结构，当我发现可以用通用双向链表来实现内核对象管理的时候，我在心底里感叹这种数据结构如此奇妙。通过阅读高质量的源码，可以提高学习效果，这是一种很棒的做法。看到一种数据结构的巧妙应用，回过头来再继续学习总结，我相信这个过程可以对提高自身的水平有很大帮助。

数据结构课程讲述的内容是比较全面的，除了基本的数据结构和算法的概念，算法复杂度的计算还有递归专题，抽象数据类型等。数据结构从基本的表、堆栈、队列、一直讲到较为复杂的树、图、散列表，另外讲述了一些常用的排序算法，并且在每一章之后都提供了足够多的练习题，为了写这些练习题真的是绞尽脑汁。

在我从事的嵌入式工作中，常用的数据结构有双向链表、动态数组、队列、堆、栈、散列表，常用的算法一般就是排序和查找。但我觉得像是树、图这样的数据结构还是要去学的，侯捷先生说过：“学从难处学，用从易处用”。在学习的过程中一定要精到不能再精为止。

数据结构和 C++ 的学习从建立工程开始提交练习代码到18年6月已经有半年时间了，说真的坚持真的不容易，即使中间有曲折，无论如何我已经下决心要把这件事做到底，把这项技能掌握好。无论需要学习几遍，不管到什么时候，这件事必须要有始有终，继续加油。

## Content

### 1. 学习总结
#### 第一章：基本概念
- [x] [Lecture 1.1 - 数据结构与算法](./Lectures/Lecture-1.1-数据结构与算法.md)
- [x] [Lecture 1.2 - 递归的思想与应用](./Lectures/Lecture-1.2-递归的思想与应用.md)
#### 第二章：表、栈和队列
- [x] [Lecture 2.1 - 抽象数据类型 ADT](./Lectures/Lecture-2.1-抽象数据类型ADT.md)
- [x] [Lecture 2.2 - 表 ADT](./Lectures/Lecture-2.2-表ADT.md)
- [x] [Lecture 2.3 - 堆栈 ADT](./Lectures/Lecture-2.3-堆栈ADT.md)
- [ ] [Lecture 2.4 - 队列 ADT](./Lectures/Lecture-2.4-队列ADT.md)
- [x] [Lecture 2.5 - 通用链表的实现](./Lectures/Lecture-2.5-通用链表的实现.md)
#### 第三章：树
- [x] [Lecture 3.1 - 树的预备知识](./Lectures/Lecture-3.1-树的预备知识.md)
- [x] [Lecture 3.2 - 二叉树](./Lectures/Lecture-3.2-二叉树.md)
- [x] [Lecture 3.3 - 查找树 ADT(二叉查找树)](./Lectures/Lecture-3.3-查找树ADT(二叉查找树).md)
- [x] [Lecture 3.4 - AVL树](./Lectures/Lecture-3.4-AVL树.md)
#### 第四章：优先队列（堆）
- [x] [Lecture 4.1 - 堆](./Lectures/Lecture-4.1-堆.md)
- [x] [Lecture 4.2 - 哈夫曼树与哈夫曼编码](./Lectures/Lecture-4.2-哈夫曼树与哈夫曼编码.md)
- [ ] [Lecture 4.3 - 不相交集 ADT](./Lectures/Lecture-4.3-不相交集ADT.md)
#### 第五章：图论算法
- [ ] [Lecture 5.1 - 图](./Lectures/Lecture-4.1-堆.md)
#### 第六章：排序
- [x] [Lecture 6.1 - 排序](./Lectures/Lecture-6.1-排序.md)

### 2. 数据结构练习题

#### 1. Complexity

* 01-1 [Maxsubsequencesum question (C)](./eclipse/DataStructuresCode/src/01_1_Maxsubsequencesum_question.cpp)
* 01-2 [Maximum Subsequence Sum (C)](./eclipse/DataStructuresCode/src/01_2_Maximum_Subsequence_Sum.cpp)
* 01-3 [BinarySearch (C)](./eclipse/DataStructuresCode/src/01_3_BinarySearch.cpp)
* 01-4 [RecursiveProject(C++)](./eclipse/DataStructuresCode/src/Project_01_recursive_function.cpp) 

#### 2. List

* 02-1 [List Merge (C)](./eclipse/DataStructuresCode/src/02_1_List_Merge.cpp)
* 02-2 [List mult add (C)](./eclipse/DataStructuresCode/src/02_2_list_mult_add.cpp)
* 02-3 [Reversing Linked List (C)](./eclipse/DataStructuresCode/src/02_3_Reversing_Linked_List.cpp)
* 02-4 [Pop Sequence (C++)](./eclipse/DataStructuresCode/src/02_4_Pop_Sequence.cpp)

#### 3. Trees

- 03-1 [Tree isomorphism (C)](./eclipse/DataStructuresCode/src/03_1_Tree_isomorphism.cpp)
- 03-2 [List Leaves (C++)](./eclipse/DataStructuresCode/src/03_2_List_Leaves.cpp)
- 03-3 [Tree Traversals Again (C)](./eclipse/DataStructuresCode/src/03_3_Tree_Traversals_Again.cpp)
- 04-4 [Is Same BinarySearch Tree (C)](./eclipse/DataStructuresCode/src/04_4_IsSameBinarySearchTree.cpp)
- 04-5 [Root of AVL_Tree (C)](./eclipse/DataStructuresCode/src/04_5_Root_of_AVL_Tree.cpp)
- 04-6 [Complete Binary Search Tree (C)](./eclipse/DataStructuresCode/src/04_6_Complete_Binary_Search_Tree.cpp)
- 04-7 [Binarysearch tree operation set (C)](./eclipse/DataStructuresCode/src/04_7_Binarysearch_tree_operation_set.cpp)
- 05-7 [Heap Path (C)](./eclipse/DataStructuresCode/src/05_7_heap_path.cpp)
- 05-8 [File Transfer (C)](./eclipse/DataStructuresCode/src/05_8_File_Transfer.cpp)
- 05-9 [Huffman Codes (C)](./eclipse/DataStructuresCode/src/05_9_Huffman_Codes.cpp)

### 3. 数据结构工作应用

本章介绍在 RT-Thread 操作系统中所使用的数据结构，研究如何使用这些常见的数据结构来实现复杂的系统功能。

- [x] [第一章：内核对象模型](./Lectures/rt-thread/1-内核对象模型.md)

## Note style

### Code

All code will be marked \`\`\`Language ...code... \`\`\` when the lecture is read in raw format. When viewed through Github and Pandoc, it will be color coded based on the language as such:

* C
    ```c
    #include <stdio.h>
    int main(void){
        char * foo = "bar";
        printf("%s",foo);
        return 0;
    }
    ```
  * All C code is Compiled with ```MinGW.org GCC-6.3.0-1```.
