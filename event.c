//
// Description : Select를 사용하는 Event
// File Name   : event.c
// Date        : 2017. 07. 14. (금) 13:21:48 KST
// By          : centaurskr@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/select.h>

typedef struct _EVENT_LIST{
	int                fd;
	int                event_id;
	int                tout, rtime, answer;
	int                del;
	int                (*handler)();
	struct _EVENT_LIST *next;
}EVENT_LIST; 
typedef struct _EVENT{
	fd_set         EventEntry;
	struct timeval Timeout;
	int            Totalcnt;
	int            CurCnt; /* BranchHandler() 에서 Temp 로 사용하는 Count */
						   /* 2003.09.22 추가                             */
						   /* 한 program에서 두번의 AppEventInit시에      */
						   /* 문제점 발견후 추가함.                       */
						   /* _BranchHandler()의 static 변수 이던것을     */
						   /* EVENT 구조체 안으로 편입 시킴.              */
	int            Maxfd;
	int            RUNNING;
	long           Seconds, Usec;
	int            (*DefaultHandler)();
	EVENT_LIST    *Eventlist;
}EVENT;

#define DEF_TIMEOUT    20000
#define START_ID       1000
#define MAX_ID    10000

static int Timedif = 0;
static int Id = 1000;

////////////////////////////////////////////////////////////////////////////////
// Description : 두 메모리 블록을 sz바이트만큼 unsigned char 단위로 비교한다
//               (memcmp()의 부호/크기 비교 결과만 취하는 로컬 구현).
// Prototype   : int MemCmp(void *s1, void *s2, int sz)
// Arguments   : void *s1   : String 1
//               void *s2   : String2
//               int   sz   : compare size
// Return      : int 
////////////////////////////////////////////////////////////////////////////////
static int MemCmp(void *s1, void *s2, int sz)
{
register unsigned char *f;
register unsigned char *t;
unsigned int            i;
	f = (unsigned char *)s1;
	t = (unsigned char *)s2;
	for(i = 0; i < sz; i++, f++, t++)
		if(*f > *t) return 1; // Not Equal
		else if( *f < *t) return -1;
	return 0; // Equal
}
////////////////////////////////////////////////////////////////////////////////
// Description : 메모리 블록 dest의 앞 sz바이트를 ch로 채운다 (memset() 로컬 구현).
// Prototype   : void MemSet(void *dest, unsigned char ch, int sz)
// Arguments   : void         *dest  : destination
//               unsigned char ch    : set charactor 
//               int           sz    : size
// Return      : void
////////////////////////////////////////////////////////////////////////////////
static void MemSet(void *dst, unsigned char ch, int sz)
{
register unsigned char *d;
unsigned int            i;
	d = (unsigned char *)dst;
	for(i = 0; i < sz; i++, d++) *d = ch;
	return ;
}
////////////////////////////////////////////////////////////////////////////////
// Description : src의 sz바이트를 dest로 복사한다 (memcpy() 로컬 구현. 겹치는
//               영역 처리는 하지 않으므로 memmove()가 필요한 경우 쓰지 말 것).
// Prototype   : void MemCopy(void *dest, void *src, int sz)
// Arguments   : void *dest :destination
//               void *src  : source
//               int   sz   : Source size
// Return      : void
////////////////////////////////////////////////////////////////////////////////
static void MemCopy(void *dst, void *src, int sz)
{
register unsigned char *f;
register unsigned char *t;
unsigned int            i;
	f = (unsigned char *)src;
	t = (unsigned char *)dst;
	for(i = 0; i < sz; i++, f++, t++)
		*t = *f;
	return ;
}


/******************************************************************************/
/* Time out Processing                                                        */
/* 등록된 모든 이벤트의 남은 시간(rtime)을 Timedif만큼 줄이고, 0 이하가 되면  */
/* answer가 설정된 항목에 한해 handler(event, fd, -event_id)를 호출해 타임아웃 */
/* 을 알린 뒤 rtime을 tout으로 리셋한다.                                      */
/* Prototype : static void _TimeoutProcessing(EVENT *event)                   */
/* Arguments : EVENT *event; 이벤트 컨텍스트                                  */
/* Return    : void                                                           */
/* Remarks   : local function                                                 */
/******************************************************************************/
static void _TimeoutProcessing(event)
EVENT *event;
{
register EVENT_LIST *p;
	if(!event->Totalcnt) return;
	p  = event->Eventlist;
	while(p){p->rtime -= Timedif; p = (EVENT_LIST *)p->next;}
	p  = event->Eventlist;
	while(p){
		if(p->rtime <= 0){
			if(p->answer)
				p->handler(event, p->fd, p->event_id * -1);
			p->rtime = p->tout;
		}
		p = (EVENT_LIST *)p->next;
	}
	return;
}
/******************************************************************************/
/* Clear deleted event item in linked list.                                   */
/* AppDelEventSelect()가 del=1로 표시해 둔 이벤트 항목들을 연결 리스트에서    */
/* 실제로 제거하고 메모리를 해제한다.                                        */
/* Prototype : static void _ClearDelEvent(EVENT *event)                      */
/* Arguments : EVENT *event; 이벤트 컨텍스트                                  */
/* Return    : void                                                           */
/* Remarks   : local function                                                 */
/******************************************************************************/
static void _ClearDelEvent(event)
EVENT *event;
{
register EVENT_LIST *p, *b;
register int         cnt = 0;
	if(!event->Totalcnt) return;
	b = p = event->Eventlist;
	while(p){
		if(p->del){
			if(!cnt){ /* 맨처음것 */
				event->Totalcnt --;
				if(!event->Totalcnt){
					free(event->Eventlist);
					event->Eventlist = NULL;
					p = NULL;
				}else{
					event->Eventlist = (EVENT_LIST *)p->next;
					free(p);
					p = (EVENT_LIST *)event->Eventlist->next;
				}
				return;
			}else{
				b->next = p->next;
				free(p);
				p = (EVENT_LIST *)b->next;
				event->Totalcnt --;
			}
		}else{
			cnt ++; b = p; p = (EVENT_LIST *)p->next;
		}
	}
	return;
}
/******************************************************************************/
/* Make event fd list for select()                                            */
/* 등록된 이벤트들의 fd로 select()용 fd_set(EventEntry)을 다시 구성하고        */
/* Maxfd를 갱신한다.                                                          */
/* Prototype : static int _MakeEventSet(EVENT *event)                        */
/* Arguments : EVENT *event; 이벤트 컨텍스트                                  */
/* Return    : int                                                            */
/*             NO Event entry : 0, SUCESS : 1                                 */
/* Remarks   : local function                                                 */
/******************************************************************************/
static int _MakeEventSet(event)
EVENT *event;
{
register EVENT_LIST *p;
	FD_ZERO(&event->EventEntry);
	p = event->Eventlist;
	event->Maxfd = 0;
	if(!event->Totalcnt) return event->Totalcnt;
	while(p){
		FD_SET(p->fd, &event->EventEntry);
		if(event->Maxfd < p->fd) event->Maxfd = p->fd;
		p = (EVENT_LIST *)p->next;	
	}
	return 1;	
}
/******************************************************************************/
/* Branch into Event handler                                                  */
/* select()가 반환한 EventEntry에서 ready 상태인 fd를 찾아 해당 이벤트의      */
/* handler(event, fd, event_id)를 호출한다 (del=1로 표시된 항목은 재진입      */
/* 방지를 위해 건너뜀).                                                       */
/* Prototype : static int _BranchHandler(EVENT *event)                       */
/* Arguments : EVENT *event; 이벤트 컨텍스트                                  */
/* Return    : int; 항상 0 (각 handler()의 반환값은 사용하지 않음)            */
/* Remarks   :                                                                */
/******************************************************************************/
static int _BranchHandler(event)
EVENT *event;
{
register EVENT_LIST *p;
	if(!event->Totalcnt) return 0;
	event->CurCnt = event->Totalcnt; /* Handler를 통한 신규 등록은 하지 않음 */
	p  = event->Eventlist;
	while(event->CurCnt){
		if(FD_ISSET(p->fd, &event->EventEntry)){
			p->rtime = p->tout;
			/* 재 진입 방지를 위함 */
			if(!p->del) p->handler(event, p->fd, p->event_id);
		}
		p = (EVENT_LIST *)p->next;
		event->CurCnt --;
	}
	return 0;
}

/*******************************************************************************
 * Drop Event structure
 * Prototype : void _DropEvent(event)
 * Argument  : EVENT *event;
 * Return    : void
 ******************************************************************************/
static void _DropEvent(event)
EVENT *event;
{
EVENT_LIST *p = NULL, *n;
	p = event->Eventlist;	
	while(p){
		n = p->next;
		free(p);
		p = n;
	}
	free(event); 
	return;
}
/******************************************************************************/
/* Check exist Event id                                                       */
/* Prototype : static int _IsExistEventId(EVENT *event, int id)              */
/* Arguments : EVENT *event; 이벤트 컨텍스트                                  */
/*             int id; 존재검사 할 Event id                                   */
/* Return    : int; Exist:1, Did't exist :0 (삭제 표시(del=1)된 항목은        */
/*             존재하지 않는 것으로 취급)                                     */
/* Remarks   :                                                                */
/******************************************************************************/
static int _IsExistEventId(event,id)
EVENT *event;
int id;
{
register EVENT_LIST *p;
	p = event->Eventlist;
	while(p){
		if(id == p->event_id && !p->del) return 1;
		p = (EVENT_LIST *)p->next;
	}
	return 0;
}
/******************************************************************************/
/* Event configuration initialization                                         */
/* Prototype : void *AppEventInitSelect(void)                                */
/* Arguments : void                                                           */
/* Return    : SUCESS: EVENT * (void*로 반환), FAIL: NULL                     */
/* Remarks   : Make event struct and environment for event handling.          */
/*             select() 기반 구현 (epoll 기반은 event_v2.c 참고)              */
/******************************************************************************/
void *AppEventInitSelect()
{
EVENT *event = NULL;
	event = (EVENT *)calloc(1, sizeof(EVENT));
	if(!event) return (EVENT *)NULL;
	MemSet((void *)event, 0x00, sizeof(EVENT));
	FD_ZERO(&event->EventEntry);
	event->Seconds = DEF_TIMEOUT;
	event->Usec    = 0;
	event->RUNNING = 1;
	event->DefaultHandler = NULL;
	return (void *)event;
}
/******************************************************************************/
/* Add event                                                                  */
/* Prototype : int AppAddEventAutoIdSelect(void *ent, int fd, int tout,      */
/*                                          int answer, int (*handler)())     */
/* Argumemts : void *ent; AppEventInitSelect()가 반환한 이벤트 컨텍스트       */
/*             int fd; Event fd (I)                                           */
/*             int tout; Timeout Value (Microsecond:1sec/100)                 */
/*             int answer; Need reporting timeout                             */
/*             int (*handler)(); Event processing function.(I)                */
/* Return    : Error occur : -1, SUCESS : int id; Event identifier(O)         */
/*             id는 자동 채번되며 MAX_ID(10000) 범위 내에서 순환한다.         */
/* Remarks   :                                                                */
/******************************************************************************/
int AppAddEventAutoIdSelect(ent, fd, tout, answer, handler)
void  *ent;
int fd, tout, answer, (*handler)();
{
register EVENT_LIST *p;
register int        id;
EVENT               *event;
	event = (EVENT *)ent;
	id = Id;
	if(!event->Totalcnt){
		event->Eventlist = (EVENT_LIST *)calloc(1, sizeof(EVENT_LIST));
		if(!event->Eventlist) return -1;
		event->Eventlist->fd        = fd;
		event->Eventlist->event_id  = id;
		event->Eventlist->tout      = tout;
		event->Eventlist->rtime     = tout;
		event->Eventlist->answer    = answer;
		event->Eventlist->del       = 0;
		event->Eventlist->handler   = handler;
		event->Totalcnt ++;
		Id ++;	Id = Id % MAX_ID;
		if(!Id)Id=START_ID;
		return id;
	}
	/*********** ID 결정 ****************************/
	p = event->Eventlist;
	while(_IsExistEventId(event, id)){id++; id = id % MAX_ID;}
	/*************** Link 추가 *********************/
	p = event->Eventlist;
	while(p->next) p = (EVENT_LIST *)p->next;
	p->next = (struct _EVENT_LIST *)calloc((size_t)1, 
		(size_t)sizeof(struct _EVENT_LIST));
	if(!p->next) return -1;
	p = (EVENT_LIST *)p->next;
	p->fd        = fd;
	p->event_id  = id;
	p->tout      = tout;
	p->rtime     = tout;
	p->answer    = answer;
	p->del       = 0;
	p->handler   = handler;
	event->Totalcnt ++;
	Id = id + 1; Id = Id % MAX_ID;
	if(!Id)Id=START_ID;
	return id;
}
/******************************************************************************/
/* Delete Event                                                               */
/* Prototype : int AppDelEventSelect(void *ent, int id)                      */
/* Argumemts : void *ent; 이벤트 컨텍스트                                     */
/*             int id; Event identifier(I)                                    */
/* Return    : NotFound : 0, SUCESS : 1                                       */
/*             주의: 즉시 삭제하지 않고 del=1로만 표시한다. 실제 제거는       */
/*             _ClearDelEvent()가 다음 AppEventLoopSelect() 루프에서 수행함.  */
/* Remarks   :                                                                */
/* -. 96.12.10(Sagittarius)                                                   */
/*    _BranchHandler에서 Delete를 수행하기 때문에 여기에서 Delete를 하면      */
/*    다음 Link에 있는 pointer 이상으로 문제가 발생한다. 때문에 이 routine    */
/*    단순히 p->del = 1을 해서 AppEventLoop()에서 정리 할 수 있도록 한다.     */
/******************************************************************************/
int AppDelEventSelect(void *ent, int id)
{
register EVENT_LIST *p;
EVENT    *event;
	event = (EVENT *)ent;
	if(!event->Totalcnt) return 1;
	p = event->Eventlist;
	while(p){
		if (p->event_id == id){
			p->del = 1; return 1;
		}
		p = (EVENT_LIST *)p->next;
	}
	return 0;
}

/******************************************************************************/
/* Application Event Loop Shutdown                                            */
/* Prototype : void AppEventLoopShutdownSelect(void *ent)                    */
/* Argument  : void *ent; 이벤트 컨텍스트. RUNNING 플래그를 0으로 만들어      */
/*             AppEventLoopSelect()의 루프를 다음 반복에서 빠져나오게 한다.   */
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void AppEventLoopShutdownSelect(void *ent)
{
EVENT *event = (EVENT *)ent;
	event->RUNNING = 0;
	return;
}
/******************************************************************************/
/* Change Application Event Loop select() timeout Value                       */
/* Prototype : void AppChangeEventTimeoutSelect(void *ent, long sec, long usec)*/
/* Argument  : void *ent; 이벤트 컨텍스트                                     */
/*             long sec, usec; select() timeout (Seconds, Microseconds)       */
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void AppChangeEventTimeoutSelect(void *ent, long sec, long usec)
{
EVENT *event = (EVENT *)ent;
	event->Seconds  = sec;
	event->Usec     = usec;
	return;
}
/*******************************************************************************
 * 설명      : Default Handler를 바꾼다. 이 handler는 매 AppEventLoopSelect()
 *             반복마다 select() 호출 직후 (등록된 이벤트 처리 전에) 무조건
 *             호출된다.
 * Prototype : void AppChangeDefaultHandlerSelect(void *ent, int (*handler)());
 * Arguments : void *ent; 이벤트 컨텍스트
 *             int (*handler)(); 새 Default Handler
 * Return    : void
 ******************************************************************************/
void AppChangeDefaultHandlerSelect(void *ent, int (*handler)())
{
EVENT *event = (EVENT *)ent;
	event->DefaultHandler = handler;
}
/******************************************************************************/
/* Application Event Loop                                                     */
/* select() 기반 메인 이벤트 루프. RUNNING이 0이 될 때까지 삭제된 이벤트      */
/* 정리 -> fd_set 재구성 -> select() 대기 -> DefaultHandler 호출 -> ready된   */
/* 이벤트 handler 호출 -> 1초 단위 타임아웃 처리를 반복하고, 종료 시 모든     */
/* 이벤트 리소스를 해제한다.                                                  */
/* Prototype : void AppEventLoopSelect(void *ent)                            */
/* Argument  : void *ent; AppEventInitSelect()가 반환한 이벤트 컨텍스트       */
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void AppEventLoopSelect(void *ent)
{
register int rtn;
time_t gettime, basetime;
EVENT *event = (EVENT *)ent;
	time(&basetime);
	while(event->RUNNING){
		_ClearDelEvent(event);
		_MakeEventSet(event);
		event->Timeout.tv_sec   = event->Seconds;
		event->Timeout.tv_usec  = event->Usec;
		rtn = select(event->Maxfd + 1, 
			&event->EventEntry, (fd_set *)NULL, (fd_set *)NULL, 
			&event->Timeout);
		if(event->DefaultHandler != NULL)event->DefaultHandler(event);
		if(rtn > 0) _BranchHandler(event);

		time(&gettime);
		if(basetime != gettime){
			Timedif = (int )(gettime - basetime);
			basetime = gettime;
			_TimeoutProcessing(event);
		}
	}
	_DropEvent(event);
	return;
}

