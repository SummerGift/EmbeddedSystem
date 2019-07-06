/*
 * list_mult_add.cpp
 *
 *  Created on: 2018年4月1日
 *      Author: Summer
 */

/*
02-线性结构2 一元多项式的乘法与加法运算

设计函数分别求两个一元多项式的乘积与和。

输入格式:

输入分2行，每行分别先给出多项式非零项的个数，再以指数递降方式输入一个多项式非零项系数和指数（绝对值均为不超过1000的整数）。
数字间以空格分隔。

输出格式:

输出分2行，分别以指数递降方式输出乘积多项式以及和多项式非零项的系数和指数。数字间以空格分隔，但结尾不能有多余空格。
零多项式应输出0 0。

输入样例:

4 3 4 -5 2  6 1  -2 0
3 5 20  -7 4  3 1
输出样例:

15 24 -25 22 30 21 -10 20 -21 8 35 6 -33 5 14 4 -15 3 18 2 -6 1
5 20 -4 4 -5 2 9 1 -2 0
*/

#include<stdio.h>
#include<stdlib.h>

struct PolyNode {
	int coef;                   // 系数
	int expon;                  // 指数
	struct PolyNode *link;      // 指向下一个节点的指针
};
typedef struct PolyNode *Polynomial;
Polynomial P1, P2;

// 系数 指数 需要将节点插入链表的尾部指针
void Attach(int c, int e, Polynomial *pRear) {
	Polynomial P;

	P = (Polynomial) malloc(sizeof(struct PolyNode));
	P->coef = c;
	P->expon = e;
	P->link = NULL;
	(*pRear)->link = P; //让尾节点的指针指向新的节点
	*pRear = P;         //更新尾指针指向的位置
}

Polynomial ReadPoly() {
	int c, e, N;
	Polynomial P,Rear,t;

	scanf("%d", &N);
    P = (Polynomial)malloc(sizeof(struct PolyNode));
    P->link = NULL;
    Rear = P;

	while (N--) {
		scanf("%d %d", &c, &e);
		Attach(c, e, &Rear);
	}

	t = P;
	P = P->link;
	free(t);

	return P;
}

Polynomial Add(Polynomial P1, Polynomial P2) {
	Polynomial Rear, t1, t2, P;
	t1 = P1;
	t2 = P2;
	P = (Polynomial) malloc(sizeof(struct PolyNode));
	P->link = NULL;
	Rear = P;

	while (t1 && t2) {
		//如果指数相同，那么将系数相加，然后插入到P上
		if (t1->expon == t2->expon) {
			//如果系数和不为0
			if(t1->coef + t2->coef)
			{
				Attach((t1->coef + t2->coef), t1->expon, &Rear);
				t1 = t1->link;
				t2 = t2->link;
			}else{
			//系数和为0
				t1 = t1->link;
				t2 = t2->link;
			}
		} else if (t1->expon > t2->expon) {
			//如果t1的指数大于t2的指数，那么将t1插入P,并t1指向下一个元素
			Attach(t1->coef, t1->expon, &Rear);
			t1 = t1->link;

		} else {
			//t2的指数大于t1，那么将t2插入P,并t2指向下一个元素
			Attach(t2->coef, t2->expon, &Rear);
			t2 = t2->link;
		}
	}

	//t1不为空，将t1后面的元素依次插入到P中
	while (t1) {
		Attach(t1->coef, t1->expon, &Rear);
		t1 = t1->link;
	}

	//t2不为空，将t2后面的元素依次插入到P中
	while (t2) {
		Attach(t2->coef, t2->expon, &Rear);
		t2 = t2->link;
	}

	//函数返回
	t2 = P;
	P = P->link;
	free(t2);

	return P;
}

Polynomial Mult(Polynomial P1, Polynomial P2)
{
	Polynomial Rear, t1, t2, P, t;
	t1 = P1;
	t2 = P2;
	int c, e;

	if(!P1 || !P2) return NULL;

	P = (Polynomial) malloc(sizeof(struct PolyNode));
	P->link = NULL;
	Rear = P;

	while(t2)   //用t1的第一项乘以P2，得到第一个链P
	{
		Attach(t1->coef * t2->coef,t1->expon + t2->expon, &Rear);
		t2 = t2->link;
	}

	t1 = t1->link;

	while(t1)   //再用两个循环将后续的项加入到链表中
	{
		t2 = P2;
		Rear = P;
		while(t2){
			e = t1->expon + t2->expon;
			c = t1->coef * t2->coef;

		    //查找到系数不比e大的位置后停下来
			while(Rear->link && (Rear->link->expon > e))
				Rear = Rear->link;

			//如果Rear当前节点的下一个参数的指数项和需要插入的项相同，那么需要合并
			if(Rear->link && (Rear->link->expon == e))
			{
				if((Rear->link->coef + c) != 0) //如果和不为0，那么更新系数
					Rear->link->coef += c;
				else{
					t = Rear->link;
					Rear->link = t->link;
					free(t);
				}
			}else{
			//不相同则生成一个新节点直接插入
				t = (Polynomial) malloc(sizeof(struct PolyNode));
				t->coef = c;
				t->expon = e;

				t->link = Rear->link;  //插入新的节点
				Rear->link = t;
				Rear = Rear->link;
			}
			t2 = t2->link;
		}
		t1 = t1->link;
	}

	//函数返回
	t2 = P;
	P = P->link;
	free(t2);

	return P;
}

void PrintPoly(Polynomial P) {
	int flag = 0;

	if (!P) {
		printf("0 0\n");
		return;
	}

	while (P) {
		if (!flag) {
			flag = 1;
		} else {
			printf(" ");
		}

		printf("%d %d", P->coef, P->expon);
		P = P->link;
	}

	printf("\n");
}

//int main() {
//	Polynomial P1, P2, PP, PS;
//	P1 = ReadPoly();
//	P2 = ReadPoly();
//	PP = Mult(P1, P2);
//	PrintPoly(PP);
//	PS = Add(P1, P2);
//	PrintPoly(PS);
//	return 0;
//}
