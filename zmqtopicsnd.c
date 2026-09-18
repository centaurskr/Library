//
// Description :
// File Name   : zmqtopicsnd.c
// Date        : 2021. 10. 07. (목) 15:33:54 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
//  Send data based in Topic
// Prototype : int SendTopic0mq(void *socket, char *topic, char *data, int len)
// Arguments : void *socket : zmq socket
//             char *topic  : topic name
//             char *data   : data for sending
//             int   len    : data length
// Return    : topic 전송 실패 : -1 (data 프레임은 시도하지 않음)
//             topic 전송 성공 후 : zmq_send()의 data 프레임 전송 결과를 그대로
//             반환 (성공 시 전송 byte 수, 실패 시 -1)
////////////////////////////////////////////////////////////////////////////////
int SendTopic0mq(void *socket, char *topic, char *data, int len)
{
int rtn;
	rtn = zmq_send(socket, topic, strlen(topic), ZMQ_SNDMORE);	
	if(rtn < 0) return -1;
	rtn = zmq_send(socket, data, len, 0);
	return rtn;
}
