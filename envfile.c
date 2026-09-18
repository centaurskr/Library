//
// Description :
// File Name   : envfile.c
// Date        : 2017. 07. 18. (화) 15:26:15 KST
// By          : centaurskr@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

////////////////////////////////////////////////////////////////////////////////
// Description : "key = value" 형태의 한 줄에서 '=' 앞의 key 부분만 잘라낸다.
//               공백/탭은 건너뛰고 '='를 만나면 종료한다. GetEnvValue() 내부
//               에서 각 줄의 key를 파싱하는 데 쓰이는 헬퍼 함수.
// Prototype   : int CutKey(char *buff, char *key)
// Arguments   : buff : "key = value" 형태의 입력 줄 (읽기 전용으로 쓰임)
//               key  : 잘라낸 key를 담을 출력 버퍼 (buff보다 작지 않아야 함)
// Return      : '='를 찾아 key를 채웠으면 1, buff 끝까지 '='가 없으면 0
////////////////////////////////////////////////////////////////////////////////
int CutKey(char *buff, char *key)
{
char *p, *d;
	p = buff;d= key;
	while(*p){
		if(*p == ' ' || *p == '\t'){p++;continue;}
		if(*p == '=') {*d = 0x00; return 1;}
		*d = *p;
		p++; d++;
	}
	return 0;
}
/*******************************************************************************
 * Function Name : GetEnvValue
 * Description   : INI 스타일 설정 파일(fname)에서 [section] 아래의 key 값을
 *                  읽어온다. 값 뒤의 '#' 주석과 좌우 공백/탭은 제거된다.
 * Arguments     : fname   : 설정 파일 경로
 *                 section : 찾을 섹션 이름 ("[section]"의 section 부분)
 *                 key     : 찾을 key 이름
 *                 value   : 찾은 값을 담을 출력 버퍼 (호출자가 크기를 보장)
 * Return        : 1  - 성공 (value에 결과 기록)
 *                 0  - 파일은 열렸지만 section 또는 key를 못 찾음
 *                 -1 - 파일을 열 수 없음
 *                 -3 - 라인 파싱 실패 ('=' 없음)
 ******************************************************************************/
int GetEnvValue(char *fname, char *section, char *key, char *value)
{
FILE *fp;
char buff[1024], bkey[512];
int   fg = 0, rtn;
register char  *p;
	fp = fopen(fname, "rt");
	if(!fp) return -1;
	fseek(fp, 0, SEEK_SET);
	/* Seek to Section */
	memset(buff, 0x00, 1024);
	while(fgets(buff, 1024, fp)){
		if(*buff != '['){memset(buff, 0x00, 1024); continue;}
		
		else if(memcmp(buff + 1, section, strlen(section))){
			memset(buff, 0x00, 1024);
			continue;
		}
		if(*(buff +  strlen(section) + 1) == ']'){fg = 1; break;}
	}
	if(!fg) {fclose(fp); return 0;} // Not found 
	/* Find  Key */
	while(fgets(buff, 1024, fp)){
		if(*buff == '['){ // New Section --> Not Found
			fg = 0; break;
		}
		if(*buff == ' ' || *buff == '#' || *buff == '\n'){
			memset(buff, 0x00, 1024);
			continue;
		}
		p = (char *)strchr(buff, (int)'\n');
		if(p) *p = 0x00;
		p = buff + strlen(buff);
		while(1){
			if(*p == '\t' || *p == 0x00 || *p == ' ')*p = 0x00;
			else break;
			p--;
		}
		rtn = CutKey(buff, bkey);
		if(!rtn){
			fclose(fp); 
			return -3;
		} // KEY ERROR
		if(strlen(bkey) != strlen(key)){
			memset(buff, 0x00, 1024);
			continue;
		}
		if(strncmp(bkey, key, strlen(bkey))){
			memset(buff, 0x00, 1024);
			continue;
		}
		fg = 2; // Find
		break;
	}
	fclose(fp);
	if(fg != 2){
		 return 0;
	}
	p = (char *)strchr(buff, (int)'#');
	if(p)*p = 0x00;
	else p = buff + strlen(buff);
	/* Cut White 'char' at Right side */ 
	while(1){
		if(*p == '\t' || *p == 0x00 || *p == ' ')*p = 0x00;
		else break;
		p--;
	}
	p = (char *)strchr(buff, (int)'=');
	if(!p)return -3;
	/* Cut White 'char' at Left side */
	p++;
	while(1){
		if(*p == ' ' || *p == '\t' || *p == 0x00) p++;
		else break;
	}
	memcpy(value, p, strlen(p));
	return 1;
}
