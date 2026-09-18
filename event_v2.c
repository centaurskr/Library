///
///< ##⚽ Event library Version 2.0\n
///<      주의 : 이전 Version과 호환성 없음
///<  @file   libeplevent.c
///<  @date   2023. 11. 06. (월) 11:14:49 KST
///<  @author Cento ♘♞ 
/// ⚞ SHIFT+CTL+u or SHIFT+CTL+V
/// ⚽(u26BD), ☝(u261D)......
///
#include <stdlib.h>
#include <stdio.h>
#include <sys/epoll.h>
#include <unistd.h>

#define  MAX_EVENT     4096
#define  DEF_TIMEOUT   1000  // 1 second

#define  EVENT_IN      0x01  // Data in Event
#define  EVENT_TO      0x10  // Timeout Event

///< 내부에서 사용하는 Event 구조체
typedef struct _Event{
	int            fd      ;///< event fd
	int            ispoll  ;///< epoll 처리? , 1인경우 tiemout 계산 안함(1회)
	int            timeout ;///< timeout  (ms 단위)
	int            tremind ;///< timeout 남은시간 (ms 단위)
	int            toreport;///< timeout report   0:Don't report, 1: Report
	int            state   ;///< 상태 : 0:pause, 1:activate
	int            in      ;///< Event List에 있음(epoll_wait() 감시대상)
	void          *arg     ;///< User data, 할당과 free는 사용자가
	//----------------------Event info *, int fd, int ewhat, void *data)
	int            (*handler)(void *, int , int, void *); ///< Handler(반환값 미사용)
	struct  _Event *next   ; ///< Next     EVENT information
}EVENT;

///< 내부에서 사용하는 Event Context
typedef struct _EVCONTEXT{
	int          EpollFd    ; ///< for Epoll
	unsigned int Run        ; ///< 수행중?
	EVENT        *Head,*Tail; ///< Event List (Double Linked list)
	int          timeout    ; ///< epoll wait time out( ms:1/1,000 second)

	/********** DEFAULT HANDLER *********************/
	// deftimeout 간격으로 defaulthandler 수행
	// deftimeout은 초단위로 설정
	//                             EVCONTEXT *, void *handlerdata
	int           (*defhandler)(void *, void *); ///< default Handler
	void         *defarg    ; ///< Default Handler Data
	int           defstate  ; ///< 상태 : 0:pause, 1:activate
	int           deftimeout; ///< Default handler timeout (ms단위:1/1,000)
	int           deftremind; ///< timeout 남은 시간       (ms단위)
}EVCONTEXT;
////////////////////////////////////////////////////////////////////////////////
///< 디버그용: 단일 EVENT 구조체의 필드 값을 표준출력으로 덤프한다
///< @fn      void DumpEvent(EVENT *ent)
///< @param   ent  덤프할 EVENT 항목
///< @return  없음
////////////////////////////////////////////////////////////////////////////////
void DumpEvent(EVENT *ent)
{
	printf("\n\t\tfd      [%d]\n", ent->fd);
	printf("\t\tispoll  [%d]\n", ent->ispoll);
	printf("\t\ttimeout [%d]\n", ent->timeout);
	printf("\t\ttremind [%d]\n", ent->tremind);
	printf("\t\ttoreport[%d]\n", ent->toreport);
	printf("\t\tstate   [%d]\n", ent->state);
	printf("\t\tin      [%d]\n", ent->in);
	printf("\t\targ     [%p]\n", ent->arg);
	printf("\t\thandler [%p]\n", ent->handler);
	printf("\t\tnext    [%p]\n", ent->next);;
}
////////////////////////////////////////////////////////////////////////////////
///< 디버그용: EVCONTEXT와 그에 연결된 모든 EVENT 항목을 표준출력으로 덤프한다
///< @fn      void DumpContext(EVCONTEXT *ctx)
///< @param   ctx  덤프할 Event Context
///< @return  없음
////////////////////////////////////////////////////////////////////////////////
void DumpContext(EVCONTEXT *ctx)
{
EVENT *ent;
	printf("\n-------Dump Event Context ------------\n");
	printf("\tEpollFd   [%d]\n",ctx->EpollFd);
	printf("\tRun       [%d]\n",ctx->Run     );
	printf("\tHead      [%p] Tail[%p]\n", ctx->Head, ctx->Tail);
	ent = ctx->Head;
	while(ent){DumpEvent(ent); ent= ent->next;}
	printf("\ttimeout    [%d]\n", ctx->timeout);
	printf("\tdefhandler [%p]\n", ctx->defhandler);
	printf("\tdefarg     [%p]\n", ctx->defarg);
	printf("\tdefstate   [%d]\n", ctx->defstate);
	printf("\tdeftimeout [%d]\n", ctx->deftimeout);
	printf("\tdeftremind [%d]\n", ctx->deftremind);
	printf("------------------------------------------\n");
}
////////////////////////////////////////////////////////////////////////////////
///< 모든 Event Link를 삭제한다(내부함수)
///< @fn         static void _deleteAllEventLink(EVCONTEXT *context)
///< @param  context  Event context
///< @return 없음
////////////////////////////////////////////////////////////////////////////////
static void _deleteAllEventLink(EVCONTEXT *context)
{
EVENT *ent;
	if(context->Head == NULL) return;
	ent = context->Head;	
	while(ent){
		context->Head = (EVENT *)ent->next;
		free(ent);
		ent = context->Head;
	}
	context->Head = NULL;
	context->Tail = NULL;
	return;
}

////////////////////////////////////////////////////////////////////////////////
///< Event List를 정리한다 (내부함수)\n
///<     evnet->in == 0인 것을 Link 삭제
///< @fn  static void _clearEvent(EVCONTEXT *context)
///< @param  context Event context
///< @return 없음
////////////////////////////////////////////////////////////////////////////////
static void _clearEvent(EVCONTEXT *context)
{
if(context == NULL || context->Head == NULL) return;

EVENT *current = context->Head;
EVENT *prev = NULL;

while(current){
	if(current->in == 0){
		// Linked List 삭제
		if(current == context->Head){
			context->Head = (EVENT *)current->next;
			// 리스트의 마지막 요소였다면 Tail도 NULL로 설정
			if(context->Head == NULL) { 
				context->Tail = NULL;
			}
			free(current);
			// Head가 변경되었으므로 current를 새로운 Head로 설정
			current = context->Head; 
		} else if (current == context->Tail) {
			// prev가 Tail 앞에 있는 노드여야 함
			if (prev) {
				prev->next = NULL;
			}
			free(current);
			context->Tail = prev; // Tail을 이전 노드로 설정
			current = NULL; // 루프 종료를 위해 current를 NULL로 설정
		} else {
			if (prev) {
				prev->next = (EVENT *)current->next;
			}
			free(current);
			// prev->next가 새로운 current가 됨
			current = (EVENT *)prev->next; 
		}
	} else {
		prev = current;
		current = (EVENT *)current->next;
	}
} // While Loop
} // Main
////////////////////////////////////////////////////////////////////////////////
///< Event를 등록한다 (epoll_ctl EPOLL_CTL_ADD로 fd를 감시 목록에 추가)
///< @fn  int Event_AddEvent(
///<                     void *info,
///<                     int   fd,
///<                     int (*handler)(void *, int, int , void *),
///<                     void *arg,
///<                     int tout,
///<                     int state)
///< @param info     Context
///< @param fd       File Descriptor
///< @param handler  Handler function
///< @param arg      Handler Argument
///< @param tout     timeout value (초단위, 0이면 Report 하지 않음)
///< @param state    Handler 상태 0:PAUSE,1:ACTIVATE
///<
///< @return  1    성공
///< @return  -1   error (calloc)
///< @return  -2   error (epoll_ctl(EPOLL_CTL_ADD))
////////////////////////////////////////////////////////////////////////////////
int Event_AddEvent(
	void *info,  // Context
	int   fd,    // File Descriptor
	int   (*handler)(void *, int, int , void *),  // Handler
	void *arg, // Handler Argument
	int tout,    // timeout value (초단위)
	int state)   // Handler 상태 0:PAUSE,1:ACTIVATE
{
EVCONTEXT *context;
EVENT     *event;
struct epoll_event epent;
int  rtn;
	context = (EVCONTEXT *)info;
	// ①  Memory 할당
	event = (EVENT *)calloc(1, sizeof(EVENT));
	if(!event) return -1;
	// ②  Event 정보 설정
	event->fd = fd;
	if(tout > 0){ // Time out 값이 있는 경우
		event->timeout  = tout * 1000;
		event->tremind  = tout * 1000;
		event->toreport = 1;    // timeout Report 함
	}else{ // Timout 값이 없는 경우
		event->timeout  = 0;
		event->tremind  = 0;
		event->toreport = 0;    // timeout Report 하지않음
	}

	event->state = state; // 0:PAUSE, 1:ACTIVATE
	event->arg   = arg;   // Event Hanedler argument pointer
	event->handler = handler; // Event Handler
	event->in    = 1;     // Event 감시 List에 있음

	// ③  epoll 등록
	epent.events   = EPOLLIN;
	epent.data.ptr = (void *)event;
	rtn = epoll_ctl(context->EpollFd, EPOLL_CTL_ADD, fd, &epent);
	if(rtn < 0){free(event); return -2;}

	// ④  Link에 등록
	if(context->Head == NULL){ // 맨처음
		context->Head = event;
		context->Tail = event;
	}else{
		context->Tail->next = event;
		context->Tail       = event;
	}

//	DumpContext(context);

	return 1;
}
////////////////////////////////////////////////////////////////////////////////
///< EventHandler argument pointer를 구한다
///< @fn   void *Event_GetEventArg(void *info, int fd)
///< @param    info     Context
///< @param    fd       File Descriptor
///<
///< @return   NULL:해당Fd가 없거나, Argument가 NULL인경우
////////////////////////////////////////////////////////////////////////////////
void *Event_GetEventArg(void *info, int fd)
{
EVCONTEXT *context;
EVENT     *event;
	context = (EVCONTEXT *)info;
	event = context->Head;
	while(event){
		if(event->fd == fd) return event->arg;
		event = event->next;
	}
	return NULL;
}
////////////////////////////////////////////////////////////////////////////////
///< EventHandler argument를 설정한다
///< @fn      int Event_SetEventArg(void *info, int fd, void *arg)
///< @param   info  Context
///< @param   fd    File Descriptor
///< @param   arg   Memory가 할당된 Event argument
///< @return  1     성공
///< @return  0     fd가 없음
////////////////////////////////////////////////////////////////////////////////
int Event_SetEventArg(void *info, int fd, void *arg)
{
EVCONTEXT         *context;
EVENT             *event;
struct epoll_event epent;
int                rtn;

	context = (EVCONTEXT *)info;
	event = context->Head;
	while(event){
		if(event->fd == fd){
			// List에 등록
			event->arg = arg;  
			// epoll에 등록
			epent.events   = EPOLLIN;
			epent.data.ptr = arg;
			rtn = epoll_ctl(context->EpollFd, EPOLL_CTL_MOD, fd, &epent);
			if(rtn == -1) return 0; // Check errno
			else return 1;
		}
		event = event->next;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
///< Memory를 할당한 Event Handler argument의 memory를 해제한다
///<     ☞ Handler등록시 malloc(),calloc()등으로 할당 Argument인 경우 만....
///< @fn   int Event_FreeEventArg(void *info, int fd)
///< @param info   Context
///< @param fd     File Descriptor
///<
///< @return    1      성공
///< @return   0      fd가 없거나, Argument가 NULL인 경우
////////////////////////////////////////////////////////////////////////////////
int Event_FreeEventArg(void *info, int fd)
{
EVCONTEXT *context;
EVENT     *event;
	context = (EVCONTEXT *)info;
	event = context->Head;
	while(event){
		if(event->fd == fd){
			if(event->arg != NULL){
				free(event->arg);
				return 1;
			}
		}
		event = event->next;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
///< Event를 삭제한다. Link "in"을 1 에서 0으로 변경하고.\n
///<     epoll_wait()에서 삭제. Link삭제는 _clearEvent()에서 한다
///< @fn       int Event_DeleteEvent(void *info, int fd)
///< @param    info   Context
///< @param    fd     File Descriptor
///< @return   1      성공
///< @return   0      실패-fd없음
////////////////////////////////////////////////////////////////////////////////
int Event_DeleteEvent(void *info, int fd)
{
EVCONTEXT *context;
EVENT     *event;
	context = (EVCONTEXT *)info;
	event = context->Head;
	while(event){
		if(event->fd == fd){
			epoll_ctl(context->EpollFd, EPOLL_CTL_DEL, event->fd, NULL);
			event->in = 0; // OUT으로 변경
			return 1;
		}
		event = event->next;
	}
	return 0;
}
////////////////////////////////////////////////////////////////////////////////
///< Event Loop의 Default Handler를 등록한다
///<      재설정을 하는 경우 arg의 Memory 정리는 호출측에서 해야 함.
///< @fn      void Event_SetDefaultHandler(
///<          void *info, void (*handler)(), void *arg, int tout, int state)
///< @param      info       Event_CreateNew()에서 생성한 Evnet Context
///< @param      handler()  Defalut Handler function 
///< @param      arg        Default handler argument
///< @param      tout       수행 간격(초단위)
///< @param      state      초기 수행 상태 (0:PAUSE, 1:ACTIVATE)
///< @return     void 
////////////////////////////////////////////////////////////////////////////////
void Event_SetDefaultHandler(void *info, int (*handler)(void*, void *), 
		void *arg, int tout, int state)
{
EVCONTEXT *context;
	context = (EVCONTEXT *)info;
	context->defhandler = handler;
	context->defstate   = state;
	context->deftimeout = tout * 1000; // Ms 단위
	context->deftremind = tout * 1000; // Ms 단위
	context->defarg     = arg;
}
////////////////////////////////////////////////////////////////////////////////
///< Default handler의 Timer을 재 설정한다
///< @fn       void Event_SetDefautlHandlerTimer(void *info, int tout)
///< @param    info       Event_CreateNew()에서 생성한 Evnet Context
///< @param    tout       수행 간격(초단위)
///< @return   없음
////////////////////////////////////////////////////////////////////////////////
void Event_SetDefaultHandlerTimer(void *info, int tout)
{
EVCONTEXT *context;
	context = (EVCONTEXT *)info;
	context->deftimeout = tout * 1000; // Ms 단위
	context->deftremind = tout * 1000; // Ms 단위
}
////////////////////////////////////////////////////////////////////////////////
///< Default handler의 상태 변경
///< @fn         void Event_SetDefaultHandlerState(void *info, int state)
///< @param      info       Event_CreateNew()에서 생성한 Evnet Context
///< @param      state      초기 수행 상태 (0:PAUSE, 1:ACTIVATE)
///< @return   
////////////////////////////////////////////////////////////////////////////////
void Event_SetDefaultHandlerState(void *info, int state)
{
EVCONTEXT *context;
	context = (EVCONTEXT *)info;
	context->defstate = state;
	if(state == 1) // ACTIVATE이면 Timer 재 설정
		context->deftremind = context->deftimeout;
}
////////////////////////////////////////////////////////////////////////////////
///<  handler의 상태 변경
///< @fn         void Event_SetHandlerState(void *info, int fd, int state)
///< @param      info       Event_CreateNew()에서 생성한 Evnet Context
///< @param      fd         Event FD
///< @param      state      초기 수행 상태 (0:PAUSE, 1:ACTIVATE)
///< @return    없음
////////////////////////////////////////////////////////////////////////////////
void Event_SetHandlerState(void *info, int fd, int state)
{
EVCONTEXT *context;
EVENT     *event;
	context = (EVCONTEXT *)info;
	event = context->Head;
	while(event){
		if(event->fd == fd){
			event->state = state;
			return;
		}
		event = event->next;
	}
	return ;
}
////////////////////////////////////////////////////////////////////////////////
///< Event Loop timeout 설정(epoll_wait timeout)
///< @fn         void Event_SetEventTimeout(void *info, double tout)
///< @param     info       Event_CreateNew()에서 생성한 Evnet Context
///< @param     tout       초단위, (0.5초 인경우 : 0.5)
///< Return     없음
////////////////////////////////////////////////////////////////////////////////
void Event_SetEventTimeout(void *info, double tout)
{
EVCONTEXT *context;
	context = (EVCONTEXT *)info;
	context->timeout = tout * 1000;
}
////////////////////////////////////////////////////////////////////////////////
///< Event Loop 종료
///< @fn   void Event_Shutdown(void *info)
///< @param info  Event Context
///< @return 없음
////////////////////////////////////////////////////////////////////////////////
void Event_Shutdown(void *info)
{
EVCONTEXT          *context;
	context =(EVCONTEXT *)info;
	context->Run = 0;
	return;
}
////////////////////////////////////////////////////////////////////////////////
///< Event Loop 시작
///< @fn   void Event_StartLoop(void *info)
///< @param info  Event Context
///< @return 없음
////////////////////////////////////////////////////////////////////////////////
void Event_StartLoop(void *info)
{
EVCONTEXT          *context;
EVENT              *list, *ent;
struct epoll_event ev[MAX_EVENT];
int                cnt, i;
	context =(EVCONTEXT *)info;
	context->Run = 1;
	while(context->Run){

//DumpContext(context);
		// 삭제된 Event Link 정리
		_clearEvent(context);

		// Event Wait
		cnt = epoll_wait(context->EpollFd, ev, MAX_EVENT, context->timeout);
		if(cnt < 0) continue; // epoll error
		// Event 처리
		for(i = 0; i < cnt ; i++){
			ent = (EVENT *)ev[i].data.ptr;
			ent->handler((void *)context, ent->fd, 0, ent->arg);
			// Handler 에서 Shutdown 처리했을 경우
			if(context->Run == 0) break;
			// Timer Reset
			if(ent->toreport){
				ent->tremind  = ent->timeout;// timeout 남은시간 (ms 단위)
				ent->ispoll   = 1; // epoll flag set(처리했음)
			}
		}
		// Handler에서 Shotdown했을 경우를 Check
		if(context->Run == 0) break; // While loop 종료
		// Timeout 처리
		list = context->Head;
		while(list){
			// activate and report Timeout ?
			// report요구 한 것
			// ACTIVATE인 것
			// epoll_wait()애서 처리 안한 것
			if(list->toreport && list->state == 1 && list->ispoll == 0){ 
				list->tremind -= context->timeout;
				if(list->tremind <= 0){
					list->handler((void *)context, list->fd, 1, list->arg);
					list->tremind = list->timeout;
					list->ispoll = 0;  // epoll flag reset(다음을 위해)
				}
			}
			list->ispoll = 0; // epoll flag Reset
			list = list->next;
		} // Timeout While
		// Default handler
		if(context->defhandler && context->defstate == 1){
			context->deftremind -= context->timeout;
			if(context->deftremind <= 0){
				context->deftremind = context->deftimeout; // Reset
				context->defhandler((void*)context, (void *)context->defarg);
			}
		}

	} // Main Loop
}
////////////////////////////////////////////////////////////////////////////////
///< EVENT 초기화
///< @fn  void *Event_CreateNew()
///< @param  없음
///< @return Event Context
////////////////////////////////////////////////////////////////////////////////
void *Event_CreateNew()
{
EVCONTEXT *context;
	context = (EVCONTEXT *)calloc(1, sizeof(EVCONTEXT));
	if(!context) return NULL;
	context->EpollFd = epoll_create(MAX_EVENT);
	if(context->EpollFd < 0){free(context); return NULL;}

	context->Run           = 0;
	context->Head          = NULL;
	context->Tail          = NULL;
	context->timeout       = DEF_TIMEOUT; // 1,000milliseconds,1초

	context->defhandler     = NULL;
	context->defarg         = NULL;
	context->defstate       = 0;
	context->deftimeout     = 1000; // 1,000milliseconds, 1초

	return (void *)context;
}
