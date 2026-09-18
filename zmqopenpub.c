//
// Description : Open zmq socket for publish
// File Name   : zmqopenpub.c
// Date        : 2021. 10. 22. (금) 16:23:12 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// Open zmq socket for publish
// Prototype : void *OpenPubSocket0mq(void *ctx, char *pdev)
// Arguments : void *ctx  : zmq context
//             char *pdev : dev for publication 
// Return    : Socket for publication
// - Sleep 위치 변경. Connect 다음으로
////////////////////////////////////////////////////////////////////////////////
void *OpenPubSocket0mq(void *ctx, char *pdev)
{
int rtn;
void *socket;
	socket = zmq_socket(ctx, ZMQ_PUB);
	if(!socket) return NULL;
	rtn = zmq_connect(socket, pdev);
	if(rtn) return NULL;
	usleep(1000);
	return socket;
}

