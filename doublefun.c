//
// Description : double 비교함수
// File Name   : doublefun.c
// Date        : 2022. 01. 20. (목) 18:42:06 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef long double DOUBLE;

/// 소수이하 8자리 임계치    123456789(반올림 적용)
#define EPSILON8           0.000000005L
////////////////////////////////////////////////////////////////////////////////
// Double 값이 같은가 비교하는 함수(소숫점 이하 유효숫자 8자리)
// Prototype : int IsEqDouble8(DOUBLE a, DOUBLE b)
// Arguments : DOUBLE a , b : 비교값
// Return    : 1:같음, 0:다름
////////////////////////////////////////////////////////////////////////////////
int IsEqDouble8(DOUBLE aa, DOUBLE bb)
{
	return fabsl(aa - bb) < EPSILON8;
}

////////////////////////////////////////////////////////////////////////////////
// Double 값이 0인가를 검사한다(소숫점 이하 8자리)
// Prototype : int IsZeroDouble8(DOUBLE aa)
// Arguments :
// Return    : 1:zero, 0:not zero
////////////////////////////////////////////////////////////////////////////////
int IsZeroDouble8(DOUBLE aa)
{
	return fabsl(aa - 0.0L) < EPSILON8;
}

////////////////////////////////////////////////////////////////////////////////
// Compare double (소숫점 이하 유효숫자 8자리 기준)
// Prototype : int CompareDouble8(double aa, double bb)
// Arguments : DOUBLE aa, bb : 비교값
// Return    : 0:같음(EPSILON8 이내), 1:aa > bb, -1:aa < bb
////////////////////////////////////////////////////////////////////////////////
int CompareDouble8(DOUBLE aa, DOUBLE bb)
{
	if( fabsl(aa-bb) < EPSILON8) return 0; // EQ
	else if(aa > bb) return 1;   // aa > bb
	else return -1;              // bb > aa
}

////////////////////////////////////////////////////////////////////////////////
// 소수점 이하 8자리 까지만 자르기 (문자열을 in-place로 truncate)
// Prototype : void CutDoubleStr8(char *value)
// Arguments : char *value : "정수.소수" 형태의 숫자 문자열, 그 자리에서 잘라냄
// Return    : 없음 (value 자체가 수정됨)
////////////////////////////////////////////////////////////////////////////////
void CutDoubleStr8(char *value)
{
char *p;
	p = strstr(value, ".");
	if(!p) return ;
	if(strlen(p+1) <=8) return;
	*(p + 9) = 0x00;
	return;
}
////////////////////////////////////////////////////////////////////////////////
// Round up double16
// 소수점 9이하 16자리 까지 숫자가 있으면 1, 없으면 0
// Prototype : int IsRoundUpDouble16(DOUBLE value)
// Arguments : DOUBLE value : 검사할 값
// Return    : 1:반올림 필요(9~16자리에 0이 아닌 자리가 있음), 0:불필요
////////////////////////////////////////////////////////////////////////////////
int IsRoundUpDouble16(DOUBLE value)
{
char temp[255];
register char *p;
int i;
	sprintf(temp, "%.16Lf", value);
	p = strstr(temp, ".");	
	p += 9;
	for(i =0; i < 7; i++, p++) if(*p != 0x30) return 1;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  DOUBLE mode
/// @fn     DOUBLE DoubleMode(DOUBLE a, DOUBLE b)
/// @param[IN] double a
/// @param[IN] double b
/// @return     a % b
////////////////////////////////////////////////////////////////////////////////
DOUBLE DoubleMode(DOUBLE a, DOUBLE b)
{
DOUBLE rem;
    rem = a;
    while(rem > b) rem -=b;
    return rem;
}
