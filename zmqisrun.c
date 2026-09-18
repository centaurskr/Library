//
// Description :
// File Name   : zmqisrun.c
// Date        : 2021. 11. 05. (금) 13:27:19 KST
// By  : Cento
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// 중복 실행을 check 하는 function 
// Prototype : int IsRunning0mq(void *context, int number)
// Arguments : void *context  : zmq context
//              int  number   : PROC_NUMBER + channel
// Return    : 1:Dual, 0:중복실행 아님, -1 : Error
////////////////////////////////////////////////////////////////////////////////
int IsRunning0mq(void *context, int number)
{
int rtn;
void *socket;
char  dev[50];
	socket = zmq_socket(context, ZMQ_PUB);
	if(!socket) return -1;
	sprintf(dev, "tcp://*:%d", number);
	rtn = zmq_bind(socket, dev);
	if(rtn) return 1;
	return 0;

}
