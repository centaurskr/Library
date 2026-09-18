//
// Description : Event using epoll
// File Name   : tbevent.cc
// Date        : 2017. 07. 06. (목) 15:52:26 KST
// By          : centaurskr@gmail.com
//
#include <iostream>
#include <map>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <sys/epoll.h>

using namespace std;

#define MAX_EVENT_ID 50000000
#define MAX_EVENT_PL 4096

typedef int (*Handler)(void *, int, int, void *, int);
typedef int (*DefHandler)(void *, void *, int);

typedef struct {
	struct timespec stm;
	int      fd;
	int      event_id;
	int      tmcnt;     // time counter for checking timeout
	int      tout, count;
	bool     report;
	void    *data;
	int      dtsize;
	Handler  handler;
}EventList;
typedef map<int, EventList *> EVENT_MAP;

typedef struct {
		struct timespec stm;
		int        eplfd;
		int        eidSeq;
		int        tout;       // timeout value (milliseconds, 1/1,000)
		bool       run;
		EVENT_MAP  list;
		void      *defdata;   // Default Handler data
		int        defdtsz; // Default Handler data size
		DefHandler defHandler;	
}TBEvent;

////////////////////////////////////////////////////////////////////////////////
// Description : EventLoop. epoll_wait()으로 대기하다 ready된 fd들의
//               handler를 호출하고, epoll_wait()이 이벤트 없이 반환했을
//               때(ecnt==0)만 DefaultHandler를 호출한다. 매 반복마다 경과
//               시간(difsec)만큼 report==true인 이벤트들의 tmcnt를 줄여
//               타임아웃(0 이하)이 되면 event_id를 음수로 넘겨 handler를
//               호출한다. event->run이 false가 되면 루프를 빠져나오며 모든
//               등록된 이벤트를 delete하고 반환한다.
// Prototype   : void TB_RunEventLoop(void *ent)
// Arguments   : ent : TB_InitEvent()가 반환한 이벤트 컨텍스트
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void TB_RunEventLoop(void *ent)
{
	TBEvent *event     = (TBEvent *)ent;
	int          ecnt, difsec;
	register int i;
	struct epoll_event ev[MAX_EVENT_PL];
	EventList   *einfo;
	time_t       basetm, currtm;

	time(&basetm);
	while(event->run){
		ecnt = epoll_wait(event->eplfd, ev, MAX_EVENT_PL, event->tout);
		if(ecnt < 0) continue;
		for(i = 0; i < ecnt; i++){
			einfo = (EventList *)ev[i].data.ptr;
			einfo->count ++; // Called counter
			einfo->handler(ent, einfo->fd, einfo->event_id, 
			               einfo->data, einfo->dtsize);
			if(event->run == false) break; // Maybe shutdown
		}

		if(ecnt == 0 && event->defHandler != NULL)
				event->defHandler(ent, event->defdata, event->defdtsz);

		// CHECK TIMEOUT
		time(&currtm);
		difsec = (int)difftime(currtm, basetm);
		basetm = currtm;
		for(auto it = event->list.begin(); it != event->list.end(); ++it){
			if(it->second->report == false) continue;
			it->second->tmcnt -= difsec;
			if(it->second->tmcnt <= 0){
				it->second->count ++;
				it->second->handler(
					ent, 
					it->second->fd, 
					it->second->event_id * -1, 
					it->second->data, it->second->dtsize);
				it->second->tmcnt = it->second->tout;
				if(event->run == false) break; // Maybe shutdown
			}
				
		}
	}
	// Clean
	for(auto it = event->list.begin();it != event->list.end(); ++it){
		delete it->second;
	}
	event->list.clear();
	return ;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Default Handler(이벤트 없이 epoll_wait()이 반환했을 때
//               호출되는 handler)를 등록/교체한다.
// Prototype   : int TB_ChangeDefaultHandler(void *ent, void *dt, int dtlen,
//                                           DefHandler handler)
// Arguments   : void *ent   : 이벤트 컨텍스트
//               void *dt    : user data
//               int   dtlen : user data len
//               DefHandler handler : 새 Default Handler
// Return      : 1:SUCESS (현재 구현은 항상 1을 반환, -1 케이스는 없음)
////////////////////////////////////////////////////////////////////////////////
int TB_ChangeDefaultHandler(void *ent, void *dt, int dtlen, DefHandler handler)
{
	TBEvent *event     = (TBEvent *)ent;
	event->defdata     = dt;
	event->defdtsz     = dtlen;
	event->defHandler  = handler;
	return 1;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Change epoll_wait() Time out value
// Prototype   : void TB_ChangeEventTimeout(void *ent, int msec)
// Arguments   : void *ent : 이벤트 컨텍스트
//               int  msec : epoll_wait() timeout (milliseconds, 1/1,000초)
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void TB_ChangeEventTimeout(void *ent, int msec)
{
	TBEvent *event = (TBEvent *)ent;
	event->tout  = msec;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Shutdown Event. run 플래그를 false로 만들어
//               TB_RunEventLoop()가 다음 반복에서 루프를 빠져나오게 한다.
// Prototype   : void TB_ShutdownEvent(void *ent)
// Arguments   : ent : 이벤트 컨텍스트
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void TB_ShutdownEvent(void *ent)
{
	TBEvent *event = (TBEvent *)ent;
	event->run = false;
	return ;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Delete Event. epoll 감시 목록에서 fd를 제거(EPOLL_CTL_DEL)
//               하고, 등록되어 있던 EventList를 delete한다.
//               주의: fd가 event->list에 없어도(find()가 end()를 반환해도)
//               이를 확인하지 않고 곧바로 it->second를 역참조하므로,
//               등록되지 않은 fd로 호출하면 정의되지 않은 동작(크래시
//               가능)이 발생한다. 반드시 TB_AddEvent()로 등록한 fd에
//               대해서만 호출해야 한다.
// Prototype   : void TB_DeleteEvent(void *ent, int fd)
// Arguments   : ent : 이벤트 컨텍스트
//               fd  : 삭제할 event fd (TB_AddEvent()로 등록된 것이어야 함)
// Return      : void (epoll_ctl 실패 시에도 별도 표시 없이 조용히 반환)
////////////////////////////////////////////////////////////////////////////////
void TB_DeleteEvent(void *ent, int fd)
{
	TBEvent *event = (TBEvent *)ent;
	struct epoll_event epl;
	int                rtn;

	epl.data.fd = fd;
	epl.events  = EPOLLIN;
	rtn = epoll_ctl(event->eplfd, EPOLL_CTL_DEL, fd, &epl);
	if(rtn)  return ; // epoll_ctl error
	auto it = event->list.find(fd);
	delete it->second;
	event->list.erase(it);
	return;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Add Event. epoll 감시 목록(EPOLL_CTL_ADD)과 내부
//               event->list(fd -> EventList map) 양쪽에 등록한다.
// Prototype   : int TB_AddEvent(void *ent, int fd, int tout, bool tfg,
//                              void *data, int dtlen, Handler handler)
// Arguments   :  void *ent : 이벤트 컨텍스트
//                int fd   : event FD(I)
//                int tout : Timeout value(sec)
//                bool tfg : false:do not need timeout report, 1: need report
//                void *data : data for handler
//                int   dtlen : data length
//                Handler handler : Event Handler
// Return      :  Error occur : -1(EventList 할당 실패), -2(epoll_ctl 실패);
//                SUCCESS : event identifier (내부 순번, MAX_EVENT_ID 내 순환)
////////////////////////////////////////////////////////////////////////////////
int TB_AddEvent(void *ent, int fd, int tout, bool tfg, void *data, int dtlen,
                Handler  handler)
{
	struct epoll_event  epl;
	int                 rtn;
	TBEvent *event  = (TBEvent *)ent;
	EventList *list = new EventList;
	if(!list) return -1; // NOTE: 표준 new는 실패 시 NULL 대신 예외(bad_alloc)를
	                      // 던지므로 이 체크는 실질적으로 도달하지 않는다.

	clock_gettime(CLOCK_REALTIME, &list->stm);
	list->data     = data;
	list->dtsize   = dtlen;
	list->event_id = event->eidSeq;
	event->eidSeq ++;
	event->eidSeq  = event->eidSeq % MAX_EVENT_ID;
	list->fd       = fd;
	list->tout     = tout;
	list->tmcnt    = tout;
	list->report   = tfg;
	list->count    = 0;
	list->handler  = handler;

	epl.data.fd    = fd;
	epl.events     = EPOLLIN;
	epl.data.ptr   = (void *)list;
	rtn = epoll_ctl(event->eplfd, EPOLL_CTL_ADD, fd, &epl);
	if(rtn){delete list; return -2;}

	event->list[fd] = list;

	return list->event_id;
}
////////////////////////////////////////////////////////////////////////////////
// Description : Create event. TBEvent 컨텍스트를 생성하고 epoll_create()로
//               epoll fd를 만든다.
// Prototype   : void *TB_InitEvent(void)
// Arguments   : 없음
// Return      : 성공 시 이벤트 컨텍스트, epoll_create() 실패 시 NULL.
//               (new TBEvent 자체의 실패는 NULL이 아닌 예외로 나타난다.)
////////////////////////////////////////////////////////////////////////////////
void *TB_InitEvent()
{
	TBEvent *event = new TBEvent;
	if(!event) return NULL;
	clock_gettime(CLOCK_REALTIME, &event->stm);
	event->eidSeq     = 1000;
	event->tout       = 10000; // 10 second
	event->defdata    = NULL;
	event->defdtsz    = 0;
	event->defHandler = NULL;
	event->run        = true;
	event->eplfd      = epoll_create(MAX_EVENT_PL);
	if(event->eplfd < 0){delete event; return NULL;}
	return (void *)event;
}
