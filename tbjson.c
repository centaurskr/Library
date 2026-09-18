#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _KEYVAL{ char *key; char *val; }KEYVAL;
typedef struct _JSONPARS{ int cnt; KEYVAL *keyval; }JSONPARS;	

////////////////////////////////////////////////////////////////////////////////
// Description : JsonParser()가 반환한 파서 컨텍스트와 그 안의 모든
//               key/value 문자열을 해제한다.
// Prototype : void JsonParserFree(void *ctx)
// Arguments : ctx : JsonParser()가 반환한 컨텍스트
// Return    : void
////////////////////////////////////////////////////////////////////////////////
void JsonParserFree(void *ctx)
{
int i;
JSONPARS *pjson;
KEYVAL   *pkv;
	pjson = (JSONPARS *)ctx;
	for(i = 0; i < pjson->cnt; i++){
		pkv = pjson->keyval + i;
		if(pkv->key)  free(pkv->key);
		if(pkv->val)  free(pkv->val);
	}
	free(pjson->keyval);
	free(pjson);
}
////////////////////////////////////////////////////////////////////////////////
// Description : "KEY":VALUE 형태의 문자열 한 조각을 파싱해서 ctx의
//               key/value 목록에 추가한다 (JsonParser() 내부에서
//               콤마로 잘라진 각 토큰마다 호출됨). 중첩 객체/배열이나
//               이스케이프된 따옴표는 지원하지 않는 단순 파서다.
// Prototype : int JsonParserAdd(void *ctx, char *str)
// Arguments : ctx : JsonParser()가 반환한 컨텍스트
//             str : "KEY":"VALUE" 또는 "KEY":12314 형태의 조각
// Return    : 성공 1, 형식이 맞지 않거나 메모리 할당 실패 시 0
////////////////////////////////////////////////////////////////////////////////
int JsonParserAdd(void *ctx, char *str)
{
char *p, *kp, *vp;
char  key[512], value[512];
JSONPARS *pars;
KEYVAL   *pkv;
int       fg = 0;
	pars = (JSONPARS *)ctx;
	p = str;
	memset(key, 0x00, 512); memset(value, 0x00, 512);
	kp = key; vp = value;
	while(1)
		if(*p == '"' || *p == ' ') p++;
		else if(p == NULL)break;
		else {fg = 1; break;}
	if(!fg) return 0;

	// KEY
	fg = 1;
	while(*p != '"'){
		if(p == NULL){fg = 0; break;}
		*kp++ = *p++;
	}
	if(!fg) return 0;

	fg = 0;
	while(1)
		if(*p == '"' || *p == ':' || *p == ' ') p++;
		else if(p == NULL)break;
		else{ fg = 1; break;}
	if(!fg) return 0;

	// VALUE
	fg = 1;
	while(*p != '"' && *p!= '}'){
		if(p == NULL){fg = 0; break;}
		*vp++ = *p++;
	}
	if(!fg) return 0;

	pars->keyval = (KEYVAL *)realloc(pars->keyval,(pars->cnt+1)*sizeof(KEYVAL));
	if(pars->keyval == NULL) return 0;	
	pkv = pars->keyval + pars->cnt;
	pkv->key   = strdup(key);	
	pkv->val   = strdup(value);	
	pars->cnt ++;

	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// Description : 평면(단일 레벨) JSON 객체 문자열을 파싱한다. '{' 이후를
//               strtok(",")로 잘라 각 "KEY":VALUE 조각을 JsonParserAdd()로
//               추가한다. 중첩 객체/배열은 지원하지 않으며, strtok()을
//               쓰므로 스레드 세이프하지 않고 입력 버퍼(json)가 변형된다.
// Prototype : void *JsonParser(char *json)
// Arguments : json : 파싱할 JSON 문자열 (strtok에 의해 내용이 변경됨)
// Return    : 성공 시 JsonParserGet()에 넘길 파서 컨텍스트, 실패 시 NULL
////////////////////////////////////////////////////////////////////////////////
void *JsonParser(char *json)
{
JSONPARS *pjson;
char *p, *tok;
int   fg = 0, rtn;
	pjson = calloc(1, sizeof(JSONPARS));
	if(pjson == NULL) return NULL;
	// Seek '{'
	p = json;
	while(1){
		if(p == NULL) break;
		else if(*p == '{'){fg = 1; p++;break;}
		p++;
	}
	if(fg == 0){ free(pjson); return NULL;}
	tok = strtok(p, ",");
	if(!tok){free(pjson); return NULL;}
	rtn = JsonParserAdd((void *)pjson,  tok);
	if(!rtn){free(pjson); return NULL;}
	while(1){
		tok = strtok(NULL, ",");
		if(!tok) break;
		rtn = JsonParserAdd((void *)pjson, tok);
		if(!rtn){JsonParserFree((void *)pjson); return NULL;}
	}
	return pjson;
}
////////////////////////////////////////////////////////////////////////////////
// Description : JsonParser()로 파싱된 결과에서 key에 해당하는 value를 찾는다.
// Prototype : char *JsonParserGet(void *ctx, char *key)
// Arguments : ctx : JsonParser()가 반환한 컨텍스트
//             key : 찾을 key 이름
// Return    : 찾은 value 문자열(내부 버퍼, 호출자가 free하면 안 됨),
//             없으면 NULL
////////////////////////////////////////////////////////////////////////////////
char *JsonParserGet(void *ctx, char *key)
{
int i;
JSONPARS *pars = (JSONPARS *)ctx;
KEYVAL   *pkv;

	for(i =0 ; i < pars->cnt; i++){
		pkv = pars->keyval + i;
		if(!memcmp(pkv->key, key, strlen(key))){
			if(!memcmp(key, pkv->key, strlen(pkv->key))) return pkv->val;
		}
	}
	return NULL;
}

typedef struct _TBJSON{
	char *str;
	int   size;
}TBJSON;

// ---------------------------------------------------------------------------
// 아래는 JSON 빌더 API다 (위 JsonParser* 계열과는 별개의 독립적인 기능).
// JsonInit() -> JsonStart() -> JsonAdd*()/JsonArrayStart()+JsonAdd*()+
// JsonArrayEnd() 반복 -> JsonEnd() -> JsonGetStr()/JsonGetLen() -> JsonFree()
// 순서로 호출해서 JSON 문자열을 점진적으로 조립한다. 각 Json*() 호출은
// json->str을 realloc()하며 널 종료를 보장하지 않으므로, 조립 도중에는
// JsonGetLen()으로 얻은 길이만큼만 유효하다고 가정해야 한다.
// ---------------------------------------------------------------------------
////////////////////////////////////////////////////////////////////////////////
// Description : JSON 빌더 컨텍스트를 생성한다.
// Prototype   : void *JsonInit()
// Arguments   : 없음
// Return      : TBJSON 컨텍스트 포인터 (calloc 실패 시 NULL을 반환하지
//               않고 그대로 역참조하므로 실패 시 동작은 보장되지 않음)
////////////////////////////////////////////////////////////////////////////////
void *JsonInit()
{
TBJSON *json;
	json = (TBJSON *)calloc(1, sizeof(TBJSON));
	json->size = 0;	
	return json;
}

////////////////////////////////////////////////////////////////////////////////
// Description : JSON 빌더 컨텍스트와 그 안의 조립된 문자열 버퍼를 해제한다.
// Prototype   : void JsonFree(void *jp)
// Arguments   : jp : JsonInit()이 반환한 컨텍스트
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonFree(void *jp)
{
TBJSON *json = (TBJSON *)jp;
	if(json->str) free(json->str);
	free(jp);
}

////////////////////////////////////////////////////////////////////////////////
// Description : JSON object START. 버퍼에 '{'를 추가한다.
// Prototype   : void JsonStart(void *jp)
// Arguments   : jp : JsonInit()이 반환한 컨텍스트
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonStart(void *jp)
{
TBJSON *json = (TBJSON *)jp;
int  size;
	size = json->size;
	json->str = (char *)realloc((void *)json->str, size + 1);
	memcpy((json->str + json->size), "{", 1);
	json->size = size + 1;
}
////////////////////////////////////////////////////////////////////////////////
//  JSON Array Start. 버퍼에 "key":[ 를 추가한다.
// Prototype : void JsonArrayStart(void *jp, char *key)
// Arguments : jp  : JsonInit()이 반환한 컨텍스트
//             key : 배열의 key 이름
// Return    : void
////////////////////////////////////////////////////////////////////////////////
void JsonArrayStart(void *jp, char *key)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	size = json->size;
	sprintf(temp, "\"%s\":[ ", key);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);

	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : KEY + VALUE(문자열) 추가. "key":"val" , 형태로 뒤에 붙인다.
// Prototype   : void JsonAdd(void *jp, char *key, char *val)
// Arguments   : jp  : JsonInit()이 반환한 컨텍스트
//               key : 필드 이름
//               val : 문자열 값 (1024바이트 내부 임시버퍼를 쓰므로
//                     "key":"val" 조합 길이가 1024를 넘으면 안 됨)
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonAdd(void *jp, char *key, char *val)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	
	size = json->size;
	sprintf(temp, "\"%s\":\"%s\" ,", key, val);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : KEY + VALUE(Integer) 추가. "key":val , 형태로 뒤에 붙인다.
// Prototype   : void JsonAddInt(void *jp, char *key, int val)
// Arguments   : jp  : JsonInit()이 반환한 컨텍스트
//               key : 필드 이름
//               val : 정수 값
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonAddInt(void *jp, char *key, int val)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	
	size = json->size;
	sprintf(temp, "\"%s\":%d ,", key, val);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : KEY + VALUE(long) 추가. "key":val , 형태로 뒤에 붙인다.
// Prototype   : void JsonAddLong(void *jp, char *key, long val)
// Arguments   : jp  : JsonInit()이 반환한 컨텍스트
//               key : 필드 이름
//               val : long 값
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonAddLong(void *jp, char *key, long val)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	
	size = json->size;
	sprintf(temp, "\"%s\":%ld ,", key, val);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : KEY + VALUE(double) 추가. "key":val , 형태로 뒤에 붙인다
//               (%f 포맷이므로 소수점 6자리까지 고정 출력됨).
// Prototype   : void JsonAddDouble(void *jp, char *key, double val)
// Arguments   : jp  : JsonInit()이 반환한 컨텍스트
//               key : 필드 이름
//               val : double 값
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonAddDouble(void *jp, char *key, double val)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	
	size = json->size;
	sprintf(temp, "\"%s\":%f ,", key, val);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : KEY + VALUE(long double) 추가. "key":val , 형태로 뒤에
//               붙인다 (%Lf 포맷).
// Prototype   : void JsonAddLDouble(void *jp, char *key, long double val)
// Arguments   : jp  : JsonInit()이 반환한 컨텍스트
//               key : 필드 이름
//               val : long double 값
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonAddLDouble(void *jp, char *key, long double val)
{
char temp[1024], *p;
int  len, size;
TBJSON *json = (TBJSON *)jp;
	
	size = json->size;
	sprintf(temp, "\"%s\":%Lf ,", key, val);
	len = strlen(temp);
	size += len;

	json->str = (char *)realloc((void *)json->str, size);
	memcpy(json->str+json->size, temp, len);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : JSON Object End. 직전에 남은 trailing comma를 공백으로
//               지운 뒤 '}'를 붙인다.
// Prototype   : void JsonEnd(void *jp, int type)
// Arguments   : jp       : JsonInit()이 반환한 컨텍스트
//               int type : 1:Continue(뒤에 ',' 추가, 상위 객체/배열이 계속됨)
//                          0:End(콤마 없이 종료)
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void JsonEnd(void *jp, int type)
{
TBJSON *json = (TBJSON *)jp;
int  len, size;
char *p;
	p = json->str + json->size -1;
	if(*p == ',') *p = ' ';
	size = json->size + 3;
	json->str = (char *)realloc((void *)json->str, size);
	if (type == 1)memcpy(json->str+json->size, "} ,", 3);
	else memcpy(json->str+json->size, "}  ", 3);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
//  JSON Array END. 직전에 남은 trailing comma를 공백으로 지운 뒤 ']'를 붙인다.
// Prototype : void JsonArrayEnd(void *jp, int type)
// Arguments : jp       : JsonInit()이 반환한 컨텍스트
//             int type : 1:Continue(뒤에 ',' 추가), 0:End(콤마 없이 종료)
// Return    : void
////////////////////////////////////////////////////////////////////////////////
void JsonArrayEnd(void *jp, int type)
{
TBJSON *json = (TBJSON *)jp;
int  len, size;
char *p;
	p = json->str + json->size -1;
	if(*p == ',') *p = ' ';
	size = json->size + 3;
	json->str = (char *)realloc((void *)json->str, size);
	if(type == 1)
		memcpy(json->str+json->size, "] ,", 3);
	else 
		memcpy(json->str+json->size, "]  ", 3);
	json->size = size;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Json string pointer 얻기. JsonEnd() 호출까지 마친 뒤에
//               쓰는 것이 일반적이다.
// Prototype   : char *JsonGetStr(void *jp)
// Arguments   : jp : JsonInit()이 반환한 컨텍스트
// Return      : 조립된 JSON 문자열 버퍼 포인터 (호출자가 free하면 안 됨;
//               길이는 JsonGetLen()으로 얻어야 함 — 널 종료가 보장되지 않음)
////////////////////////////////////////////////////////////////////////////////
char *JsonGetStr(void *jp)
{
TBJSON *json = (TBJSON *)jp;
	return json->str;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Json string 길이 얻기
// Prototype   : int JsonGetLen(void *jp)
// Arguments   : jp : JsonInit()이 반환한 컨텍스트
// Return      : 현재까지 조립된 JSON 문자열의 바이트 길이
////////////////////////////////////////////////////////////////////////////////
int JsonGetLen(void *jp)
{
TBJSON *json = (TBJSON *)jp;
	return json->size;
}
