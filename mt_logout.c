///
/// Logout Multicasting Version
///      -DLOGOUT_TYPE=2
/// @file mt_logout.c
/// @date 2023. 12. 28. (목) 17:01:19 KST
/// @author Cento 
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdarg.h>
#include <netdb.h>
#include <pthread.h>
#include <unistd.h>

#include "TbData.h"

typedef struct _LOGENV{
   	int     Loglevel;
    char    Logname [80];
    char    Logdir  [80];
    int     Logfd;
	int     valid;
	int     Pid, Tid;
    struct  sockaddr_in Logaddr;
}LOGENV;

extern int OpenMtPublish();
extern int GetPortNumberR(char *, char *);

static LOGENV LogEnv;
////////////////////////////////////////////////////////////////////////////////
// Description : 로그 한 건을 LOG_DATAHD 헤더 + printf 스타일 메시지 형태로
//               UDP 멀티캐스트(LogEnv.Logaddr)로 전송한다. Logout() 매크로가
//               __FILE__/__FUNCTION__/__LINE__을 채워 내부적으로 호출한다.
//               _MtInitLogout()으로 먼저 초기화되어 있어야 한다.
// Prototype   : void _MtLogout(char mode, const char *file, const char *func,
//                               const int line, const char *fmt, ...)
// Arguments   : mode : 로그 모드 문자 (C:생성, I:Info, D:Debug, E:Error, J:Journal)
//               file : 소스 파일명 (보통 __FILE__)
//               func : 함수명 (보통 __FUNCTION__)
//               line : 라인 번호 (보통 __LINE__)
//               fmt  : printf 스타일 포맷 스트링, 이후 가변인자
// Return      : void (전송 실패 여부는 반환하지 않는다)
////////////////////////////////////////////////////////////////////////////////
void _MtLogout(
char mode, const char *file, const char *func,const int line, const char *fmt, ...)
{
char buff[SZ_MAXLOG];
char        *dp;
LOG_DATAHD  *dthd;
va_list      args;
int          len, rtn;
struct timespec tm;

	dthd = (LOG_DATAHD *)buff;
	memset(buff, 0x00, sizeof(LOG_DATAHD));
	dthd->mode[0] = mode;
	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy(&dthd->nanotm, &tm, sizeof(struct timespec));
	memcpy(dthd->lname, LogEnv.Logname, strlen(LogEnv.Logname));
	memcpy(dthd->fname, file, strlen(file));
	memcpy(dthd->funname, func, strlen(func));
	dthd->line = line;
	dthd->pid  = LogEnv.Pid;
	dthd->tid  = LogEnv.Tid;

	dp = buff + sizeof(LOG_DATAHD);
	va_start(args, fmt);
	len = vsnprintf(dp, SZ_MAXLOG - sizeof(LOG_DATAHD),fmt, args);
	va_end(args);
	dthd->dtlen  = len;
	sendto(LogEnv.Logfd, buff, (int)(sizeof(LOG_DATAHD) + len) ,0,
		(struct sockaddr *)&LogEnv.Logaddr, sizeof(struct sockaddr));
}

////////////////////////////////////////////////////////////////////////////////
// Description : Multicasting 로그 시스템을 초기화한다. 환경변수 LOG_GROUP
//               (멀티캐스트 그룹)과 LOG_HOME(로그 디렉토리), 그리고
//               "tblog/udp" 서비스 포트를 읽어 발행 소켓을 열고, 프로그램
//               시작을 알리는 'C' 모드 초기화 레코드를 전송한다.
//               InitLogout() 매크로로 감싸 호출된다.
// Prototype   : int _MtInitLogout(int arc, char **arv, char *lname, int valid)
// Arguments   : arc   : argc (전송되는 초기화 레코드에 커맨드라인 인자를 기록)
//               arv   : argv
//               lname : 로그 파일(논리) 이름
//               valid : 로그 보관일수
// Return      : 1     성공
//               -1000 lname이 NULL
//               -1001 "tblog/udp" 포트 조회 실패
//               -1002 환경변수 LOG_GROUP 없음
//               -2001 OpenMtPublish() 소켓 open 실패
//               -3001 환경변수 LOG_HOME 없음
////////////////////////////////////////////////////////////////////////////////
int _MtInitLogout(int arc, char **arv, char *lname, int valid)
{
char *ep;
int   rtn, i, len, port;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
	if(lname == NULL)return -1000;
	LogEnv.Loglevel = 100; // Temp
	LogEnv.Pid = getpid();
	LogEnv.Tid = (int)pthread_self();	
	LogEnv.valid = valid;

	port = GetPortNumberR("tblog", "udp");
	if(port < 0)return -1001;

	ep = getenv("LOG_GROUP");
	if(!ep)return -1002;

	LogEnv.Logfd = OpenMtPublish(&LogEnv.Logaddr, "lo", ep, port, 0);
	if(LogEnv.Logfd < 0)return -2001;

	ep = getenv("LOG_HOME");
	if(!ep)return -3001;
	strcpy(LogEnv.Logdir, ep);
	strcpy(LogEnv.Logname, lname);

	memset(buff, 0x00, 1024);
	pinit = (LOG_INIT *)buff;
	pinit->mode[0] = 'C';

	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy((time_t *)&pinit->nanotm, &tm, sizeof(struct timespec));

	memcpy(pinit->lname, LogEnv.Logname, strlen(LogEnv.Logname));
	memcpy(pinit->dir, ep, strlen(ep));
	pinit->validdt = valid;
	pinit->pid = LogEnv.Pid;
	p = buff + sizeof(LOG_INIT);
	for(i = 0; i < arc; i++){
		sprintf(tmp, "(%d:%s) ", i, arv[i]);
		strcat(p, tmp);
	}
	len = strlen(p);
	sendto(LogEnv.Logfd, buff, sizeof(LOG_INIT) + len, 0, 
		(struct sockaddr *)&LogEnv.Logaddr, sizeof(struct sockaddr));
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Description : 로그용 소켓(LogEnv.Logfd)을 닫는다. CloseLogout() 매크로로
//               감싸 호출된다.
//               주의: TbLogout.h의 extern 선언은 _MtCloseLogout0mq()이지만
//               CloseLogout() 매크로와 본 구현은 _MtCloseLogout()(접미사
//               없음)을 쓴다 — 헤더와 구현의 이름이 어긋나 있다.
// Prototype   : void _MtCloseLogout(void)
// Arguments   : 없음
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void _MtCloseLogout()
{
	if(LogEnv.Logfd >= 0) close(LogEnv.Logfd);
}
////////////////////////////////////////////////////////////////////////////////
/// Log 일자를 변경한다 (날짜가 바뀌었음을 알리는 'C' 모드 레코드를 다시
/// 전송한다). ChangeDateLogout() 매크로로 감싸 호출된다.
/// 주의: TbLogout.h의 extern 선언은 _MtChangeDateLogout0mq()이지만
/// ChangeDateLogout() 매크로와 본 구현은 _MtChangeDateLogout()(접미사
/// 없음)을 쓴다 — 헤더와 구현의 이름이 어긋나 있다.
/// @fn      void _MtChangeDateLogout()
/// @param   없음
/// @return  없음
////////////////////////////////////////////////////////////////////////////////
void _MtChangeDateLogout()
{
char *ep;
int   rtn, i, len;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
int   pid;


    memset(buff, 0x00, 1024);
    pinit = (LOG_INIT *)buff;
    pinit->mode[0] = 'C';

    clock_gettime(CLOCK_REALTIME, &tm);
    memcpy((time_t *)&pinit->nanotm, &tm, sizeof(struct timespec));

    memcpy(pinit->lname, LogEnv.Logname, strlen(LogEnv.Logname));
    memcpy(pinit->dir, LogEnv.Logdir,  strlen(LogEnv.Logdir));
    pinit->validdt = LogEnv.valid;
    pinit->pid = LogEnv.Pid;
    p = buff + sizeof(LOG_INIT);
    strcpy(p, "CHANGE DATE--");
    len = strlen(p);
	sendto(LogEnv.Logfd, buff, sizeof(LOG_INIT) + len, 0, 
		(struct sockaddr *)&LogEnv.Logaddr, sizeof(struct sockaddr));
}

