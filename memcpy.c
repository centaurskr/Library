//
//  Description :
//  File Name   : memcpy.c
//  Date        : 2017. 07. 18. (화) 10:44:23 KST
//  By          : centaurskr@gmail.com
// 

#include <stdio.h>
#include <stdlib.h>


////////////////////////////////////////////////////////////////////////////////
// Description : src에서 dest로 len 바이트를 1바이트씩 복사한다(memcpy 대체 구현)
// Prototype   : void MemCopy(char *dest, char *src, int len)
// Arguments   : char *dest : 목적지 버퍼(len 바이트 이상 확보되어 있어야 함)
//               char *src  : 원본 버퍼
//               int   len  : 복사할 바이트 수
// Return      : 없음 (dest가 수정됨)
// NOTE        : `if(i <= 0) return;` 이 `i`를 초기화하기 전에 읽는다
//               (undefined behavior). 실질적으로는 항상 for문까지 진행되어
//               len 바이트를 복사하는 것으로 관찰되지만, 이 사전 체크는
//               len<=0 가드로 동작하지 않는다.
//////////////////////////////////////////////////////////////////////////////
void MemCopy(char *dest, char *src, int len)
{
register unsigned char *d;
register unsigned char *s;
register int            i;
	if(i <= 0) return ;
	d = (unsigned char *)dest;s = (unsigned char *)src;
	for(i = 0; i < len; s++, d++, i++)*d = *s;
	return;
}
