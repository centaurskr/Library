//
// Description :
// File Name   : strtrim.c
// Date        : 2017. 07. 14. (금) 13:57:42 KST
// By          : centaurskr@gmail.com
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*******************************************************************************
 * src의 앞/뒤 공백(및 NUL)을 제거하여 dst에 담는다(trim)
 * Prototype : int StrTrim(char *dst, char *src, int len)
 * Arguments : char *dst; [OUT] trim된 결과를 담을 buffer(NUL 종료)
 *             char *src; [IN]  원본 문자열
 *             int   len; [IN]  src에서 읽을 길이(공백 포함, NUL 불필요)
 * Return    : trim된 결과 문자열의 길이. len이 0이면 0을 반환하고 dst는 건드리지 않음.
 ******************************************************************************/
int StrTrim(char *dst, char *src, int len)
{
register int  dlen, i;
register char *dp, *sp;
char *buff;
	if(!len) return 0;
	buff = (char *)calloc(1, len + 1);
	strncpy(buff, src, len);
/* R-Trim */
	sp = buff + len -1; dlen = len -1;
	while(1){
		if(dlen <= 0) break;
		else if(*sp != ' ' && *sp != 0x00) break;
		else *sp = 0x00;
		sp --; dlen --;
	}
	dlen = strlen(buff); sp = buff; i = 0;
	/* L-Trim */
	while(i < dlen){ if(*sp != ' ' && *sp != 0x00) break; sp ++; i++; }
	dlen = strlen(sp);
	strcpy(dst, sp);
	free(buff);
	return dlen;
}

