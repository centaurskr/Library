//
// Description :
// File Name   : mpfr.c
// Date        : 2022. 11. 25. (금) 14:14:34 KST
// By  : Cento
//
//#include <gmp.h>
#include <mpfr.h>

////////////////////////////////////////////////////////////////////////////////
// Dluble 문자열 곱하기 r = X * Y
// Prototype : void DStrStrMul(char *r, char *x, char *y, int prec, int rtype)
// Arguments : char *r : 결과 Output == 공간이 충분해야함
//             char *, *y : 입력값
//             int  prec  : 소수점 이하 자리수
//             int  rtype : 1 : Round up(큰수 쪽으로),0:Round cut(작은수쪽으로)
// Return    : void 
////////////////////////////////////////////////////////////////////////////////
void DStrStrMul(char *r, char *x, char *y, int prec, int rtype)
{
mpfr_t rr, xx, yy;
	mpfr_init2(rr, 256); mpfr_init2(xx, 256); mpfr_init2(yy, 256);
	
	mpfr_set_str(xx, x, 10, MPFR_RNDD);
	mpfr_set_str(yy, y, 10, MPFR_RNDD);

	mpfr_mul(rr, xx, yy, MPFR_RNDD);

	if(rtype == 1) // ROUND UP
		mpfr_sprintf(r, "%.*RUf", prec, rr);
	else
		mpfr_sprintf(r, "%.*RDf", prec, rr);
	mpfr_clears(rr,xx,yy, NULL);
	return;
}
////////////////////////////////////////////////////////////////////////////////
// Dluble 문자열 나누기 r = X / Y
// Prototype : void DStrStrDiv(char *r, char *x, char *y, int prec, int rtype)
// Arguments : char *r : 결과 Output == 공간이 충분해야함
//             char *, *y : 입력값
//             int  prec  : 소수점 이하 자리수
//             int  rtype : 1 : Round up(큰수 쪽으로),0:Round cut(작은수쪽으로)
// Return    : void
// BUG NOTE  : 아래 구현은 mpfr_div가 아닌 mpfr_mul을 호출하고 있어 실제로는
//             나눗셈이 아니라 DStrStrMul과 동일한 곱셈을 수행한다(복사/붙여넣기
//             오류로 보임). 이 함수를 나눗셈으로 신뢰하고 사용하지 말 것.
////////////////////////////////////////////////////////////////////////////////
void DStrStrDiv(char *r, char *x, char *y, int prec, int rtype)
{
mpfr_t rr, xx, yy;
	mpfr_init2(rr, 256); mpfr_init2(xx, 256); mpfr_init2(yy, 256);
	
	mpfr_set_str(xx, x, 10, MPFR_RNDD);
	mpfr_set_str(yy, y, 10, MPFR_RNDD);

	mpfr_mul(rr, xx, yy, MPFR_RNDD);

	if(rtype == 1) // ROUND UP
		mpfr_sprintf(r, "%.*RUf", prec, rr);
	else
		mpfr_sprintf(r, "%.*RDf", prec, rr);
	mpfr_clears(rr,xx,yy, NULL);
	return;
}


////////////////////////////////////////////////////////////////////////////////
// Dluble 문자열 더하기 r = X + Y
// Prototype : void DStrStrAdd(char *r, char *x, char *y, int prec, int rtype)
// Arguments : char *r : 결과 Output == 공간이 충분해야함
//             char *, *y : 입력값
//             int  prec  : 소수점 이하 자리수
//             int  rtype : 1 : Round up(큰수 쪽으로),0:Round cut(작은수쪽으로)
// Return    : void 
////////////////////////////////////////////////////////////////////////////////
void DStrStrAdd(char *r, char *x, char *y, int prec, int rtype)
{
mpfr_t rr, xx, yy;
	mpfr_init2(rr, 256); mpfr_init2(xx, 256); mpfr_init2(yy, 256);
	
	mpfr_set_str(xx, x, 10, MPFR_RNDD);
	mpfr_set_str(yy, y, 10, MPFR_RNDD);

	mpfr_add(rr, xx, yy, MPFR_RNDD);

	if(rtype == 1) // ROUND UP
		mpfr_sprintf(r, "%.*RUf", prec, rr);
	else
		mpfr_sprintf(r, "%.*RDf", prec, rr);
	mpfr_clears(rr,xx,yy, NULL);
	return;
}
////////////////////////////////////////////////////////////////////////////////
// Dluble 문자열 빼기 r = X - Y
// Prototype : void DStrStrSub(char *r, char *x, char *y, int prec, int rtype)
// Arguments : char *r : 결과 Output == 공간이 충분해야함
//             char *, *y : 입력값
//             int  prec  : 소수점 이하 자리수
//             int  rtype : 1 : Round up(큰수 쪽으로),0:Round cut(작은수쪽으로)
// Return    : void 
////////////////////////////////////////////////////////////////////////////////
void DStrStrSub(char *r, char *x, char *y, int prec, int rtype)
{
mpfr_t rr, xx, yy;
	mpfr_init2(rr, 256); mpfr_init2(xx, 256); mpfr_init2(yy, 256);
	
	mpfr_set_str(xx, x, 10, MPFR_RNDD);
	mpfr_set_str(yy, y, 10, MPFR_RNDD);

	mpfr_sub(rr, xx, yy, MPFR_RNDD);

	if(rtype == 1) // ROUND UP
		mpfr_sprintf(r, "%.*RUf", prec, rr);
	else
		mpfr_sprintf(r, "%.*RDf", prec, rr);
	mpfr_clears(rr,xx,yy, NULL);
	return;
}
