///
/// Stand alone logout
///    -DLOGOUT_TYPE=0
/// @file st_logout.c
/// @date 2023. 12. 28. (목) 15:10:44 KST
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
#include <netinet/in.h>
#include <netdb.h>
#include <stdarg.h>
#include <sys/ioctl.h>


extern int errno;


static char Lname[32];
static FILE *Logfp;
static char Path[512];
static char Pattern[32];
static int  Dcnt;

extern int CleanLogfile(char *, char *, int);

///////////////////////////////////////////////////////////////////////////////
// Description : 현재 열려 있는 로그 파일(Logfp)을 flush 없이 닫는다.
//               CloseLogout() 매크로로 감싸 호출된다.
// Prototype   : void _SCloseLogout(void)
// Arguments   : 없음
// Return      : void
///////////////////////////////////////////////////////////////////////////////
void _SCloseLogout()
{
	if(Logfp){
		fclose(Logfp);
		Logfp = NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////
// Description : Stand-alone(파일 기반) 로그 시스템을 초기화한다. 환경변수
//               LOG_HOME(없으면 "/tmp")에 "<lname>_MMDD.LOG" 형식의 로그
//               파일을 열고, CleanLogfile()로 dcnt일 이상 지난 기존 로그
//               파일(패턴: "<lname>_*.LOG")을 정리한 뒤, 시작 헤더와
//               커맨드라인 인자를 기록한다. InitLogout() 매크로로 감싸
//               호출된다.
// Prototype   : void _SInitLogout(int largc, char **largv, char *lname, int dcnt)
// Arguments   : largc : argc (로그 파일에 커맨드라인 인자를 기록)
//               largv : argv
//               lname : 로그 파일(논리) 이름, 파일명 접두사로 쓰임
//               dcnt  : 로그 파일 보관일수 (CleanLogfile()에 전달)
// Return      : void (로그 파일 open 실패 시 Logfp는 NULL로 남고 이후
//               _SLogout()은 조용히 무시된다)
///////////////////////////////////////////////////////////////////////////////
void _SInitLogout(int largc, char **largv, char *lname, int dcnt)
{
char tmp[255], buff[512], path[512], logfilename[1024];
char *ep;
int     i;
struct timespec tm;
struct tm newtime;

	if(Logfp)fclose(Logfp);

	memset(Path, 0x00, 512);
	ep=getenv("LOG_HOME");
	if(!ep)strcpy(Path, "/tmp");
	else strcpy(Path, ep);
	sprintf(Pattern, "%s_*.LOG", lname);
	Dcnt = dcnt;
	i = CleanLogfile(Path, Pattern, Dcnt);
	memset(Lname, 0x00, 32);
	memcpy(Lname, lname, strlen(lname));

	clock_gettime(CLOCK_REALTIME, &tm);
	localtime_r((time_t *)&tm.tv_sec, &newtime);
	strftime(tmp, 10, "%m%d", &newtime);
	sprintf(logfilename, "%s/%s_%.4s.LOG", Path, Lname, tmp);

//printf("Logfile : %s\n", logfilename);

	Logfp = fopen(logfilename, "at+");
	if(!Logfp) return;
	strftime(tmp, 12, "%T", &newtime);
	sprintf(buff, "----- START -----[%s:%d] [%.8s]\n", lname, getpid(), tmp);
	fwrite(buff, strlen(buff) , 1, Logfp);
	fwrite("ARGUMENTS ", 10, 1, Logfp);
	for(i = 1; i < largc; i++) fprintf(Logfp, "[%2d][%s] ", i, largv[i]);
	fprintf(Logfp, "\n");
	fprintf(Logfp, "-----------------------------------------------------\n");
	fflush(Logfp);

	return;
}
///////////////////////////////////////////////////////////////////////////////
// Description : 로그 한 줄을 "[HH:MM:SS:ms]-mode] message-(file:func-line)"
//               형식으로 파일에 append하고 즉시 flush한다. _SInitLogout()으로
//               로그 파일이 열려 있지 않으면 아무 것도 하지 않는다. Logout()
//               매크로가 __FILE__/__FUNCTION__/__LINE__을 채워 호출한다.
// Prototype   : void _SLogout(char mode, const char *fname, const char *func,
//                              int line, const char *fmt, ...)
// Arguments   : mode  : 로그 모드 문자 (C:생성, I:Info, D:Debug, E:Error, J:Journal)
//               fname : 소스 파일명 (보통 __FILE__)
//               func  : 함수명 (보통 __FUNCTION__)
//               line  : 라인 번호 (보통 __LINE__)
//               fmt   : printf 스타일 포맷 스트링, 이후 가변인자
// Return      : void
///////////////////////////////////////////////////////////////////////////////
void _SLogout(char mode,
const char *fname, 
const char *func, int line, 
const char *fmt, ...)
{
va_list ap;
char buff[5120], tmp[32];
time_t  tl;
struct timespec tv;
int    len;
	if(Logfp){
		memset(tmp, 0x00, 32);
		clock_gettime(CLOCK_REALTIME, &tv);
		strftime(tmp , 20, "[%H:%M:%S:", localtime(&tv.tv_sec));
		fprintf(Logfp, "%s%d]-%c] ", tmp, (int)(tv.tv_nsec/1000000),mode);

		memset(buff, 0x00, 5120);
		va_start(ap, fmt);
		vsprintf(buff, fmt, ap);
		va_end(ap);

		len = strlen(buff);
		if(*(buff + len -1 ) == '\n') *(buff + len -1) = 0x00;

		fprintf(Logfp, "%s-(%s:%s-%d)\n", buff, fname, func, line);

		fflush(Logfp);
	}
	return ;
}
///////////////////////////////////////////////////////////////////////////////
// Description : 날짜가 바뀌었을 때 현재 로그 파일을 닫고, 오래된 로그를
//               정리(CleanLogfile())한 뒤, 새 날짜의 "<lname>_MMDD.LOG"
//               파일을 열어 일자변경 헤더를 기록한다. ChangeDateLogout()
//               매크로로 감싸 호출된다.
// Prototype   : void _SChangeDateLogout(void)
// Arguments   : 없음
// Return      : void (로그 파일 open 실패 시 Logfp는 NULL로 남는다)
///////////////////////////////////////////////////////////////////////////////
void _SChangeDateLogout()
{
char tmp[255], logfilename[1024];
struct timespec tm;
struct tm newtime;


	if(Logfp){
		fflush(Logfp);
		fclose(Logfp);
	}

	CleanLogfile(Path, Pattern, Dcnt);

	clock_gettime(CLOCK_REALTIME, &tm);
	localtime_r((time_t *)&tm.tv_sec, &newtime);
	strftime(tmp, 10, "%m%d", &newtime);
	sprintf(logfilename, "%s/%s_%.4s.LOG", Path, Lname, tmp);

	Logfp = fopen(logfilename, "at+");
	if(!Logfp) return;

	strftime(tmp, 12, "%T", &newtime);
	fprintf(Logfp, "----- 일자변경 START -----[%s:%d] [%.8s]\n", 
		Lname, getpid(), tmp);
	fprintf(Logfp, "-----------------------------------------------------\n");
	fflush(Logfp);

	return;
}
