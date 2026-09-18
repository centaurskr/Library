///
/// Linked list
/// @file linkedlst.c
/// @date 2024. 01. 02. (화) 12:10:30 KST
/// @author Cento 
///
#include <stdlib.h>
#include <string.h>

#ifndef _TB_LINKED_LIST_
#define _TB_LINKED_LIST_
///< LINKED List Node
typedef struct _LINK_NODE{
    int           key;     ///< node key(임의 설정, 중복허용 안함)
    int           len;     ///< node data 길이
    char         *data;    ///< node data pointer
    struct _LINK_NODE *next;    ///< Next link
}LINK_NODE;

///< 범용적인 LINKED LIST
typedef struct _TB_LINKED_LIST{
    int         count;  ///< Link count
    LINK_NODE  *start;  ///< 시작 NODE
    LINK_NODE  *end  ;  ///< 끝   NODE
}TB_LINKED_LIST;
#endif
////////////////////////////////////////////////////////////////////////////////
/// Data Move (TB_LINKED_LIST 내부 사용함수)
/// @brief  Move data
/// @fn      static void MoveData(void *dst, void *src, int len)
/// @param dst Destination
/// @param src Source
/// @param len Data 길이   
/// @return  없음
////////////////////////////////////////////////////////////////////////////////
static void MoveData(void *dst, void *src, int len)
{
unsigned char *d;
unsigned char *s;
unsigned int   i;
	d = (unsigned char *)dst;s = (unsigned char *)src;
	for(i= 0; i < len; i++, d++, s++)*d = *s;
}
////////////////////////////////////////////////////////////////////////////////
/// Linked list를 초기화 한다 
/// @fn        void *TbListInit()
/// @brief    linked list 초기화
/// @param    없음
/// @return   Linked list context( 사용후 DestoryLink(void *lk) 호출 필수)
////////////////////////////////////////////////////////////////////////////////
void *TbListInit()
{
TB_LINKED_LIST *link;
	link = (TB_LINKED_LIST *)calloc(1, sizeof(TB_LINKED_LIST));
	if(link == NULL) return NULL;
	link->start = NULL;
	link->end = NULL;
	link->count = 0;
	return (void *)link;
}
////////////////////////////////////////////////////////////////////////////////
/// Linked list에 노드를 추가한다 \n
/// data는 내부에서 len만큼 memory allocation한다
/// @fn int TbListInsert(void *lk, int key, void *data, int len)
/// @brief    node 추가
/// @param    lk     Linked list context
/// @param    key    Find, Search등을 위한 Key로 중복되지않는 숫자
/// @param    data   Linked list data
/// @param    len    Linked list data 길이
/// @return   0 실패
/// @return   1 성공
////////////////////////////////////////////////////////////////////////////////
int TbListInsert(void *lk, int key, void *data, int len)
{
TB_LINKED_LIST *link;
LINK_NODE *node;
	link = (TB_LINKED_LIST *)lk;
	node = (LINK_NODE *)calloc(1, sizeof(LINK_NODE));	
	if(node == NULL) return 0;
	node->data = (char *)calloc(1, len);
	if(node->data == NULL){ free((void *)node); return 0;}
	MoveData((void *)node->data, data, len);
	node->key = key;
	node->len = len;
	node->next = NULL;

	if(link->count == 0){
		link->start = node;
		link->count = 1;
		link->end = node;
	}else{
		link->end->next = (struct _LINK_NODE *)node;
		link->end = node;
		link->count ++;
	}
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
/// Key에 해당하는 Linked list node를 삭제한다
/// @fn     int TbListDelete(void *lk, int key)
/// @brief    Node 삭제
/// @param    lk    Linked list context
/// @param    key   node or data key
/// @return   0  Not found
/// @return   1  성공
////////////////////////////////////////////////////////////////////////////////
int TbListDelete(void *lk, int key)
{
TB_LINKED_LIST *link;
LINK_NODE *node, *b;
int   fg= 0;
	link = (TB_LINKED_LIST *)lk;
	if(link->count == 0) return 0;
	node = link->start; b = node;
	for(;node;){
		if(node->key == key){ fg = 1;break;}
		b= node; node = (LINK_NODE *)node->next;
	}
	if(fg == 0) return 0;

	if((b == node) && (node == link->start)){
		link->start = (LINK_NODE *)node->next;
	}else if(node == link->end){
		link->end = b;	
		link->end->next = NULL;
	}else{
		b->next = node->next;
	}
	link->count --;
	free((void *)node->data);
	free((void *)node);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
/// Linked List를 삭제한다(모든 Node Data도 free())
/// @fn       void TbListDestory(void *lk)
/// @brief    Linked list 삭제(파괴)
/// @param    lk Linked list context
/// @return   없음
////////////////////////////////////////////////////////////////////////////////
void TbListDestory(void *lk)
{
TB_LINKED_LIST *link;
LINK_NODE *node, *b;
int   i;
	link = (TB_LINKED_LIST *)lk;
	if(link->count == 0){free(lk); return ;}
	node = link->start;
	for(i = 0; i < link->count; i++){
		b = (LINK_NODE *)node->next;
		free((void *)node->data);
		free((void *)node);
		node = b;
	}
	free(lk);
	return ;
}
////////////////////////////////////////////////////////////////////////////////
/// Index로 Linked list data를 찿는다(index 시작 0 부터)
/// @fn      void *TbListFetchByIndex(void *lk, int index)
/// @brief    Fetch data by index
/// @param    lk     Linked list context
/// @param    index  Linked list index\nInsert한 순서\n0부터 시작
/// @return   NULL  Data 없음
/// @return   data  node data point
////////////////////////////////////////////////////////////////////////////////
void *TbListFetchByIndex(void *lk, int index)
{
TB_LINKED_LIST *link;
LINK_NODE *node;
int i;
	link = (TB_LINKED_LIST *)lk;
	if(link->count == 0 || link->count <= index) return NULL;
	node = link->start;
	for(i = 0; i < index; i++)node = (LINK_NODE *)node->next;
	return (void *)node->data;
}
////////////////////////////////////////////////////////////////////////////////
/// Key로 Linked list data를 찿는다
/// @fn      void *TbListFetchByKey(void *lk, int key)
/// @brief    Fetch data by key
/// @param    lk    Linked list context
/// @param    key   InsertNode() 당시 Key 값
/// @return   NULL  Data 없음
/// @return   data  node data point
////////////////////////////////////////////////////////////////////////////////
void *TbListFetchByKey(void *lk, int key)
{
TB_LINKED_LIST *link;
LINK_NODE *node;
int fg = 0;
	link = (TB_LINKED_LIST *)lk;
	if(link->count == 0) return NULL;
	node = link->start;
	for(;node;){
		if(node->key == key){fg = 1; break;}
		node=(LINK_NODE *)node->next;
	}
	if(fg == 0) return NULL;
	return (void *)node->data;
}
