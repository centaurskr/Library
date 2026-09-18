//
// Description : Open zmq socket for REP socket
// File Name   : zmqopenrep.c
// Date        : 2021. 11. 05. (금) 14:31:40 KST
// By  : Cento
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// Open zmq socket for REP
// Note: connects (zmq_connect) rather than binds — this REP socket is meant
// to dial in to a broker/proxy endpoint rather than bind its own address.
// Prototype : void *OpenRepSocket0mq(void *ctx, char *pdev)
// Arguments : void *ctx  : zmq context
//             char *pdev : REP가 connect할 endpoint
// Return    : 성공 : REP socket, 실패(zmq_socket/zmq_connect 실패) : NULL
////////////////////////////////////////////////////////////////////////////////
void *OpenRepSocket0mq(void *ctx, char *pdev)
{
int rtn;
void *socket;
	socket = zmq_socket(ctx, ZMQ_REP);
	if(!socket) return NULL;
	rtn = zmq_connect(socket, pdev);
	if(rtn) return NULL;
	return socket;
}

