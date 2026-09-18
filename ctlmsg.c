//
// Description : Send Controll Message (Multicasting Version )
// File Name   : ctlmsg.c
// Date        : 2017. 08. 16. (수) 15:07:54 KST
// By          :
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>
#include <errno.h>

#include <unistd.h>
#include <time.h>

#include "TbCapi.h"
#include "TbData.h"

/// Control Message를 위한 기본정보
typedef struct _CONTROLINFO{
	unsigned int      MyNum       ;  ///< 내 process 식별 번호
	char              CtlGroup[16];  ///< Multicasting Group
	int               CtlPort     ;  ///< Port
	int               wCtlFd, rCtlFd; ///< Message 전송/수신 Fd
	struct sockaddr_in CtlAddr    ;   ///< Message Publish addr
}CTLINFO;

#define _CRT_SECURE_NO_WARNINGS

////////////////////////////////////////////////////////////////////////////////
/// @brief Control 전송을 종료한다
/// @fn     void DestroyControlMessage(void *ctlinfo)
/// @param  ctlinfo Control message context
/// @return  없음
////////////////////////////////////////////////////////////////////////////////
void DestroyControlMessage(void *ctlinfo)
{
CTLINFO *info;
	info = (CTLINFO *)ctlinfo;
	if(info){
		close(info->wCtlFd);
		close(info->rCtlFd);
		free(ctlinfo);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Description : Control Message 송수신용 컨텍스트를 생성한다. 루프백(lo)
//               인터페이스로 멀티캐스트 구독/발행 소켓을 함께 연다.
// Prototype   : void *InitControlMessage(int port, char *group, int mynum)
// Arguments   : port  : Control Message 멀티캐스트 포트
//               group : 멀티캐스트 그룹 주소 문자열 (16바이트 이하)
//               mynum : 이 프로세스를 식별할 번호 (ControlMessage의 From 필드로 쓰임)
// Return      : 성공 시 CTLINFO 컨텍스트 포인터, 실패 시 NULL
//               (group 길이 초과, 구독/발행 소켓 open 실패 시)
////////////////////////////////////////////////////////////////////////////////
void *InitControlMessage(int port, char *group, int mynum)
{
CTLINFO *ctl;
	ctl = (CTLINFO *)calloc(1, sizeof(CTLINFO));
	if(!ctl) return NULL;
	if(strlen(group) > 16){free(ctl); return NULL;}
	strncpy(ctl->CtlGroup, group, strlen(group));
	ctl->CtlPort = port;
	ctl->MyNum   = mynum;

	ctl->rCtlFd = OpenMtSubscribe("lo", ctl->CtlGroup, ctl->CtlPort);
	if(ctl->rCtlFd < 0){ free(ctl);return NULL; }
	ctl->wCtlFd = OpenMtPublish(&ctl->CtlAddr, "lo", 
						ctl->CtlGroup, ctl->CtlPort, 0);
	if(ctl->wCtlFd < 0){ close(ctl->rCtlFd);free(ctl);return NULL; }

	return (void *)ctl;
}

////////////////////////////////////////////////////////////////////////////////
// Description : Control Message 한 건을 멀티캐스트로 전송한다. msg가
//               CTL_MSG_LEN(80바이트)을 넘으면 앞부분만 잘라서 보낸다.
// Prototype   : int ControlMessage(void *info, unsigned int to, unsigned int cmd, char *msg, int len)
// Arguments   : info : InitControlMessage()가 반환한 컨텍스트
//               to   : 수신 대상 프로세스 번호 (CTLPK.To)
//               cmd  : Command ID (CTLPK.Cmd, 예: CMD_START)
//               msg  : 전송할 메시지 본문
//               len  : msg 길이 (CTL_MSG_LEN 초과 시 CTL_MSG_LEN만큼만 복사)
// Return      : 항상 1 (sendto() 실패 여부는 반환값에 반영되지 않는다)
////////////////////////////////////////////////////////////////////////////////
int ControlMessage(void *info, unsigned int to, unsigned int cmd, char *msg, int len)
{
	CTLINFO *ctli;
	CTLPK pk;

	int   rtn;

	ctli = (CTLINFO *)info;
	memset((char *)&pk, 0x00, sizeof(CTLPK));
	clock_gettime(CLOCK_REALTIME, &pk.tv);
	pk.From = ctli->MyNum;
	pk.To   = to;
	pk.Cmd  = cmd;
	if(len > CTL_MSG_LEN) memcpy(pk.Msg, msg, CTL_MSG_LEN);
	else memcpy(pk.Msg, msg, len);
	rtn = sendto(ctli->wCtlFd, (char *)&pk, sizeof(CTLPK),
			0, (struct sockaddr *)&ctli->CtlAddr, sizeof(struct sockaddr));

	return 1;

}
////////////////////////////////////////////////////////////////////////////////
// Description : Control Message 수신용 소켓 fd를 얻는다 (select/poll 등에
//               등록해서 수신 이벤트를 감지할 때 사용).
// Prototype   : int GetControlMessageFd(void *info)
// Arguments   : info : InitControlMessage()가 반환한 컨텍스트
// Return      : 수신 소켓 fd (rCtlFd), info가 NULL이면 -1
////////////////////////////////////////////////////////////////////////////////
int GetControlMessageFd(void *info)
{
	if(info) return ((CTLINFO *)info)->rCtlFd;
	return -1;
}
