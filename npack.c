 //
 // Description :
 // File Name   : npack.c
 // Date        : 2014. 02. 17. (월) 12:38:03 KST
 // By          :
 //
#include <stdio.h>
#include <stdlib.h>
typedef enum {
	DOT_VAL   = 0x0a,
	COMMA_VAL ,
	PLUS_VAL,
	MINUS_VAL,
	DUMMY_VAL = 0x0f
}SP_CHAR;
/******************************************************************************
 * Prototype : int DecimalPack(unsigned char *dest, unsigned char *src, int sz)
 * Arguments : unsigned char *dest; Output buffer
 *             unsigned char *src; input string point
 *             int            sz; input  size
 * Return    : size of 'dest'
 * Remarks   : 압축하고자 하는 String은 '0' ~ '9', ".", ",", "+", "-"이외는 
 *             포함할 수 없다.
 ******************************************************************************/
int DecimalPack(unsigned char *dest, unsigned char *src, int sz)
{
register unsigned char *s, *d;
register int size = 1, i;
	d = dest; s = src;
	for(i = 0;;){
		// 1st nible
		if(*s == '.')      *d = DOT_VAL;
		else if(*s == ',') *d = COMMA_VAL;
		else if(*s == '+') *d = PLUS_VAL;
		else if(*s == '-') *d = MINUS_VAL;
		else *d = *s & 0x0f;
		*d = *d << 4;
		i++;
		if(i >= sz){*d = *d | DUMMY_VAL; break;}
		s++;
		// 2nd nible
		if(*s == '.')      *d = *d | DOT_VAL;
		else if(*s == ',') *d = *d | COMMA_VAL;
		else if(*s == '+') *d = *d | PLUS_VAL;
		else if(*s == '-') *d = *d | MINUS_VAL;
		else *d = *d | (*s & 0x0f);
		i++;
		if(i >= sz)break;
		size ++; d++; s++;
	}
	return size;
}
/******************************************************************************
 * Prototype : int DecimalUnPack(unsigned char *dest,unsigned char *src, int sz)
 * Arguments : unsigned char *dest; Output buffer
 *             unsigned char *src; input string point(Packed string)
 *             int            sz; input  size
 * Return    : size of 'dest'
 * Remarks   : 
 ******************************************************************************/
int DecimalUnPack(unsigned char *dest, unsigned char *src, int sz)
{
register unsigned char *d, *s, tmp;
register unsigned int size=0, i;
	d = dest; s = src;
	for(i = 0; ;){
		// 1st nible
		tmp = *s >> 4;
		switch(tmp){
			case DOT_VAL   : *d = '.';break;
			case COMMA_VAL : *d = ',';break;
			case PLUS_VAL  : *d = '+';break;
			case MINUS_VAL : *d = '-';break;
			default :
				*d = 0x30 | tmp;
				break;
		}
		d++; size ++;

		//2nd nible
		tmp = *s & 0x0f;
		if(tmp == DUMMY_VAL) break;
		switch(tmp){
			case DOT_VAL   : *d = '.';break;
			case COMMA_VAL : *d = ',';break;
			case PLUS_VAL  : *d = '+';break;
			case MINUS_VAL : *d = '-';break;
			default :
				*d = 0x30 | tmp;
				break;
		}
		s++;i++;d++;size++;
		if( i >= sz) break;
	}
	return size;
}
