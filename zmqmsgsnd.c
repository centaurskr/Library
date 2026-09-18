//
// Description : send() function for zmq with message_t
// File Name   : zmqmsgsnd.c
// Date        : 2021. 10. 05. (화) 16:22:52 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
////////////////////////////////////////////////////////////////////////////////
// 0MQ socket에서 Data를 보낸다
// Copies `sz` bytes from `data` into a zmq_msg_t and sends it (blocking,
// flags=0).
// Prototype : int SendMessage0mq(void *socket, char *data, int size)
// Arguments : void *socket : 송신할 zmq socket
//             char *data   : 송신할 buffer
//             int   sz     : 송신할 byte 수
// Return    : 성공 : 전송한 byte 수, 실패 : -1 (errno 참고)
////////////////////////////////////////////////////////////////////////////////
int SendMessage0mq(void *socket, char *data, int sz)
{
zmq_msg_t message;
int       size;
	zmq_msg_init_size(&message, sz);
	memcpy(zmq_msg_data(&message), data, sz);
	size = zmq_sendmsg(socket, &message, 0);
	zmq_msg_close(&message);
	return size;
}
