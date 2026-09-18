/// 
/// zmq logout (thread Version)
///   -DLOGOUT_TYPE=11
/// @file zmqlogout_thr.c
/// @date 2022. 09. 27. (화) 10:00:46 KST
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


extern int   SendTopicMessage0mq(void *, char *, char *, int);
extern void *OpenPubSocket0mq(void *, char *);
////////////////////////////////////////////////////////////////////////////////
// Description : Initialize the zmq-based logging subsystem for the calling
//               (main) thread, allocating a private LOGENV. Reads LOG_PUBDEV
//               (zmq PUB endpoint) and LOG_HOME (log directory) from the
//               environment, opens the log PUB socket, and publishes an
//               initial 'C'(reate) control record with the process's
//               command-line arguments. Unlike zmqlogout.c's single-thread
//               version, the LOGENV handle is returned to the caller
//               instead of kept in a static, so each thread can hold its
//               own (see InitLogoutThread0mq() for worker threads).
// Prototype   : void *_TInitLogout0mq(void *ctx, int arc, char **arv,
//                   char *lname, int valid)
// Arguments   : void  *ctx   : zmq context
//               int    arc   : argc, count of arv
//               char **arv   : argv, process command-line arguments
//               char  *lname : logfilename : logical log name
//               int    valid : 보관일(로그 보관 기간, 일 단위)
// Return      : 성공 : LOGENV * (opaque handle, pass to the other _T*
//                       functions), 실패(lname NULL, calloc 실패,
//                       LOG_PUBDEV/LOG_HOME 미설정, 소켓 open 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *_TInitLogout0mq(void *ctx, int arc, char **arv, char *lname, int valid)
{
char *ep;
int   rtn, i, len;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
int   pid;
LOGENV *env;

	if(lname == NULL)return NULL;
	env = (LOGENV *)calloc(1, sizeof(LOGENV));
	if(!env) return NULL;

	env->Loglevel = 100; // Temp
	pid = (int)getpid();
	env->Pid = pid;
	env->Tid = (int)pthread_self();	
	env->valid = valid;
	env->Context = ctx;
	ep = getenv("LOG_PUBDEV");
	if(!ep)return NULL;
	env->LogSocket = OpenPubSocket0mq(env->Context, ep);
	if(env->LogSocket == NULL)return NULL;

	ep = getenv("LOG_HOME");
	if(!ep)return NULL;
	strcpy(env->Logdir, ep);
	strcpy(env->Logname, lname);

	memset(buff, 0x00, 1024);
	pinit = (LOG_INIT *)buff;
	pinit->mode[0] = 'C';

	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy((time_t *)&pinit->nanotm, &tm, sizeof(struct timespec));

	memcpy(pinit->lname, env->Logname, strlen(env->Logname));
	memcpy(pinit->dir, ep, strlen(ep));
	pinit->validdt = valid;
	pinit->pid = env->Pid;
	p = buff + sizeof(LOG_INIT);
	for(i = 0; i < arc; i++){
		sprintf(tmp, "(%d:%s) ", i, arv[i]);
		strcat(p, tmp);
	}
	len = strlen(p);

	SendTopicMessage0mq(env->LogSocket, "LOGOUT", buff,
			sizeof(LOG_INIT) + len);
	return (void *)env;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Thread 용 InitLogout. Clones the log destination/session
//               fields (Context, Logdir, Logname, valid, Loglevel) from an
//               existing LOGENV (typically the one returned by
//               _TInitLogout0mq() in the main thread), opens its own log
//               PUB socket, tags Pid/Tid with pthread_self(), and publishes
//               a "THREAD START TID(...)" control record.
// Prototype   : void *InitLogoutThread0mq(void *old)
// Arguments   : void *old ; main thread 의 LOGENV (_TInitLogout0mq()의 반환값)
// Return      : 성공 : 새 LOGENV * (이 thread 전용 handle), 실패(calloc 실패,
//                       LOG_PUBDEV 미설정, 소켓 open 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *InitLogoutThread0mq(void *old)
{
char *ep;
int   rtn, i, len;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
int   pid;
LOGENV *env, *pold;

	pold = (LOGENV *)old;
	env = (LOGENV *)calloc(1, sizeof(LOGENV));
	if(!env) return NULL;

	env->Loglevel = pold->Loglevel; // Temp
	env->Tid      = (int)pthread_self();	
	env->Pid      = env->Tid;
	env->valid    = pold->valid;
	env->Context  = pold->Context;
	ep = getenv("LOG_PUBDEV");
	if(!ep)return NULL;
	env->LogSocket = OpenPubSocket0mq(env->Context, ep);
	if(env->LogSocket == NULL)return NULL;

	memcpy(env->Logdir, pold->Logdir, strlen(pold->Logdir));
	memcpy(env->Logname, pold->Logname, strlen(pold->Logname));

	memset(buff, 0x00, 1024);
	pinit = (LOG_INIT *)buff;
	pinit->mode[0] = 'C';

	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy((time_t *)&pinit->nanotm, &tm, sizeof(struct timespec));

	memcpy(pinit->lname, env->Logname, strlen(env->Logname));
	memcpy(pinit->dir, pold->Logdir, strlen(pold->Logdir));
	pinit->validdt  = env->valid;
	pinit->pid      = env->Pid;
	p = buff + sizeof(LOG_INIT);
	sprintf(p, "THREAD START TID(%u)-----------" , env->Tid);
	len = strlen(p);
	SendTopicMessage0mq(env->LogSocket, "LOGOUT", buff,
			sizeof(LOG_INIT) + len);
	return (void *)env;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Format one log record (timestamp, log name, source
//               file/func/line, pid/tid, printf-style message) and publish
//               it on topic "LOGOUT" over the log PUB socket held in the
//               LOGENV handle `ep`. Thread-safe variant of _XLogout0mq():
//               state is passed explicitly instead of read from a static.
//               Intended to be called through a logging macro that
//               supplies file/func/line via __FILE__/__func__/__LINE__.
// Prototype   : void _TLogout0mq(void *ep, char mode, const char *file,
//                   const char *func, const int line, const char *fmt, ...)
// Arguments   : void       *ep   : LOGENV * from _TInitLogout0mq()/
//                                  InitLogoutThread0mq()
//               char        mode : log record mode/severity tag (1 char)
//               const char *file : source file name (__FILE__)
//               const char *func : source function name (__func__)
//               const int   line : source line number (__LINE__)
//               const char *fmt  : printf-style format string, plus args
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void _TLogout0mq(void *ep,
char mode, const char *file, const char *func,const int line, const char *fmt, ...)
{
char buff[SZ_MAXLOG];
char        *dp;
LOG_DATAHD  *dthd;
va_list      args;
int          len, rtn;
struct timespec tm;
LOGENV *env;
	env = (LOGENV *)ep;

	dthd = (LOG_DATAHD *)buff;
	memset(buff, 0x00, sizeof(LOG_DATAHD));
	dthd->mode[0] = mode;
	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy(&dthd->nanotm, &tm, sizeof(struct timespec));
	memcpy(dthd->lname, env->Logname, strlen(env->Logname));
	memcpy(dthd->fname, file, strlen(file));
	memcpy(dthd->funname, func, strlen(func));
	dthd->line = line;
	dthd->pid  = env->Pid;
	dthd->tid  = env->Tid;

	dp = buff + sizeof(LOG_DATAHD);
	va_start(args, fmt);
	len = vsnprintf(dp, SZ_MAXLOG - sizeof(LOG_DATAHD),fmt, args);
	va_end(args);
	dthd->dtlen = len;
	SendTopicMessage0mq(env->LogSocket, "LOGOUT", buff,
			(int)(sizeof(LOG_DATAHD) + len) );
}

////////////////////////////////////////////////////////////////////////////////
// Description : Close the log PUB socket held by this thread's LOGENV
//               handle, if any. Does not free the LOGENV struct itself.
// Prototype   : void _TCloseLogout0mq(void *ep)
// Arguments   : void *ep : LOGENV * from _TInitLogout0mq()/
//                          InitLogoutThread0mq()
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void _TCloseLogout0mq(void *ep)
{
LOGENV *env;
	env = (LOGENV *)ep;
	if(env->LogSocket) zmq_close(env->LogSocket);
}

////////////////////////////////////////////////////////////////////////////////
// 로그 파일 날짜 변경(rollover)을 알리는 control 레코드를 전송한다
/// @fn     void _TChangeDateLogout0mq(void *evp)
/// @brief  Publish a 'C'(reate) control record with a "CHANGE DATE--"
///         marker on topic "LOGOUT", telling the log collector to roll
///         over to a new day's log file. Thread-safe variant of
///         _XChangeDateLogout0mq(): uses the passed-in LOGENV handle
///         instead of a static.
/// @param  evp LOGENV * from _TInitLogout0mq()/InitLogoutThread0mq()
/// @return void
////////////////////////////////////////////////////////////////////////////////
void _TChangeDateLogout0mq(void *evp)
{
char *ep;
int   rtn, i, len;
char  buff[1024], tmp[255], *p;
LOG_INIT *pinit;
struct timespec tm;
int   pid;
LOGENV *env;

	env = (LOGENV *)evp;

	memset(buff, 0x00, 1024);
	pinit = (LOG_INIT *)buff;
	pinit->mode[0] = 'C';

	clock_gettime(CLOCK_REALTIME, &tm);
	memcpy((time_t *)&pinit->nanotm, &tm, sizeof(struct timespec));

	memcpy(pinit->lname, env->Logname, strlen(env->Logname));
	memcpy(pinit->dir, env->Logdir,  strlen(env->Logdir));
	pinit->validdt = env->valid;
	pinit->pid = env->Pid;
	p = buff + sizeof(LOG_INIT);
	strcpy(p, "CHANGE DATE--");
	len = strlen(p);

	SendTopicMessage0mq(env->LogSocket, "LOGOUT", buff,
			sizeof(LOG_INIT) + len);
}

