///
/// 
/// @section init_sec 프로그램소개 
/// @short User data 부분 추가
/// @todo  
/// @file zmqevent_arg.c
/// @date 2022. 09. 27. (화) 09:02:51 KST
/// @author Cento 
///
#define ZMQ_BUILD_DRAFT_API
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <zmq.h>

typedef struct _EVENT_LIST{
	int                type;     // fd type : 1:zmq socket, 2: nomal fd
	void               *zfd;
	int                fd;
	int                del;
	int                event_id;
	int                tout, rtime, answer;
	int                (*handler)();
	struct _EVENT_LIST *next;
}EVENT_LIST; 
typedef struct _EVENT{
	void           *Poller;
	int            Timeout;
	int            Totalcnt;
	int            CurCnt; // BranchHandler() 에서 Temp 로 사용하는 Count
						   // 2003.09.22 추가
						   // 한 program에서 두번의 AppEventInit시에
						   // 문제점 발견후 추가함.
						   // _BranchHandler()의 static 변수 이던것을
						   // EVENT 구조체 안으로 편입 시킴.
	int            RUNNING;
	long           Seconds;
	void          *udata  ; // User data
	int            (*DefaultHandler)();
	EVENT_LIST    *Eventlist;
	zmq_poller_event_t *events;
}EVENT;

#define DEF_TIMEOUT    10000 // 10초
#define START_ID       1000
#define MAX_ID    10000

static int Timedif = 0;
static int Id = 1000;

////////////////////////////////////////////////////////////////////////////////
// Description :
// Prototype   : void MemSet(void *dest, unsigned char ch, int sz)
// Arguments   : void         *dest  : destination
//               unsigned char ch    : set charactor 
//               int           sz    : size
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void MemSet(void *dst, unsigned char ch, int sz)
{
register unsigned char *d;
unsigned int            i;
	d = (unsigned char *)dst;
	for(i = 0; i < sz; i++, d++) *d = ch;
	return ;
}

/******************************************************************************/
/* Time out Processing                                                        */
/* Prototype : static void _TimeoutProcessing()                               */
/* Arguments : void                                                           */
/* Return    : void                                                           */
/* Remarks   : local function                                                 */
/******************************************************************************/
static void _TimeoutProcessing(EVENT *event)
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
/* Check exist Event id                                                       */
/* Prototype : int _IsExistEventId(id)                                        */
/* Arguments : int id; 존재검사 할 Event id                                   */
/* Return    : int; Exist:1, Did't exist :0                                   */
/* Remarks   :                                                                */
/******************************************************************************/
static int _IsExistEventId(EVENT *event,int id)
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
/* Prototype : EVENT * _AppEventInit(void *udata)                             */
/* Arguments : void                                                           */
/* Return    : int; SUCESS:EVENT *, FAIL:NULL                                 */
/* Remarks   : Make event struct and environment for event handling           */
/******************************************************************************/
void *_AppEventInit(void *udata)
{
EVENT *event = NULL;
	event = (EVENT *)calloc(1, sizeof(EVENT));
	if(!event) return (void *)NULL;
	MemSet((void *)event, 0x00, sizeof(EVENT));
	event->Poller = zmq_poller_new();
	if(!event->Poller) return (void *)NULL;

	event->udata   = udata;
	event->Timeout = DEF_TIMEOUT;
	event->RUNNING = 1;
	event->DefaultHandler = NULL;
	return (void *)event;
}
////////////////////////////////////////////////////////////////////////////////
// Add event  for nomal socket
// Prototype : int _AppAddEventAutoIdFd(fd, tout, answer, handler)
// Argumemts : int fd; Event fd (I)
//             int tout; Timeout Value (Microsecond:1sec/100)
//             int answer; Need reporting timeout
//             int (*handler)(); Event processing function.(I)
// Return    : Error occur : -1, SUCESS : int id; Event identifier(O)
// Remarks   :
////////////////////////////////////////////////////////////////////////////////
int _AppAddEventAutoIdFd(void *ent, int fd, int tout, int answer, int (*handler)())
{
register EVENT_LIST *p;
register int        id;
EVENT               *event;
	event = (EVENT *)ent;
	id = Id;
	if(!event->Totalcnt){
		event->Eventlist = (EVENT_LIST *)calloc(1, sizeof(EVENT_LIST));
		if(!event->Eventlist) return -1;
		event->Eventlist->type      = 2;
		event->Eventlist->zfd       = NULL;
		event->Eventlist->fd        = fd;
		event->Eventlist->del       = 0;
		event->Eventlist->event_id  = id;
		event->Eventlist->tout      = tout;
		event->Eventlist->rtime     = tout;
		event->Eventlist->answer    = answer;
		event->Eventlist->handler   = handler;
		event->Totalcnt ++;
		Id ++;	Id = Id % MAX_ID;
		if(!Id)Id=START_ID;
		zmq_poller_add_fd(event->Poller, fd, (void *)event->Eventlist, ZMQ_POLLIN);
		event->events = (zmq_poller_event_t *)reallocarray(
			event->events, event->Totalcnt , sizeof(zmq_poller_event_t));
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
	p->type      = 2; // Normal Type
	p->fd        = fd;
	p->zfd       = NULL;
	p->del       = 0;
	p->event_id  = id;
	p->tout      = tout;
	p->rtime     = tout;
	p->answer    = answer;
	p->handler   = handler;
	event->Totalcnt ++;
	Id = id + 1; Id = Id % MAX_ID;
	if(!Id)Id=START_ID;
	zmq_poller_add_fd(event->Poller, fd, (void *)p, ZMQ_POLLIN);
	event->events = (zmq_poller_event_t *)reallocarray(
		event->events, event->Totalcnt , sizeof(zmq_poller_event_t));
	return id;
}
////////////////////////////////////////////////////////////////////////////////
// Add event  for zeromq socket
// Prototype : int _AppAddEventAutoId0mq(fd, tout, answer, handler)
// Argumemts : int fd; Event fd (I)
//             int tout; Timeout Value (Microsecond:1sec/100)
//             int answer; Need reporting timeout
//             int (*handler)(); Event processing function.(I)
// Return    : Error occur : -1, SUCESS : int id; Event identifier(O)
// Remarks   :
////////////////////////////////////////////////////////////////////////////////
int _AppAddEventAutoId0mq(void *ent, void *fd, int tout, 
		int answer, int (*handler)() )
{
register EVENT_LIST *p;
register int        id, rtn;
EVENT               *event;
	event = (EVENT *)ent;
	id = Id;
	if(!event->Totalcnt){
		event->Eventlist = (EVENT_LIST *)calloc(1, sizeof(EVENT_LIST));
		if(!event->Eventlist) return -1;
		event->Eventlist->type      = 1;
		event->Eventlist->zfd       = fd;
		event->Eventlist->fd        = -1;
		event->Eventlist->del       = 0;
		event->Eventlist->event_id  = id;
		event->Eventlist->tout      = tout;
		event->Eventlist->rtime     = tout;
		event->Eventlist->answer    = answer;
		event->Eventlist->handler   = handler;
		event->Totalcnt ++;
		Id ++;	Id = Id % MAX_ID;
		if(!Id)Id=START_ID;
		rtn = zmq_poller_add(event->Poller, fd, (void *)event->Eventlist, ZMQ_POLLIN);
		event->events = (zmq_poller_event_t *)reallocarray(
			event->events, event->Totalcnt , sizeof(zmq_poller_event_t));
		if(!event->events) return -1;
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
	p->type      = 1; // ZMQ Type
	p->zfd       = fd;
	p->fd        = -1;
	p->del       = 0;
	p->event_id  = id;
	p->tout      = tout;
	p->rtime     = tout;
	p->answer    = answer;
	p->handler   = handler;
	event->Totalcnt ++;
	Id = id + 1; Id = Id % MAX_ID;
	if(!Id)Id=START_ID;
	rtn = zmq_poller_add(event->Poller, fd, (void *)p, ZMQ_POLLIN);
	event->events = (zmq_poller_event_t *)reallocarray(
			event->events, event->Totalcnt , sizeof(zmq_poller_event_t));
	if(!event->events) return -1;
	return id;
}
/******************************************************************************/
/* Delete Event                                                               */
/* Prototype : int _AppDelEvent(id)                                           */
/* Argumemts : int id; Event identifier(I)                                    */
/* Return    : NotFound : 0, SUCESS : 1                                       */
/* Remarks   :                                                                */
/* -. 96.12.10(Sagittarius)                                                   */
/*    _BranchHandler에서 Delete를 수행하기 때문에 여기에서 Delete를 하면      */
/*    다음 Link에 있는 pointer 이상으로 문제가 발생한다. 때문에 이 routine    */
/*    단순히 p->del = 1을 해서 AppEventLoop()에서 정리 할 수 있도록 한다.     */
/******************************************************************************/
int _AppDelEvent(void *ent, int id)
{
EVENT_LIST *p, *save;
EVENT    *event;
	event = (EVENT *)ent;
	if(!event->Totalcnt) return 1;
	p = event->Eventlist;
	save = p->next;
	while(p){
		if (p->event_id == id){
			if(p->type == 1) // ZMQ socket
				zmq_poller_remove(event->Poller, p->zfd);
			else
				zmq_poller_remove_fd(event->Poller, p->fd);
			p->del = 1;
			return 1;
		}
		p = save;
		if(p) save = p->next;
	}
	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// Clear deleted event item in linked list.
// Prototype : static void _ClearDelEvent()
// Arguments : void
// Return    : int
//             NO Event entry : 0, SUCESS : 1
// Remarks   : local function             
////////////////////////////////////////////////////////////////////////////////
static void _ClearDelEvent(EVENT *event)
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
				event->events = (zmq_poller_event_t *)reallocarray(
					event->events, event->Totalcnt ,sizeof(zmq_poller_event_t));
                return;
            }else{
                b->next = p->next;
                free(p);
                p = (EVENT_LIST *)b->next;
                event->Totalcnt --;
				event->events = (zmq_poller_event_t *)reallocarray(
					event->events, event->Totalcnt ,sizeof(zmq_poller_event_t));
            }
        }else{
            cnt ++; b = p; p = (EVENT_LIST *)p->next;
        }
    }
    return;
}

////////////////////////////////////////////////////////////////////////////////
// Tear down an EVENT context: unregister every remaining event from the
// poller, free each EVENT_LIST node, destroy the poller, then free the
// EVENT struct itself. Called once by _AppEventLoop() after RUNNING drops
// to 0 (via _AppEventLoopShutdown()); not meant to be called directly.
// Prototype : static void _DropEvent(void *ent)
// Arguments : void *ent : EVENT * returned by _AppEventInit()
// Return    : void
////////////////////////////////////////////////////////////////////////////////
static void _DropEvent(void *ent)
{
EVENT *event = (EVENT *)ent;
EVENT_LIST *p = NULL, *save;
	p = event->Eventlist;
	while(p){
		save = p->next;
		if(p->type == 1) // ZMQ socket
			zmq_poller_remove(event->Poller, p->zfd);
		else
			zmq_poller_remove_fd(event->Poller, p->fd);
		free(p);
		p = save;
	}
	zmq_poller_destroy(&event->Poller);
	free(ent);
	return;

}
/******************************************************************************/
/* Application Event Loop Shutdown                                            */
/* Prototype : void AppEventLoopShutdown()                                    */
/* Argument  : void;                                                          */
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void _AppEventLoopShutdown(void *ent)
{
EVENT *event = (EVENT *)ent;
	event->RUNNING = 0;
	return  ;
}
/******************************************************************************/
/* Change Application Event Loop select() timeout Value                       */
/* Prototype : void AppChangeEventTimeout(ent, sec)
/* Argument  : int sec;
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void _AppChangeEventTimeout(void *ent, int sec, long usec)
{
EVENT *event = (EVENT *)ent;
	event->Timeout  = sec;
	return;
}
/*******************************************************************************
 * 설명      : Defalut Handler를 바꾼다
 * Prototype : void AppChangeDefalutHandler(event, handler);
 * Arguments :
 * Return    :
 ******************************************************************************/
void _AppChangeDefaultHandler(void *ent, int (*handler)())
{
EVENT *event = (EVENT *)ent;
	event->DefaultHandler = handler;
}
/******************************************************************************/
/* Application Event Loop                                                     */
/* Prototype : void AppEventLoop()                                            */
/* Argument  : void;                                                          */
/* Return    : void                                                           */
/* Remarks   :                                                                */
/******************************************************************************/
void _AppEventLoop(void *ent)
{
int rtn, i;
EVENT *event = (EVENT *)ent;
zmq_poller_event_t *p;
EVENT_LIST         *lst;

time_t gettime, basetime;
	time(&basetime);
	while(event->RUNNING){
		_ClearDelEvent((EVENT *)event);
		rtn = zmq_poller_wait_all(event->Poller, event->events, 
				event->Totalcnt, event->Timeout);
		if(event->DefaultHandler != NULL)event->DefaultHandler(event);
		if(rtn <= 0) continue;
		p = event->events;
		for(i = 0 ; i < rtn; i++){
			lst = (EVENT_LIST *)p->user_data;
			if(lst->type == 1) // ZMQ
				lst->handler(ent, lst->zfd, lst->event_id);
			else
				lst->handler(ent, lst->fd, lst->event_id);
			p++;
		}	
	}
	_DropEvent(ent);
	return;
}
////////////////////////////////////////////////////////////////////////////////
//  User data pointer를 얻는다
/// @fn   void *AppEventGetUserData(void *ent)
/// @brief Return the user-data pointer previously stored on this EVENT
///        context (via _AppEventInit()'s udata argument or
///        AppEventSetUserData()).
/// @param ent EVENT * returned by _AppEventInit()
/// @return the stored user-data pointer, or NULL if none was set
////////////////////////////////////////////////////////////////////////////////
void *AppEventGetUserData(void *ent)
{
EVENT *event;
	event = (EVENT *)ent;
	return event->udata;
}

////////////////////////////////////////////////////////////////////////////////
//  User data 를 설정한다
/// @fn   void AppEventSetUserData(void *ent, void *udata)
/// @brief Store (overwrite) the user-data pointer on this EVENT context,
///        readable later via AppEventGetUserData().
/// @param ent   EVENT * returned by _AppEventInit()
/// @param udata caller-owned pointer to associate with this event context
/// @return void
////////////////////////////////////////////////////////////////////////////////
void AppEventSetUserData(void *ent, void *udata)
{
EVENT *event;
	event = (EVENT *)ent;
	event->udata = udata;
	return ;
}

////////////////////////////////////////////////////////////////////////////////
// 필요시 Memory free 
// Prototype : void AppEventFreeUserData(void *ent)
// Arguments :
// Return    :
////////////////////////////////////////////////////////////////////////////////
void AppEventFreeUserData(void *ent)
{
EVENT *event;
	event = (EVENT *)ent;
	if(event->udata) free(event->udata);
}
