//
// Description : Open zmq socket for REQ socket
// File Name   : zmqopenreq.c
// Date        : 2022. 05. 12. (목) 18:31:40 KST
// By  : Cento
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// Open zmq socket for REQ
// Prototype : void *OpenReqSocket0mq(void *ctx, char *pdev)
// Arguments : void *ctx  : zmq context
//             char *pdev : REQ가 connect할 endpoint (짝이 되는 REP/ROUTER)
// Return    : 성공 : REQ socket, 실패(zmq_socket/zmq_connect 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *OpenReqSocket0mq(void *ctx, char *pdev)
{
int rtn;
void *socket;
	socket = zmq_socket(ctx, ZMQ_REQ);
	if(!socket) return NULL;
	rtn = zmq_connect(socket, pdev);
	if(rtn) return NULL;
	return socket;
}

