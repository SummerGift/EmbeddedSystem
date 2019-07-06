/*
 * File:    dlist.c
 * Author:  Li XianJing <xianjimli@hotmail.com>
 * Brief:   double list implementation.
 *
 * Copyright (c) Li XianJing
 *
 * Licensed under the Academic Free License version 2.1
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

/*
 * History:
 * ================================================================
 * 2008-11-09 Li XianJing <xianjimli@hotmail.com> created
 *
 */
#include <stdlib.h>
#include "dlist.h"

typedef struct _DListNode
{
	struct _DListNode* prev;
	struct _DListNode* next;

	void* data;                     //通用链表在存储数据的时候只保存数据的指针，这样不用复制数据效率高
}DListNode;

struct _DList
{
	DListNode* first;
};

static DListNode* dlist_node_create(void* data)
{
	DListNode* node = malloc(sizeof(DListNode));

	if(node != NULL)
	{
		node->prev = NULL;
		node->next = NULL;
		node->data = data;
	}

	return node;
}

static void dlist_node_destroy(DListNode* node)
{
	if(node != NULL)
	{
		node->next = NULL;
		node->prev = NULL;
		free(node);
	}

	return;
}

DList* dlist_create(void)
{
	DList* thiz = malloc(sizeof(DList));

	if(thiz != NULL)
	{
		thiz->first = NULL;
	}

	return thiz;
}

static DListNode* dlist_get_node(DList* thiz, size_t index, int fail_return_last)
{
	DListNode* iter = thiz->first;

	while(iter != NULL && iter->next != NULL && index > 0)
	{
		iter = iter->next;
		index--;
	}

	if(!fail_return_last)
	{
		iter = index > 0 ? NULL : iter;
	}

	return iter;
}

DListRet dlist_insert(DList* thiz, size_t index, void* data)
{
	DListNode* node = NULL;
	DListNode* cursor = NULL;

	if((node = dlist_node_create(data)) == NULL)
	{
		return DLIST_RET_OOM; 
	}

	if(thiz->first == NULL)
	{
		thiz->first = node;

		return DLIST_RET_OK;
	}

	cursor = dlist_get_node(thiz, index, 1);
	
	if(index < dlist_length(thiz))
	{
		if(thiz->first == cursor)
		{
			thiz->first = node;
		}
		else
		{
			cursor->prev->next = node;
			node->prev = cursor->prev;
		}
		node->next = cursor;
		cursor->prev = node;
	}
	else
	{
		cursor->next = node;
		node->prev = cursor;
	}

	return DLIST_RET_OK;
}

DListRet dlist_prepend(DList* thiz, void* data)
{
	return dlist_insert(thiz, 0, data);
}

DListRet dlist_append(DList* thiz, void* data)
{
	return dlist_insert(thiz, -1, data);
}

DListRet dlist_delete(DList* thiz, size_t index)
{
	DListNode* cursor = dlist_get_node(thiz, index, 0);

	if(cursor != NULL)
	{
		if(cursor == thiz->first)
		{
			thiz->first = cursor->next;
		}

		if(cursor->next != NULL)
		{
			cursor->next->prev = cursor->prev;
		}

		if(cursor->prev != NULL)
		{
			cursor->prev->next = cursor->next;
		}

		dlist_node_destroy(cursor);
	}

	return DLIST_RET_OK;
}

DListRet dlist_get_by_index(DList* thiz, size_t index, void** data)
{
	DListNode* cursor = dlist_get_node(thiz, index, 0);

	if(cursor != NULL)
	{
		*data = cursor->data;
	}

	return cursor != NULL ? DLIST_RET_OK : DLIST_RET_FAIL;
}

DListRet dlist_set_by_index(DList* thiz, size_t index, void* data) {
    DListNode* cursor = dlist_get_node(thiz, index, 0);

    if (cursor != NULL) {
        cursor->data = data;
    }

    return cursor != NULL ? DLIST_RET_OK : DLIST_RET_FAIL;
}

size_t dlist_length(DList* thiz) {
    size_t length = 0;
    DListNode* iter = thiz->first;

    while (iter != NULL) {
        length++;
        iter = iter->next;
    }

    return length;
}

DListRet dlist_print(DList* thiz, DListDataPrintFunc print)
{
	DListRet ret = DLIST_RET_OK;
	DListNode* iter = thiz->first;

	while(iter != NULL)
	{
		print(iter->data);

		iter = iter->next;
	}

	return ret;
}

void dlist_destroy(DList* thiz)
{
	DListNode* iter = thiz->first;
	DListNode* next = NULL;

	while(iter != NULL)
	{
		next = iter->next;
		dlist_node_destroy(iter);
		iter = next;
	}

	thiz->first = NULL;
	free(thiz);

	return;
}

#include <stdio.h>
#include <assert.h>
#include "dlist.h"

static DListRet print_int(void* data)
{
    printf("%d ", (int)data);

    return DLIST_RET_OK;
}

int main(int argc, char* argv[])
{
    int i = 0;
    int n = 100;
    DList* dlist = dlist_create();

    for(i = 0; i < n; i++)
    {
        assert(dlist_append(dlist, (void*)i) == DLIST_RET_OK);
    }
    for(i = 0; i < n; i++)
    {
        assert(dlist_prepend(dlist, (void*)i) == DLIST_RET_OK);
    }

    dlist_print(dlist, print_int);

    dlist_destroy(dlist);

    return 0;
}
