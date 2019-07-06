#ifndef DLIST_H
#define DLIST_H

#ifdef __cplusplus
extern "C" {
#endif/*__cplusplus*/

typedef enum _DListRet
{
	DLIST_RET_OK,
	DLIST_RET_OOM,
	DLIST_RET_STOP,
	DLIST_RET_PARAMS,
	DLIST_RET_FAIL
}DListRet;

struct _DList;
typedef struct _DList DList;

typedef DListRet (*DListDataPrintFunc)(void* data);

DList* dlist_create(void);

DListRet dlist_insert(DList* thiz, size_t index, void* data);
DListRet dlist_prepend(DList* thiz, void* data);
DListRet dlist_append(DList* thiz, void* data);
DListRet dlist_delete(DList* thiz, size_t index);
DListRet dlist_get_by_index(DList* thiz, size_t index, void** data);
DListRet dlist_set_by_index(DList* thiz, size_t index, void* data);
size_t   dlist_length(DList* thiz);
DListRet dlist_print(DList* thiz, DListDataPrintFunc print);

void dlist_destroy(DList* thiz);

#ifdef __cplusplus
}
#endif/*__cplusplus*/

#endif/*DLIST*/

