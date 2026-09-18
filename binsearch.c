///<
///< Description : Search mode가 적용된 bsearch
///< File Name   : binsearch.c
///< Date        : 2022. 06. 21. (화) 09:11:33 KST
///< By  : Cento
///<
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef enum{
    BS_LT = 1, // Less then
    BS_LE,     // Less then equal
    BS_EQ,     // Equal
    BS_GE,     // Great then equal
    BS_GT      // Great then
}SEARCH_MODE;
////////////////////////////////////////////////////////////////////////////////
///< mode를 지원하는 bsearch
///< Prototype : void* BinSearch(void *key, int mode, void *base, 
///<             int cnt, int size, int (*compare)())
///< Arguments : void *key : search key
///<             int mode  : BS_LT ~ BS_GT
///<             void *base : 검색 시작점
///<             int   cnt  : Data 총개수
///<             int   size : Data size
///<             int  (*compare)() : 비교함수(key, element) : key<elem 이면 음수,
///<                                 key==elem 이면 0, key>elem 이면 양수 반환
///< Return    : mode에 따라 조건을 만족하는 첫 element의 포인터.
///<             BS_EQ에서 일치하는 원소가 없으면 NULL.
///<             BS_LT/BS_GT 등에서 범위를 벗어나면 NULL.
////////////////////////////////////////////////////////////////////////////////
void *BinSearch(const void *key, int mode,
    void *base, int cnt, int size, int (*compare)())
{
int rtn, sidx, eidx, midx;
void *s;
void *e;
void *m;
    s = base;  e = base + size * (cnt - 1);

	// 처음값비교
	rtn = compare(key, s);
	if(rtn < 0){
		if(mode == BS_LT || mode == BS_LE || mode == BS_EQ) return NULL;
		else if(mode == BS_GT || mode == BS_GE) return s;
	}else if(rtn == 0){
		if(mode == BS_LT) return NULL;
		else if(mode == BS_LE || mode == BS_GE || mode == BS_EQ) return s;
		else if(mode == BS_GT) return (void *)(s + size);
	}

	// 마지막값 비교
	rtn = compare(key, e);
	if(rtn > 0){
		if(mode == BS_GT || mode == BS_GE || mode == BS_EQ) return NULL;
		else if(mode == BS_LT || mode == BS_LE) return e;
	}else if(rtn == 0){
		if(mode == BS_GT) return NULL;
		else if(mode == BS_LE || mode == BS_GE || mode == BS_EQ) return e;
		else if(mode == BS_LT) return (void *)(e - size);
	}

	sidx =0; eidx = cnt -1; midx = (eidx - sidx)/ 2;
    m = base + size * midx;
    while(1){
		if(midx == sidx || midx == eidx) break;
		m = base + midx * size;
        rtn = compare(key, m);
        if(rtn == 0) break;
        else if(rtn < 0) eidx = midx;
        else if(rtn > 0) sidx = midx;
		midx = sidx + (eidx - sidx)/2;
    }
    if(rtn == 0){
        if(mode == BS_LE || mode == BS_GE || mode == BS_EQ) return m;
        else if(mode == BS_GT) return m + size; // GT
        else if(mode == BS_LT) return m - size; // LT
    }
    switch(mode){
    case BS_LT:
    case BS_LE:
        return base + sidx * size;
    case BS_EQ: return NULL;

    case BS_GT:
    case BS_GE:
        return base + eidx * size;
    }
    return NULL;
}
