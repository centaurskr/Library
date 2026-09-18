//
// Description : Open zmq socket for PULL socket
// File Name   : zmqopenpull.c
// Date        : 2021. 11. 05. (금) 14:31:40 KST
// By  : Cento
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// Open zmq socket for PULL
// Prototype : void *OpenPullSocket0mq(void *ctx, char *pdev)
// Arguments : void *ctx  : zmq context
//             char *pdev : dev for  PULL
// Return    : 성공 : PULL socket, 실패(zmq_socket/zmq_connect 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *OpenPullSocket0mq(void *ctx, char *pdev)
{
int rtn;
void *socket;
	socket = zmq_socket(ctx, ZMQ_PULL);
	if(!socket) return NULL;
	rtn = zmq_connect(socket, pdev);
	if(rtn) return NULL;
	return socket;
}

