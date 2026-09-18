//
// Description :
// File Name   : zmqctlmsg.c
// Date        : 2021. 10. 15. (금) 10:16:05 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

static void *CtlPubSocket = NULL;
static int MyId;
typedef struct _HD{ int  a; int b; int cmd;}HD;
////////////////////////////////////////////////////////////////////////////////
// Control Message 전송 with ZMQ
// Prototype : int SendControlMsg0mq(int to, int cmd, char *msg)
// Arguments : int   toid : Destination id
//             int   cmd  : Destination id
//             char *buff : control message
// Return    : send size
////////////////////////////////////////////////////////////////////////////////
int SendControlMsg0mq(int to, int cmd, char *msg)
{
int rtn;
HD hd;
	if(CtlPubSocket == NULL) return 0;
	rtn = zmq_send(CtlPubSocket, "CONTROL", 7, ZMQ_SNDMORE);	
	if(rtn < 0) return -1;
	hd.a   = MyId;
	hd.b   = to;
	hd.cmd = cmd;
	rtn = zmq_send(CtlPubSocket, (char *)&hd, sizeof(HD), ZMQ_SNDMORE);
	rtn = zmq_send(CtlPubSocket, msg, strlen(msg), 0);
	return rtn;
}
////////////////////////////////////////////////////////////////////////////////
// Control Message 수신 with ZMQ
// Prototype : int ReceiveControlMsg0mq(void *socket, void *hd,
//                                 char *buff, int size)
// Arguments : void *socket : control socket
//             void *hd     : Mesasge header(HD) -- OUTPUT
//             char *buff   : control buffer
//             int   size   : buffer size
// Return    : send size
////////////////////////////////////////////////////////////////////////////////
int ReceiveControlMsg0mq(void *socket, void *hd, char *buff, int size)
{
int rtn;
char tmp[1024];
    rtn = zmq_recv(socket, tmp, 1024, 0); // TOPIC "CONTROL"
	if(rtn < 0) return -1; 
	rtn = zmq_recv(socket, (char *)hd, sizeof(HD), 0);
	rtn = zmq_recv(socket, buff, size, 0);
	return rtn;
}
////////////////////////////////////////////////////////////////////////////////
// Control Message PUB/SUB socket 초기화
// Prototype : void *MakeControlMsgSocket0mq(void *ctx, int idx, 
//                                           char *pdev, char *sdev)
// Arguments : void *ctx  : zmq context
//             int   idx  : Process ID(flybitdt.h)
//             char *pdev : dev for publication CTL-Message
//             char *sdev : dev for subscribing CTL_Message
// Return    : Socket for subscribing
////////////////////////////////////////////////////////////////////////////////
void *MakeControlMsgSocket0mq(void *ctx, int idx, char *pdev, char *sdev)
{
int rtn;
void *socket;
	CtlPubSocket = zmq_socket(ctx, ZMQ_PUB);
	rtn = zmq_connect(CtlPubSocket, pdev);
	if(rtn) return NULL;

	socket = zmq_socket(ctx, ZMQ_SUB);
	rtn = zmq_connect(socket, sdev);
	if(rtn) return NULL;

	MyId = idx;

	zmq_setsockopt(socket, ZMQ_SUBSCRIBE, "CONTROL", 7);

	sleep(1);

	return socket;
}

////////////////////////////////////////////////////////////////////////////////
// 
// Prototype :  void CloseControlMsg0mq(void *socket)
// Arguments :  void *socket : SUB socket
// Return    : 
////////////////////////////////////////////////////////////////////////////////
void CloseControlMsg0mq(void *socket)
{
	if(CtlPubSocket) zmq_close(CtlPubSocket);
	zmq_close(socket);
	return ;
}
