//
// Description : read() function for zmq with message_t
// File Name   : zmqmsgrcv.c
// Date        : 2021. 10. 05. (화) 16:22:52 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>
////////////////////////////////////////////////////////////////////////////////
/// 0MQ socket에서 Message Type Data를 읽는다\n
/// return된 point는 꼭 free()해줘야한다.
/// @fn      char *ReceiveMessage0mq(void *socket, int *rlen)
/// @brief   zmq message 수신
/// @param   socket 수신 socket
/// @param   rlen   수신 size buffer
/// @return  수신한 data pointer
////////////////////////////////////////////////////////////////////////////////
char *ReceiveMessage0mq(void *socket, int *rlen)
{
zmq_msg_t message;
int       size;
char      *data;
	zmq_msg_init(&message);
	zmq_recvmsg(socket, &message, 0);
	size = zmq_msg_size(&message);
	data = malloc(size + 1);
	memcpy(data, zmq_msg_data(&message), size);
	zmq_msg_close(&message);
	data[size] = 0x00;
	*rlen = size;
	return data;
}
