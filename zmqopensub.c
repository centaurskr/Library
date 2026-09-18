//
// Description : Open zmq socket for subscribe
// File Name   : zmqopensub.c
// Date        : 2021. 10. 22. (금) 16:23:12 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// Open zmq socket for subscribe
// Note: does not set ZMQ_SUBSCRIBE — by default a SUB socket receives
// nothing until a topic filter is set, so call SetSubTopic0mq() (see
// zmqsetsub.c) on the returned socket before expecting messages.
// Prototype : void *OpenSubSocket0mq(void *ctx, char *sdev)
// Arguments : void *ctx  : zmq context
//             char *sdev : SUB가 connect할 PUB endpoint
// Return    : 성공 : SUB socket, 실패(zmq_socket/zmq_connect 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *OpenSubSocket0mq(void *ctx, char *sdev)
{
int rtn;
void *socket;
	socket = zmq_socket(ctx, ZMQ_SUB);
	if(!socket) return NULL;
	rtn = zmq_connect(socket, sdev);
	if(rtn) return NULL;
	return socket;
}

