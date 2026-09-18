///
/// @brief  :
/// @file   : zmqsockmon.c
/// @date   : 2024. 09. 27. (금) 14:53:41 KST
/// @author : Cento
///
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
/// Monitor용 socket을 만든다
/// @fn      void *MakeMonitorSocket0mq(void *ctx, void *sock)
/// @param   ctx    : Zmq context
/// @param   socket : Monitoring 할 socket
/// @return  monitor socket, NULL
////////////////////////////////////////////////////////////////////////////////
void *MakeMonitorSocket0mq(void *ctx, void *sock)
{
void *monsocket;
char ipcname[255];
struct timespec tm;
int    rtn;
	clock_gettime(CLOCK_REALTIME, &tm);	
	sprintf(ipcname, "inproc://mon%ld%ld", tm.tv_sec, tm.tv_nsec);

	rtn = zmq_socket_monitor(sock, ipcname, ZMQ_EVENT_ALL);
	if(rtn < 0) return NULL;

	monsocket = zmq_socket(ctx, ZMQ_PAIR);
	if(monsocket == NULL) return NULL;

	rtn = zmq_connect(monsocket, ipcname);
	if(rtn != 0) return NULL;

	return monsocket;
}
