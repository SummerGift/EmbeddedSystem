/*
 * 04_7_Binarysearch_tree_operation_set.cpp
 *
 *  Created on: 2018年5月1日
 *      Author: SummerGift
 */
#include <stdio.h>
#include <stdlib.h>

typedef int ElementType;

typedef struct TNode *Position;
typedef Position BinTree;
struct TNode{
    ElementType Data;
    BinTree Left;
    BinTree Right;
};

BinTree Insert( BinTree BST, ElementType X );
BinTree Delete( BinTree BST, ElementType X );
Position Find( BinTree BST, ElementType X );
Position FindMin( BinTree BST );
Position FindMax( BinTree BST );

Position FindMin(BinTree BST) {
	if (!BST)
		return NULL;
	else if (!BST->Left)
		return BST;
	else
		return FindMin(BST->Left);
}

Position FindMax(BinTree BST) {
	if (BST) {
		while (BST->Right)
			BST = BST->Right;
	}
	return BST;
}

BinTree Insert(BinTree BST, ElementType X) {
	if (!BST) {
		BST = (BinTree) malloc(sizeof(struct TNode));
		BST->Data = X;
		BST->Left = BST->Right = NULL;
	} else {
		if (X < BST->Data)
			BST->Left = Insert(BST->Left, X);
		else if (X > BST->Data)
			BST->Right = Insert(BST->Right, X);
	}
	return BST;
}

BinTree Delete(BinTree BST, ElementType X) {
	Position tmp;

	if (!BST) {
		printf("Not Found\n");
	} else {
		if (X < BST->Data)
			BST->Left = Delete(BST->Left, X);
		else if (X > BST->Data)
			BST->Right = Delete(BST->Right, X);
		else {
			if (BST->Left && BST->Right) {
				tmp = FindMin(BST->Right);
				BST->Data = tmp->Data;
				BST->Right = Delete(BST->Right, BST->Data);
			} else {
				tmp = BST;                //处理基本情况，也就是只有一个子节点或者没有子节点的情况
				if (!BST->Left) {
					BST = BST->Right;
				} else {
					BST = BST->Left;
				}
				free(tmp);
			}
		}
	}
	return BST;
}

Position Find(BinTree BST, ElementType X) {
	while (BST) {
		if (X > BST->Data)
			BST = BST->Right;
		else if (X < BST->Data)
			BST = BST->Left;
		else
			return BST;
	}
	return NULL;
}


