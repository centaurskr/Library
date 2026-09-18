//
// Description : send() function for zmq with topic using message_t
// File Name   : zmqtopicmsgsnd.c
// Date        : 2021. 11. 18. (목) 17:29:17 KST
// By  : Cento
//


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
////////////////////////////////////////////////////////////////////////////////
// 0MQ socket으로 topic 프레임 + data 프레임을 순서대로 보낸다 (multipart message)
// Prototype : int SendTopicMessage0mq(void *socket, char *topic, char *data, int size)
// Arguments : void *socket : 송신할 zmq socket (보통 PUB)
//             char *topic  : 첫 프레임으로 보낼 topic 문자열
//             char *data   : 두번째 프레임으로 보낼 data buffer
//             int   sz     : data buffer 길이(byte)
// Return    : 성공 : data 프레임 전송 byte 수, 실패 : -1 (errno 참고)
////////////////////////////////////////////////////////////////////////////////
int SendTopicMessage0mq(void *socket, char *topic, char *data, int sz)
{
zmq_msg_t message, tpmsg;
int       size, rtn, tlen;
	//Topic 
	tlen = strlen(topic);
	zmq_msg_init_size(&tpmsg, tlen);
	memcpy(zmq_msg_data(&tpmsg), topic, tlen);
	rtn = zmq_sendmsg(socket, &tpmsg, ZMQ_SNDMORE);

	// Message
	zmq_msg_init_size(&message, sz);
	memcpy(zmq_msg_data(&message), data, sz);
	size = zmq_sendmsg(socket, &message, 0);

	zmq_msg_close(&tpmsg);
	zmq_msg_close(&message);
	return size;
}
