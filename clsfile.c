///
/// File을 정리(delete)한다(지정한 일자 만큼 유지)
/// @file clsfile.c
/// @date 2024. 01. 02. (화) 11:42:03 KST
/// @author Cento 
///
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>

/******************************************************************************
* FUNCTION:    _Comparefilename
* DESCRIPTION: File ÀÌ¸§À» ºñ±³.
* PARAMETERS:  char *hstr, *tstr, *fname;
*              hstr  -> ºñ±³ ÇüÅÂÀÇ "*" ÀÌÀü string
*              tstr  -> ºñ±³ ÇüÅÂÀÇ "*" ÀÌÈÄ string
*              fname -> ºñ±³´ë»ó file name
* RETURNED:     1 --> Like that
*               0 --> Unlike that
******************************************************************************/
static int _Comparefilename(char *hstr, char *tstr, char *fname)
{
char *p = NULL;
	if(!strlen(hstr) && !strlen(tstr)) return 0;

	if(!strlen(hstr)){ /* "*" is located in the first of string */
		p = (char *)strstr(fname, tstr);
		if(!p) return 0;
		if(strlen(p) == strlen(tstr)) return 1;
		else return 0;
	}else if(!strlen(tstr)){ /* "*"is located in the last of string */
		if( strncmp(hstr, fname, strlen(hstr)) ) return 0;
		else return 1;
	}else{ /* "*"is located in the middle of string */
		if( strncmp(hstr, fname, strlen(hstr)) ) return 0;

		p = (char *)strstr(fname, tstr);
		if(!p) return 0;
		if(strlen(p) == strlen(tstr)) return 1;
		else return 0;
	}
}

/******************************************************************************
* FUNCTION:    _DiffDate
* DESCRIPTION: ÀÏÀÚ Â÷ÀÌ °è»ê.
* PARAMETERS:  time_t time1, tim_t time2
* RETURNED:    int ; Â÷ÀÌ ÀÏ
*              ÀÏ´ÜÀ§ °è»êÀ¸·Î ´ÙÀ½°ú °°´Ù.
*              time1 > time2  : Plus
*              time1 < time2  : minus
*              time1 == time2 : 0
******************************************************************************/
static int _DiffDate(time_t time1, time_t time2)
{
int  difd, d1, d2;
long difs;
char date1[30], date2[30];
	strftime(date1, 30, "%j", localtime(&time1));
	strftime(date2, 30, "%j", localtime(&time2));
	d1 = atoi(date1); d2 = atoi(date2);
	if(d1 == d2) return 0;
	difd = d1 - d2;
	return difd;
}

/******************************************************************************
* FUNCTION:    CleanLogfile
* DESCRIPTION: pt 디렉토리에서 f 패턴("*"를 하나 포함할 수 있는 와일드카드)에
*              매치되는 파일 중, 마지막 수정일 기준으로 gdate(일)보다 오래된
*              파일을 삭제한다.
* PARAMETERS:  char *pt; logfile path
*              char *f; Type of Logfilename 
*              int   gdate; Number of days of log to retain(Except today)
* RETURN    : Count of deleted file 
******************************************************************************/
int CleanLogfile(char *pt, char *f, int gdate)
{
int            cnt = 0, dif, rtn;
DIR           *dir;
struct dirent *ent;
struct stat    st;
char           fname[1024], hstr[1024], tstr[1024], *namep, *p;
time_t         curr;
	time(&curr);
	dir = opendir(pt);
	if(!dir) return -1;
	memset(hstr, 0x00, 1024); memset(tstr, 0x00, 1024);
	if(*f == '*') strcpy(tstr, f + 1);
	else if( *(f + strlen(f) -1 ) == '*') strncpy(hstr, f, strlen(f) - 1);
	else{
		p = (char *)strchr(f, '*');
		if(!p) strcpy(hstr, f);
		else{ *p= 0x00; strcpy(hstr, f); strcpy(tstr, p + 1); }
	}
	while( (ent = readdir(dir)) != NULL){
		if(!ent->d_ino)
			continue;

		namep = (char *)ent->d_name;
		sprintf(fname, "%s/%s", pt, namep);
		stat(fname, &st);
		if(st.st_mode & S_IFDIR) continue;
		rtn = _Comparefilename(hstr, tstr, namep);
		if(!rtn) continue;
		dif = abs(_DiffDate(curr, st.st_mtime)); //  time of last modification
		if(!gdate && dif != gdate){unlink(fname); cnt++;}
		else if(gdate < dif) {unlink(fname); cnt++;}
	}
	closedir(dir);
	return cnt;
}
///////////////////////////////////////////////////////////////////////////////
