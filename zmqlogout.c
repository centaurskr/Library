///
///
/// 0MQ를 사용하는 logout(single thread  용)
///    -DLOGOUT_TYPE=1
/// @file zmqlogout.c
/// @date 2022. 07. 06. (수) 17:46:11 KST
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
#include <zmq.h>
#include "TbData.h"

typedef struct _LOGENV{
   	int     Loglevel;
    char    Logname [80];
    char    Logdir  [80];
    void   *LogSocket;
	void   *Context;
	int     valid;
	int     Pid, Tid;
}LOGENV;

static LOGENV LogEnv;
extern int   SendTopicMessage0mq(void *, char *, char *, int);
extern void *OpenPubSocket0mq(void *, char *);

////////////////////////////////////////////////////////////////////////////////
// Description : Format one log record (timestamp, log name, source
//               file/func/line, pid/tid, printf-style message) and publish
//               it on topic "LOGOUT" over the log PUB socket opened by
//               _XInitLogout0mq(). Intended to be called through a logging
//               macro that supplies file/func/line via __FILE__/__func__/
//               __LINE__.
// Prototype   : void _XLogout0mq(char mode, const char *file,
//                   const char *func, const int line, const char *fmt, ...)
// Arguments   : char        mode : log record mode/severity tag (1 char)
//               const char *file : source file name (__FILE__)
//               const char *func : source function name (__func__)
//               const int   line : source line number (__LINE__)
//               const char *fmt  : printf-style format string, plus args
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void _XLogout0mq(
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
	dthd->dtlen = len;
	SendTopicMessage0mq(LogEnv.LogSocket, "LOGOUT", buff,
			(int)(sizeof(LOG_DATAHD) + len) );
}
////////////////////////////////////////////////////////////////////////////////
// Description : Initialize the zmq-based logging subsystem for this process.
//               Reads the LOG_PUBDEV (zmq PUB endpoint) and LOG_HOME
//               (log directory) environment variables, opens the log PUB
//               socket, stores the session state in the static LogEnv, and
//               publishes an initial 'C'(reate) control record containing
//               the process's command-line arguments.
// Prototype   : int _XInitLogout0mq(void *ctx, int arc, char **arv,
//                   char *lname, int valid)
// Arguments   : void  *ctx   : zmq context
//               int    arc   : argc, count of arv
//               char **arv   : argv, process command-line arguments
//               char  *lname : logfilename : logical log name
//               int    valid : 보관일(로그 보관 기간, 일 단위)
// Return      : 1 : 성공
//               -1000 : lname이 NULL
//               -1002 : LOG_PUBDEV 환경변수 미설정
//               -2001 : 로그 PUB 소켓 open 실패
//               -3001 : LOG_HOME 환경변수 미설정
////////////////////////////////////////////////////////////////////////////////
int _XInitLogout0mq(void *ctx, int arc, char **arv, char *lname, int valid)
{
char *ep;
int   rtn, i, len;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
int   pid;

	if(lname == NULL)return -1000;
	LogEnv.Loglevel = 100; // Temp
	pid = getpid();
	LogEnv.Pid = pid;
	LogEnv.Tid = (int)pthread_self();	
	LogEnv.valid = valid;
	LogEnv.Context = ctx;
	ep = getenv("LOG_PUBDEV");
	if(!ep)return -1002;
	LogEnv.LogSocket = OpenPubSocket0mq(LogEnv.Context, ep);
	if(LogEnv.LogSocket == NULL)return -2001;

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

	SendTopicMessage0mq(LogEnv.LogSocket, "LOGOUT", buff,
			sizeof(LOG_INIT) + len);
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// Description : Close the log PUB socket opened by _XInitLogout0mq(), if any.
// Prototype   : void _XCloseLogout0mq(void)
// Arguments   : void
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void _XCloseLogout0mq()
{
	if(LogEnv.LogSocket) zmq_close(LogEnv.LogSocket);
}

////////////////////////////////////////////////////////////////////////////////
// 로그 파일 날짜 변경(rollover)을 알리는 control 레코드를 전송한다
/// @fn     void _XChangeDateLogout0mq(void)
/// @brief  Publish a 'C'(reate) control record with a "CHANGE DATE--"
///         marker on topic "LOGOUT", telling the log collector to roll
///         over to a new day's log file. Uses the LogEnv state already
///         populated by _XInitLogout0mq().
/// @param  void
/// @return void
////////////////////////////////////////////////////////////////////////////////
void _XChangeDateLogout0mq()
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

	SendTopicMessage0mq(LogEnv.LogSocket, "LOGOUT", buff,
			sizeof(LOG_INIT) + len);
}

