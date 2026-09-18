//
// Description :
// File Name   : zmqtopicrcv.c
// Date        : 2021. 10. 07. (목) 15:42:25 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
//  Receive data based in Topic
//  zmq_setsockopt(.... ZMQ_SUBSCRIBE, topic, ...)가 이미 설정되어야 함
// Prototype : int ReceiveTopic0mq
//                 (void *socket, char *topic, char *buffer, int size)
// Arguments : void *socket : zmq socket
//             char *topic  : [OUT] 수신한 topic을 담을 buffer (최대 512byte,
//                            strncpy로 채워지며 null terminate 되지 않음)
//             char *buffer : [OUT] 수신한 data를 담을 buffer
//             int   size   : buffer(data) size
// Return    : topic 수신 실패 : -1 (data 프레임은 시도하지 않음)
//             topic 수신 성공 후 : zmq_recv()의 data 프레임 수신 결과를 그대로
//             반환 (성공 시 수신 byte 수, 실패 시 -1)
////////////////////////////////////////////////////////////////////////////////
int ReceiveTopic0mq(void *socket, char *topic, char *buffer, int size)
{
int rtn;
char buff[512];
	memset(buff, 0x00, 512);
	rtn = zmq_recv(socket, buff, 512, 0);
	if(rtn < 0) return -1;
	strncpy(topic, buff, rtn);
	rtn = zmq_recv(socket, buffer, size, 0);
	return rtn;
}
