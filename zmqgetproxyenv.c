///
/// ## Zmq Proxy 환경변수 읽기
///
/// ##환경변수 
/// -# FRONT   : (필수) REQ 또는 PUB endpint
/// -# BACK    : (필수) REP 또는 SUB endpint
/// -# CONTROL : (옵션) CONTROL endpoint (PAUSE, RESUM, TERMINATE control)
/// -# CAPTURE : (옵션) Data Capture를 위한 endpoint
///
/// @file getenv.c
/// @date 2023. 12. 07. (목) 10:27:36 KST
/// @author Cento 
///
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define  MASK_ID    0x01 ///< ID Value mask
#define  MASK_FRONT 0x02 ///< FRONT Value mask
#define  MASK_BACK  0x04 ///< BACK Value mask
#define  MASK_CTL   0x08 ///< CONTROL Value mask
#define  MASK_CAP   0x10 ///< CAPTURE Value mask

#define IS_ID(fg)    (fg & MASK_ID)
#define IS_FRONT(fg) (fg & MASK_FRONT)
#define IS_BACK(fg)  (fg & MASK_BACK)
#define IS_CTL(fg)   (fg & MASK_CTL)
#define IS_CAP(fg)   (fg & MASK_CAP)
////////////////////////////////////////////////////////////////////////////////
/// String이 같은가를 비교한다
///
/// A and B, B and A
///
/// @brief  String 비교
/// @param  a [IN] string a
/// @param  b [IN] string b
/// @return 
///        1 :  같음
///        0 :  다름
////////////////////////////////////////////////////////////////////////////////
static int IsEqStr(char *a, char *b)
{
	if(strlen(a) != strlen(b)) return 0;
	if(memcmp(a, b, strlen(a))) return 0;
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
/// 왼쪽 Space제거 
/// @brief  L space 제거
/// @param  str [IN] string
/// @return 없음
////////////////////////////////////////////////////////////////////////////////
void _LTrim(char *str)
{
char *start;
	
    // Check if the string is NULL or empty
    if (str == NULL || *str == '\0') return;
	start = str;
    // Find the first non-space character
    while (isspace((unsigned char)(*str))) str++;

    // Shift the remaining characters to the beginning of the string
    memmove(start, str, strlen(str) + 1);
}
////////////////////////////////////////////////////////////////////////////////
/// 오른쪽 Space제거 
/// @brief  R space 제거
/// @param  str [IN] string
/// @param  len [IN] str 길이
/// @return 없음
////////////////////////////////////////////////////////////////////////////////
static void _RTrim(char *str, int len)
{
char *p;
	p = str + len;
	while(p != str){
		if(*p == ' ' || *p == '\t' || *p == '\n') *p = 0x00;
		p--;
	}
	return;
}
////////////////////////////////////////////////////////////////////////////////
///  Zmq Proxy server 환경변수를 읽는다
/// @brief  Proxy 환경 읽기
/// @param   file   [IN ] 환경변수 파일 full path
/// @param   id     [OUT] 환경변수 파일의 ID 값 (필수, 1 이상이어야 함)
/// @param   front  [OUT] REQ 또는 PUB endpoint
/// @param   back   [OUT] REP 또는 SUB endpoint
/// @param   ctl    [OUT] control  endpoint
/// @param   cap    [OUT] data capture endpoint
/// @return  integer
///        1 : 성공
///       -1 : file이 없음
///       -2 : 환경 파일 오류(ID, FRONT 또는 BACK 중 하나라도 없거나 중복 선언됨)
////////////////////////////////////////////////////////////////////////////////
int GetProxyEnv(char *file, int *id, 
	char *front, char *back, char *ctl, char *cap)
{
FILE *fp;
char buff[512], *pstr, key[12], value[100];
int  fg = 0, rtn = 1;
	fp = fopen(file, "rt");
	if(!fp) return -1;
//printf("FILE [%s]\n", file);
	memset(buff, 0x00, 512);
	/// - Loop를 돌면서 파일을 읽는다
	///    + 처음이 #이면 주석으로 간주한다.
	///    + FRONT 정의부분 검사
	///    + BACK 정의부분 검사
	///    + CONTROL 정의부분 검사
	///    + CAPTURE 정의부분 검사
	while(fgets(buff, 512, fp)){
		if(*buff == '#'){memset(buff, 0x00, 512); continue;}
		pstr = strstr(buff, "\n");
		if(pstr != NULL)*pstr = 0x00;
//printf("\nREAD[%s]\n", buff);
		memset(key, 0x00, 12);
		pstr = strtok(buff, "=");
		if(!pstr){memset(buff, 0x00, 512); continue;}
		memcpy(key, pstr, strlen(pstr));
		_RTrim(key, strlen(key));

//printf("KEY [%s] fg[0x[%08x] [0x%08x] [0x%08x]\n", 
		//key, fg, MASK_FRONT, fg & MASK_FRONT);

		memset(value, 0x00, 100);
		if(IsEqStr(key, "ID")){
			if(fg & MASK_ID){rtn = -2; break;} // 중복선언
			pstr = strtok(NULL, "\n");
			if(!pstr){rtn = -2; break;} // 값이 없음
			sprintf(value, "%.80s", pstr);
			_RTrim(value, strlen(value));
			_LTrim(value);
			*id = atoi(value);
			if(*id <= 0){rtn = -2;break;}
			fg = fg | MASK_ID;
		}else if(IsEqStr(key, "FRONT")){
			if(fg & MASK_FRONT){rtn = -2; break;} // 중복선언
			pstr = strtok(NULL, "\n");
			if(!pstr){rtn = -2; break;} // 값이 없음
			sprintf(value, "%.80s", pstr);
			_RTrim(value, strlen(value));
			_LTrim(value);
			memcpy(front, value, strlen(value));
			fg = fg | MASK_FRONT;
		}else if(IsEqStr(key, "BACK")){
			if(fg & MASK_BACK){rtn = -2; break;} // 중복선언
			pstr = strtok(NULL, "\n");
			if(!pstr){rtn = -2; break;} // 값이 없음
			sprintf(value, "%.80s", pstr);
			_RTrim(value, strlen(value));
			_LTrim(value);
			memcpy(back, value, strlen(value));
			fg = fg | MASK_BACK;
		}else if(IsEqStr(key, "CONTROL")){
			if(fg & MASK_CTL){rtn = -2; break;} // 중복선언
			pstr = strtok(NULL, "\n");
			if(!pstr){rtn = -2; break;} // 값이 없음
			sprintf(value, "%.80s", pstr);
			_RTrim(value, strlen(value));
			_LTrim(value);
			memcpy(ctl, value, strlen(value));
			fg = fg | MASK_CTL;
		}else if(IsEqStr(key, "CAPTURE")){
			if(fg & MASK_CAP){rtn = -2; break;} // 중복선언
			pstr = strtok(NULL, "\n");
			if(!pstr){rtn = -2; break;} // 값이 없음
			sprintf(value, "%.80s", pstr);
			_RTrim(value, strlen(value));
			_LTrim(value);
			memcpy(cap, value, strlen(value));
			fg = fg | MASK_CAP;
		}
		memset(buff, 0x00, 512);
	}
	fclose(fp);
	//printf("RETURN FG [%08x] F|B[%08x]\n", fg, MASK_FRONT | MASK_BACK);
	if(IS_FRONT(fg) && IS_BACK(fg) && IS_ID(fg)) return rtn;
	else return -2;
}
