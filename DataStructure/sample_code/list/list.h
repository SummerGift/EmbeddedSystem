/*
 * list.h
 *
 *  Created on: 2018年3月23日
 *      Author: Summer
 */

#ifndef LIST_H_
#define LIST_H_

typedef int ElementType;
typedef struct LNode *PtrToLNode;
struct LNode {
    ElementType Data;
    PtrToLNode Next;
};
typedef PtrToLNode Position;
typedef PtrToLNode List;

/* 查找 */
#define ERROR NULL

List MakeEmpty();
int IsEmpty( List L );
Position Find( List L, ElementType X );
bool Insert( List L, ElementType X, Position P );
bool Delete( List L, Position P );
int IsLast( Position P, List L );



#endif /* LIST_H_ */
