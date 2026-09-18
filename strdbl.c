//
// Description : String 연산 "X, +, -, compare"
// File Name   : strdbl.c
// Date        : 2023. 01. 09. (월) 18:57:08 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "TbData.h"
////////////////////////////////////////////////////////////////////////////////
// 정수부(istr) 앞의 불필요한 '0'을 제거한다 (전부 0이면 "0" 하나로 축약)
// Prototype : static void _zerocls(DOUBLESTR *dstr)
// Arguments : DOUBLESTR *dstr : IN/OUT, istr/ilen이 갱신됨
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
static void _zerocls(DOUBLESTR *dstr)
{
char tmp[100], *s, *d;
int i;
    memset(tmp, 0x00, 100);
    s = dstr->istr;
    for(i = 0; i < dstr->ilen; i++, s++)
        if(*s == '0') continue;
        else break;;
    if(i == dstr->ilen){
        memset(dstr->istr, 0x00, 30);
        *dstr->istr = '0';
        dstr->ilen = 1;
        return ;
    }
    d = tmp;
    for(;i<dstr->ilen; i++, s++, d++)*d = *s;

    i = strlen(tmp);
    memset(dstr->istr, 0x00, 30);
    memcpy(dstr->istr, tmp,i);
    dstr->ilen = i;
    return ;
}

////////////////////////////////////////////////////////////////////////////////
// DOUBLESTR 내용을 stdout에 디버그 출력한다
// Prototype : void DumpDoubleStr(DOUBLESTR *dstr)
// Arguments : DOUBLESTR *dstr : 출력할 값
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
void DumpDoubleStr(DOUBLESTR *dstr)
{
	printf("-------DOUBLESTR Dump-----------\n");
	printf("SIGN[%d]\n", dstr->sign);
	printf("정수부 길이 [%d] 정수부[%s]\n", dstr->ilen, dstr->istr);
	printf("소수부 길이 [%d] 소수부[%s]\n\n", dstr->flen, dstr->fstr);
}
////////////////////////////////////////////////////////////////////////////////
// 문자열 100바이트를 16진수(hex) 형태로 stdout에 디버그 출력한다
// Prototype : void DumpStr100(char *str)
// Arguments : char *str : 최소 100바이트 읽을 수 있어야 함
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
void DumpStr100(char *str)
{
int i;
unsigned char *d = (unsigned char *)str;
	for(i = 0; i < 100; i++, d++)printf("[%02x]", *d);
	printf("\n");
}
//////////////////////////////////////////////////////////////////////////////
///
/// DOUBLESTR 나누기 생각
/// Af = Afi X 1/10^m , Bf = Bfi X 1/10^n
///   (Ai + Af)/(Bi + Bf)
/// = Ai/Bi + Afi/(Bfi X 10^m) + Ai/(Bfi X 10n) + Afi/(Bfi X 10^m X 10^n)
/// ex) 9.3 / 3 : Ai:9, Afi:3, Bi:3, Bfi:0, m=1,n=0
///     = 9/3 + 3/(3 X 10^1) + 9/(0 X 10^0) + 3/(0 X 10^m X 10^n)
///     = 3   + 0.1          +  0           + 0
///     = 3.1
///
//////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// 부호 없는 정수 문자열을 비교한다
// 주의 : 0으로 시작하지않아야한다.
// Prototype : static int _cMPStrStrI(char *a, int alen, char *b, int blen)
// Arguments : char *a, int alen : 첫번째 정수부 문자열/길이
//             char *b, int blen : 두번째 정수부 문자열/길이
// Return    : 1 : a > b, -1 : a < b, 0 : a==b
////////////////////////////////////////////////////////////////////////////////
static int _cMPStrStrI(char *a, int alen, char *b, int blen)
{
int i;
	if(alen > blen ) return 1;
	else if(blen > alen)return -1;
	else{
		for(i= 0; i < alen; i++)
			if(*(a+i) > *(b+i)) return 1;
			else if(*(a+i) < *(b+i)) return -1;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// 부호 없는 소수 문자열을 비교한다
// 주의 : 뒷부분 0이 없어야한다.
// Prototype : static int _cMPStrStrF(char *a, int alen, char *b, int blen)
// Arguments : char *a, int alen : 첫번째 소수부 문자열/길이
//             char *b, int blen : 두번째 소수부 문자열/길이
// Return    : 1 : a > b, -1 : a < b, 0 : a==b
////////////////////////////////////////////////////////////////////////////////
static int _cMPStrStrF(char *a, int alen, char *b, int blen)
{
int i, len;
	if(alen > blen ) len = blen; // 작은것
	else len = alen;
	for(i = 0; i < len; i++)
		if( *(a + i) > *(b +i) ) return 1;
		else if( *(a + i) < *(b + i) ) return -1;
	if(alen > blen) return 1;	
	else if(alen < blen) return -1;
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
// 10의 보수로 변환하는 Function
// Prototype : static void _tenComplement(char *r, char *a)
// Arguments : char *r : [OUT] 결과(strlen(a)와 같은 길이)
//             char *a : [IN]  숫자문자열('0'~'9')
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
static void _tenComplement(char *r, char *a)
{
int len, i, ten;
	len = strlen(a);
	ten = 10;
	// 뒷부분 '0' 지우기
	for(i = len -1; i >= 0; i--){
		if( *(a+i) == 0x30){ *(r + i) = 0x00; continue;}
		else break;
	}
	// 모두 0일때는 '0'을 return
	if(i < 0){*r = 0x30; return;}
	*(r + i) = ten - (*(a +i) - 0x30) + 0x30; i--;
	ten = 9;
	for(;i>=0; i--) *(r+i) = ten - (*(a +i) - 0x30) +0x30;
	return ;
}
////////////////////////////////////////////////////////////////////////////////
// 소수부 문자열 +  연산 (r = a + b, 자릿수는 max(alen,blen)으로 정렬됨)
// Prototype : static int _aDDStrStrF(char *r, char *a, int alen, char *b, int blen)
// Arguments : char *r : [OUT] 결과(길이 max(alen,blen))
//             char *a, int alen : 첫번째 소수부/길이
//             char *b, int blen : 두번째 소수부/길이
// Return    : int : carry
////////////////////////////////////////////////////////////////////////////////
static int _aDDStrStrF(char *r, char *a, int alen, char *b, int blen)
{
int len, i, carry, sum;
	carry = 0;	
	if(alen > blen)len = alen;
	else len = blen;
	for(i = len -1; i>=0  ; i--){
		sum = *(a + i) - '0' + *(b + i) - '0' + carry;
		while(sum < 0) sum += '0';
		if(sum > 9){ carry = 1; sum -= 10;}
		else carry = 0;
		r[i] = sum + '0';
	}
	return carry;
}
////////////////////////////////////////////////////////////////////////////////
// 정수부 문자열 +  연산 (r = a + b + c, r은 x의 길이+1 이상 확보되어야 함)
// Prototype : static int _aDDStrStrI(char *r, char *a, int alen, char *b, int blen, int c)
// Arguments : char *r : [OUT] 결과 (긴쪽 정렬 기준으로 채움, carry 처리 시 앞자리 여유 필요)
//             char *a, int alen : 첫번째 정수부/길이
//             char *b, int blen : 두번째 정수부/길이
//             int  c            : 입력 carry(소수부 연산의 carry를 이어받음)
// Return    : int : carry(최상위 자리 넘침 여부)
////////////////////////////////////////////////////////////////////////////////
static int _aDDStrStrI(char *r, char *a, int alen, char *b, int blen, int c)
{
int carry, sum, xidx, yidx, ridx;
char *x, *y; // X:긴것, Y:짧은것
	carry = c;
	if(alen > blen){
		x = a; xidx = alen - 1; y = b; yidx = blen - 1; ridx = alen - 1;
	} else  {
		x = b; xidx = blen - 1; y = a; yidx = alen - 1;ridx = blen - 1;
	}
	for(;yidx>=0;xidx--, yidx--, ridx --){
		sum = *(x + xidx) - '0' + *(y + yidx) - '0' + carry;
		while(sum < 0) sum += '0';
		if(sum > 9){ carry = 1; sum -= 10;}
		else carry = 0;
		r[ridx] = sum  + '0';
	}
	for(;xidx>=0;xidx --, ridx--){
		sum = *(x + xidx) - '0' + carry;
		if(sum > 9){ carry = 1; sum -= 10;}
		else carry = 0;
		r[ridx] = sum  + '0';
	}
	return carry;
}
////////////////////////////////////////////////////////////////////////////////
// 소수부 문자열 -  연산 (r = a - b, a의 절대값이 b보다 크거나 같다고 가정)
// Prototype : static int _sUBStrStrF(char *r, char *a, int alen, char *b, int blen)
// Arguments : char *r : [OUT] 결과(길이 max(alen,blen))
//             char *a, int alen : 피감수 소수부/길이
//             char *b, int blen : 감수 소수부/길이
// Return    : int : carry(정수부 뺄셈에 넘겨줄 borrow)
////////////////////////////////////////////////////////////////////////////////
static int _sUBStrStrF(char *r, char *a, int alen, char *b, int blen)
{
int len, i, sub;
char carry;
	carry = 0;	
	if(alen > blen)len = alen;
	else len = blen;
	for(i = len -1; i>=0  ; i--){
		sub = *(a+i) - *(b+i) - carry;
		if(sub <= -0x30) sub += 0x30;
		else if(sub >= 0x30) sub -= 0x30;
		if(sub < 0){carry = 1; sub += 10;}	
		else carry = 0;
		r[i] = sub + '0';
	}
	return carry;
}
////////////////////////////////////////////////////////////////////////////////
// 정수부 문자열 -  연산 : a - b (a가 큰것, 즉 alen >= blen 이고 결과가 음수가 아님)
// Prototype : static int _sUBStrStrI(char *r, char *a, int alen, char *b, int blen, int c)
// Arguments : char *r : [OUT] 결과(길이 alen)
//             char *a, int alen : 피감수(큰 수) 정수부/길이
//             char *b, int blen : 감수(작은 수) 정수부/길이
//             int  c            : 입력 borrow(소수부 연산에서 넘어온 값)
// Return    : int : carry(0:+, 1:-) — 항상 0 반환(a가 큰 수라는 전제이므로)
////////////////////////////////////////////////////////////////////////////////
static int _sUBStrStrI(char *r, char *a, int alen, char *b, int blen, int c)
{
int carry, sub, i, aidx;
	carry = c;
	aidx = alen - 1;
	for(i = blen -1; i >= 0; i--, aidx --){
		sub = *(a+aidx) - *(b+i) - carry;
		if(sub < 0){sub += 10;carry = 1;}
		else carry = 0;
		*(r+aidx) = sub + '0';
	}

	for(i = aidx; i >= 0; i--){
		sub = *(a+i) - 0x30 - carry;
		if(sub < 0){sub += 10;carry = 1;}
		else carry = 0;
		*(r+i) = sub + '0';
	}
	return 0; // a가 큰수 임. 항상 carry 없음
}

///////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
//  문자열을  DOUBLESTR로 변환한다 ("[+-]digits[.digits]" 형태, 공백은 skip)
// Prototype : int SetDoubleStr_Str(DOUBLESTR *dstr, char *str)
// Arguments : DOUBLESTR *dstr : [OUT] 변환 결과
//             char      *str  : [IN]  변환할 문자열(NUL 종료, 50자 미만)
// Return    : 1:성공, -1:입력 길이가 50자 이상(실패)
////////////////////////////////////////////////////////////////////////////////
int SetDoubleStr_Str(DOUBLESTR *dstr, char *str)
{
register unsigned char *p, *dp;
register int i, tlen;
	tlen = strlen(str);
	if(tlen >=50) return -1; // 수수점 포함 50이 넘는다
	memset((void *)dstr, 0x00, sizeof(DOUBLESTR));
	i = 0;
	// 1.정수부분
	p = (unsigned char *)str;
	// 1.0 space skip and '0'
	for(i = 0; i < tlen; i++){ if(*p != ' ' && *p !='0') break; p++; }

	if(i == tlen){
		memset(dstr->istr, 0x00, sizeof(dstr->istr));
		memset(dstr->fstr, 0x00, sizeof(dstr->fstr));
		*dstr->istr = '0'; *dstr->fstr = '0';
		dstr->ilen = 1;     dstr->flen = 1;
		dstr->sign = 0;
		return 1;
	}

	// 1.1. 부호
	if(*p == '-'){dstr->sign = 1; i++;p++;}
	else if(*p == '+'){dstr->sign = 0;i++;p++;}
	else dstr->sign = 0;


	// 1.3 현재 위치가 '.' 또는 숫자
	if(*p == '.'){ // '.'
		*dstr->istr = '0'; 
		dstr->ilen = 1;
	}else{ // 숫자
		dp = dstr->istr;
		dstr->ilen = 0;
		for(;i < tlen; i++){
			if(*p == '.') break;
			*dp = *p; p ++; dp ++;dstr->ilen ++;
		}
	}
	if(dstr->ilen == 0){ dstr->ilen = 1; *dstr->istr = '0';}
	if(tlen == i){// 소수부분 없음
		dstr->flen = 1; *dstr->fstr = '0';
		return 1;
	}
	// 2. 소수부분
	// 2.1 '.' skip
	p++;
	//2.2 NULL 까지 소수부분
	dp = dstr->fstr; dstr->flen = 0;
	for(;i<tlen;i++){
		if(*p == 0x00 || *p == ' ')  break;
		*dp = *p; dstr->flen ++; dp++;p++;
	}
	// 2.3 뒤 부분 '0'제거 
	if(dstr->flen){
		for(;dstr->flen;dstr->flen --)
			if(*(dstr->fstr + dstr->flen - 1) != '0') break;
			else *(dstr->fstr + dstr->flen - 1) = 0x00;
	}
	if(dstr->flen == 0){*dstr->fstr = '0';dstr->flen= 1;}
	_zerocls(dstr);//정수부분 0제거
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// DOUBLESTR을 string으로 변환
// Prototype : void GetDoubleStr_Str(char *str, DOUBLESTR *dstr)
// Arguments : char       *str  : Output String Buffer 
//             DOUBLESTR  *dstr : Input
// Return    : void
////////////////////////////////////////////////////////////////////////////////
void GetDoubleStr_Str(char *str, DOUBLESTR *dstr)
{
int  i;
char  *dp, *sp;
	dp = str;
	sp = dstr->istr;
	for(i = 0; i < dstr->ilen; i++){
		if(*sp == '0') sp++;
		else break;
	}
	// 부호처리 
	if(dstr->sign == 1){*dp= '-'; dp++;}
	// 1.1 정수부분
	if(i == dstr->ilen){
		*dp ='0';dp++;
	}else{
		for(; i < dstr->ilen; i++){ *dp = *sp; dp++;sp++; }
	}
	*dp = '.'; dp++;
	// 1.2 소수부분
	sp = dstr->fstr;
	for(i =0 ; i < dstr->flen; i++) {*dp = *sp;dp++;sp++;}
	*dp = 0x00;
}
////////////////////////////////////////////////////////////////////////////////
// DOUBLESTR  크기비교 함수 (부호까지 고려한 실제 값 비교, a - b의 부호)
// Prototype : int CompDoubleStr(DOUBLESTR *a, DOUBLESTR *b)
// Arguments : DOUBLESTR *a, *b : 비교할 두 값
// Return    : 0:a==b, 1:a>b, -1:a<b (정상 케이스 외 -99 반환 시 내부 오류)
////////////////////////////////////////////////////////////////////////////////
int CompDoubleStr(DOUBLESTR *a, DOUBLESTR *b)
{
int cp;
	// 절대값 비교
	cp = _cMPStrStrI(a->istr, a->ilen, b->istr, b->ilen);
	if(cp == 0) cp = _cMPStrStrF(a->fstr, a->flen, b->fstr, b->flen);
	
	// 절대값이 같은 경우
	if(cp == 0){
		if(a->sign == b->sign) return 0; // 부호가 같다
		else if(a->sign > b->sign) return -1; // -, +
		else return 1; // +, -
	}

	// 절대값이 다른경우
	// 부호 비교
	if(a->sign == 1 && !b->sign) return -1; // -, +
	else if (!a->sign && b->sign) return 1; // +, -
	else{ // -, - or +, +
		switch(cp){
			case 1 : // |a| > |b|
				if(a->sign) return -1; // - , -
				else return 1; // + , +
				break;
			case -1 :// |a| < |b|
				if(a->sign) return 1; // - , -
				else return -1; // + , +
				break;
		}
	}
	return -99; // ERROR ?
}
////////////////////////////////////////////////////////////////////////////////
//  DOUBLESTR 더하기 연산 : r = a + b
// Prototype : void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
// Arguments : DOUBLESTR *r  : OUT
//                       *a, *b : input
// Return    :
////////////////////////////////////////////////////////////////////////////////
void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
{
char ir[100], fr[100], *p;
int  len, carry, i, sign;
DOUBLESTR *x, *y;
	
	memset(fr, 0x00, 100);
	memset(ir, 0x00, 100);
	if(a->sign && b->sign){        // -a, -b => -(a+b)
		carry = _aDDStrStrF(fr, a->fstr, a->flen, b->fstr, b->flen);
		len = strlen(fr);
		memcpy(r->fstr, fr, len); r->flen = len;
		p = ir + 1;
		carry = _aDDStrStrI(p, a->istr, a->ilen, b->istr, b->ilen, carry);
		if(carry == 0){len = strlen(p);}
		else{
			p = ir;*ir = carry + 0x30;
			len = strlen(p) + 1;
		}
		memcpy(r->istr, p, len); r->ilen = len;
		r->sign = 1;
	}else if(a->sign && !b->sign){ // -a,  b => b - a
		x = b;y = a;
		// 절대값비교
		carry = _cMPStrStrI(b->istr, b->ilen, a->istr, a->ilen);
		if(carry == 0){
			carry = _cMPStrStrF(b->fstr, b->flen, a->fstr, a->flen);
		}
		if (carry == -1) {x = a; y= b; sign = 1;}
		else sign = 0;

		carry = _sUBStrStrF(fr, x->fstr, x->flen, y->fstr, y->flen);
		carry = _sUBStrStrI(ir, x->istr, x->ilen, y->istr, y->ilen, carry);
		// 정수부분 '0' 삭제
		p = ir;len = strlen(ir);
		for(i = 0; i < len; i ++, p++)if(*p != '0') break;
		if(i == len){
			*r->istr = '0'; r->ilen = 1;
		}else {
			len = strlen(p);
			memcpy(r->istr, p, len);
			r->ilen = len;
		}
		// 소수부분
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;
		r->sign = sign;
	}else if(!a->sign && b->sign){ //  a, -b => a - b 
		x = a;y = b;
		// 절대값비교
		carry = _cMPStrStrI(a->istr, a->ilen, b->istr, b->ilen);
		if(carry == 0){
			carry = _cMPStrStrF(a->fstr, a->flen, b->fstr, b->flen);
		}
		if (carry == -1) {x = b; y= a; sign = 1;}
		else sign = 0;

		carry = _sUBStrStrF(fr, x->fstr, x->flen, y->fstr, y->flen);
		carry = _sUBStrStrI(ir, x->istr, x->ilen, y->istr, y->ilen, carry);
		// 정수부분 '0' 삭제
		p = ir;len = strlen(ir);
		for(i = 0; i < len; i ++, p++)if(*p != '0') break;
		if(i == len){
			*r->istr = '0'; r->ilen = 1;
		}else {
			len = strlen(p);
			memcpy(r->istr, p, len);
			r->ilen = len;
		}
		// 소수부분
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;
		r->sign = sign;

	}else{                         //  a,  b => a + b
		carry = _aDDStrStrF(fr, a->fstr, a->flen, b->fstr, b->flen);
		len = strlen(fr);
		memcpy(r->fstr, fr, len); r->flen = len;
		p = ir + 1;
		carry = _aDDStrStrI(p, a->istr, a->ilen, b->istr, b->ilen, carry);
		if(carry == 0){len = strlen(p);}
		else{
			p = ir;*ir = carry + 0x30;
			len = strlen(p) + 1;
		}
		memcpy(r->istr, p, len); r->ilen = len;
		r->sign = 0;
	}
	_zerocls(r);//정수부분 0제거
	return ;
}
////////////////////////////////////////////////////////////////////////////////
//  DOUBLESTR 빼기 연산 : r = a - b
// Prototype : void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
// Arguments : DOUBLESTR *r  : OUT
//                       *a, *b : input
// Return    :
////////////////////////////////////////////////////////////////////////////////
void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
{
char ir[100], fr[100], *p;
int  len, carry, sign, i;
DOUBLESTR *x, *y;
	memset(ir, 0x00, 100); memset(fr, 0x00, 100);
	memset((void *)r, 0x00, sizeof(DOUBLESTR));
	if(a->sign && b->sign){ // -a , -b => b - a
		x = b;y = a;
		// 절대값비교
		carry = _cMPStrStrI(b->istr, b->ilen, a->istr, a->ilen);
		if(carry == 0){
			carry = _cMPStrStrF(b->fstr, b->flen, a->fstr, a->flen);
		}
		if (carry == -1) {x = a; y= b; sign = 1;}
		else sign = 0;

		carry = _sUBStrStrF(fr, x->fstr, x->flen, y->fstr, y->flen);
		carry = _sUBStrStrI(ir, x->istr, x->ilen, y->istr, y->ilen, carry);
		// 정수부분 '0' 삭제
		p = ir;len = strlen(ir);
		for(i = 0; i < len; i ++, p++)if(*p != '0') break;
		if(i == len){
			*r->istr = '0'; r->ilen = 1;
		}else {
			len = strlen(p);
			memcpy(r->istr, p, len);
			r->ilen = len;
		}
		// 소수부분
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;
		r->sign = sign;
	}else if(a->sign && !b->sign){ // -a, b = -(a + b)
		carry = _aDDStrStrF(fr, a->fstr, a->flen, b->fstr, b->flen);
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;

		p = ir + 1;
		carry = _aDDStrStrI(p, a->istr, a->ilen, b->istr, b->ilen, carry);
		if(carry == 0){len = strlen(p);}
		else{
			p = ir;*ir = carry + 0x30;
			len = strlen(p);
		}
		memcpy(r->istr, p, len);
		r->sign = 1;
	}else if(!a->sign && b->sign){ // a,  -b = a + b
		carry = _aDDStrStrF(fr, a->fstr, a->flen, b->fstr, b->flen);
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;

		p = ir + 1;
		carry = _aDDStrStrI(p, a->istr, a->ilen, b->istr, b->ilen, carry);
		if(carry == 0){len = strlen(p);}
		else{
			p = ir;*ir = carry + 0x30;
			len = strlen(p) + 1;
		}
		memcpy(r->istr, p, len);
		r->ilen = len;
		r->sign = 0;
	}else{ // a , b = > a - b
		x = a; y = b;
		// 절대값비교
		carry = _cMPStrStrI(a->istr, a->ilen, b->istr, b->ilen);
		if(carry == 0){
			carry = _cMPStrStrF(a->fstr, a->flen, b->fstr, b->flen);
		}
		if (carry == -1) {x = b; y= a; sign = 1;}
		else sign = 0;

		carry = _sUBStrStrF(fr, x->fstr, x->flen, y->fstr, y->flen);
		carry = _sUBStrStrI(ir, x->istr, x->ilen, y->istr, y->ilen, carry);
		// 정수부분 '0' 삭제
		p = ir;len = strlen(ir);
		for(i = 0; i < len; i ++, p++)if(*p != '0') break;
		if(i == len){
			*r->istr = '0'; r->ilen = 1;
		}else {
			len = strlen(p);
			memcpy(r->istr, p, len);
			r->ilen = len;
		}
		// 소수부분
		len = strlen(fr);
		memcpy(r->fstr, fr, len);
		r->flen = len;
		r->sign = sign;
	}
	_zerocls(r);//정수부분 0제거
	return ;
}
////////////////////////////////////////////////////////////////////////////////
// DOUBLESTR을 "0"(sign=0, ilen=flen=1, istr="0", fstr="0")으로 초기화한다
// Prototype : void InitDoubleStr(DOUBLESTR *dstr)
// Arguments : DOUBLESTR *dstr : [OUT] 초기화할 값
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
void InitDoubleStr(DOUBLESTR *dstr)
{
	memset((void *)dstr, 0x00, sizeof(DOUBLESTR));
	dstr->ilen = 1;
	dstr->flen = 1;
	*dstr->istr = '0';
	*dstr->fstr = '0';
	dstr->sign = 0;
}
////////////////////////////////////////////////////////////////////////////////
//  DOUBLESTR 곱하기 연산 : r = a X b (long multiplication, 결과는 최대 400자리)
// Prototype : void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
// Arguments : DOUBLESTR *r  : [OUT] 곱셈 결과
//             DOUBLESTR *a, *b : [IN] 입력값
// Return    : 없음
////////////////////////////////////////////////////////////////////////////////
void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b)
{
char result[401], *av, *bv, *rp;
int  ridx, aidx, bidx;
int  aflen, bflen;
int  carry, rpos;
int  rval;

	memset((void *)r, 0x00, sizeof(DOUBLESTR));
	memset(result, 0x00, 401);
	ridx = 400  - 1; // 맨 마지막이 0x00;
	bidx = b->flen + b->ilen - 1;
	bflen = 0;bv = b->fstr + b->flen - 1;
	rpos = 0; 
	for(;bidx>= 0;){

		aidx = a->flen + a->ilen -1;
		aflen = 0; av = a->fstr + a->flen -1;
		ridx = 400 - rpos - 1;carry = 0;
		for(;aidx >= 0;){
			rval = *(result + ridx);
			if(rval == 0x00) rval = 0;
			else rval = rval - '0';

			rval = rval + (*av - '0') * (*bv - '0') + carry;
			carry = rval / 10;
			rval = rval % 10;
			*(result + ridx) = rval + '0';
			aflen ++; aidx --;
			if(aflen == a->flen)av = a->istr +  a->ilen - 1;
			else av --;

			ridx --;
				
		}

		if(carry) *(result + ridx) = carry + '0';

		bflen ++; bidx --;
		if(bflen == b->flen)bv = b->istr +  b->ilen - 1;
		else bv --;
		rpos ++;
	}
	if(carry) *(result + ridx) = carry +'0';
	else        ridx += 1;

	rp = result + ridx; // 결과의 맨처음 Point

	rval = strlen(rp); //  결과 전체 길이

	// 소수점 이하, 이동(뒷부분부터 체우기)
	r->flen = a->flen + b->flen;	
	av = r->fstr;bv = rp + rval - r->flen;
	for(ridx = 0; ridx < r->flen; ridx++, av++, bv++) *av = *bv;

	// 정수부분(처음 부터 체우기)
	r->ilen = rval - r->flen;
	av = r->istr; bv = rp;
	for(ridx = 0; ridx < r->ilen; ridx++, av++, bv++) *av = *bv;

	// 부호처리
	if(a->sign == b->sign) r->sign = 0;
	else r->sign = 1;
	_zerocls(r);//정수부분 0제거
}
///////////////////////////////////////////////////////////////////////////////
// DOUBLESTR 값이 0인지 검사한다(정수부/소수부 모든 자리가 '0'인지)
// Prototype : int IsZeroDoubleStr(DOUBLESTR *dstr)
// Arguments : DOUBLESTR *dstr : 검사할 값
// Return    : 1:0임, 0:0아님
///////////////////////////////////////////////////////////////////////////////
int IsZeroDoubleStr(DOUBLESTR *dstr)
{
int fg, i;
char *p;
	fg = 0;p = dstr->istr;
	for(i= 0; i < dstr->ilen; i++, p++)
		if(*p != '0'){fg = 1; break;}
	if(fg) return 0; // 0아님
	fg = 0;p = dstr->fstr;
	for(i= 0; i < dstr->flen; i++, p++)
		if(*p != '0'){fg = 1; break;}
	if(fg) return 0;
	else return 1; // 0임
}
///////////////////////////////////////////////////////////////////////////////
// DOUBLESTR을 복사한다 (memcpy 기반 struct 전체 복사)
// Prototype : void CopyDoubleStr(DOUBLESTR *dst, DOUBLESTR *src)
// Arguments : DOUBLESTR *dst : [OUT] 복사될 대상
//             DOUBLESTR *src : [IN]  원본
// Return    : 없음
///////////////////////////////////////////////////////////////////////////////
void CopyDoubleStr(DOUBLESTR *dst, DOUBLESTR *src)
{
	memcpy((void*)dst, (void *)src, sizeof(DOUBLESTR));
}
////////////////////////////////////////////////////////////////////////////////
// 절사 및 절상 
// Prototype : void RoundDoubleStr(DOUBLESTR *dstr,  int mode)
// Arguments : int mode : 1:9자리에서 올림(5이상 올림)
//                        2:8자리 이하 버림
//                        3:9자리에서 올림(0이 아니면 올림)
// Return    :
////////////////////////////////////////////////////////////////////////////////
void RoundDoubleStr(DOUBLESTR *dstr, int mode)
{
char *p, rt[100];
int rlen, carry;
	if(dstr->flen < 9) return;
	p = dstr->fstr + 8; // 9번째
	switch (mode){
		case 1 : // 9자리에서 올림(5이상 올림)
			if(*p >= '5'){
				memset(rt, 0x00, 50);
				carry = _aDDStrStrF(rt, dstr->fstr, 8, "00000001", 8);
				if(carry){ // 소수부분 Carry 있음, 정수 더하기 1
					// 정수분 carry 처리
					memset(rt, 0x00, 100);
					p = dstr->istr;
					carry = _aDDStrStrI(rt,dstr->istr, dstr->ilen, "1", 1, 0); 
					memset(dstr->istr, 0x00, DSTR_LEN);
					if(carry){ *p = '1'; p++;}
					rlen = strlen(rt);
					memcpy(p, rt, rlen);
					dstr->ilen = carry + rlen;
					// 소수부분은 '0'임
					dstr->flen = 1;
					memset(dstr->fstr, 0x00, DSTR_LEN);
					*dstr->fstr = '0';
				}else{ // 소수부분 Carry 없음
					memset(dstr->fstr, 0x00, DSTR_LEN);
					memcpy(dstr->fstr, rt, 8);
					dstr->flen = 8;
				}
			}else{
				dstr->flen = 8;
				p = dstr->fstr;
				for(rlen = 8; rlen < DSTR_LEN; rlen++) *(p + rlen)= 0x00;
			}
			break;
		case 2 : // 8자리 이하 버림
			dstr->flen = 8;
			p = dstr->fstr;
			for(rlen = 8; rlen < DSTR_LEN; rlen++) *(p + rlen)= 0x00;
			break;
		case 3 : // 9자리에서 올림(0 아니면 올림)
			if(*p == '0'){ // 버림
				dstr->flen = 8;
				p = dstr->fstr;
				for(rlen = 8; rlen < DSTR_LEN; rlen++) *(p + rlen)= 0x00;
				break;
			}
			// 올림 처리
			memset(rt, 0x00, 50);
			carry = _aDDStrStrF(rt, dstr->fstr, 8, "00000001", 8);
			if(carry){ // 소수부분 Carry 있음, 정수 더하기 1
				// 정수분 carry 처리
				memset(rt, 0x00, 100);
				p = dstr->istr;
				carry = _aDDStrStrI(rt,dstr->istr, dstr->ilen, "1", 1, 0); 
				memset(dstr->istr, 0x00, DSTR_LEN);
				if(carry){ *p = '1'; p++;}
				rlen = strlen(rt);
				memcpy(p, rt, rlen);
				dstr->ilen = carry + rlen;
				// 소수부분은 '0'임
				dstr->flen = 1;
				memset(dstr->fstr, 0x00, DSTR_LEN);
				*dstr->fstr = '0';
			}else{ // 소수부분 Carry 없음
				memset(dstr->fstr, 0x00, DSTR_LEN);
				memcpy(dstr->fstr, rt, 8);
				dstr->flen = 8;
			}
			break;
		default :
			return ;
	}
	return ;
}
