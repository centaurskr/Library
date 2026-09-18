///
/// read() function for zmq with topic using message_t
/// @file zmqtopicmsgrcv.c
/// @date  2021. 11. 18. (목) 17:36:58 KST
/// @author Cento 
///

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
/// 0MQ socket에서 Message형태 Data를 읽는다 (topic 프레임 + data 프레임 순서 수신)\n
/// return된 point는 꼭 free()해줘야한다.
/// @fn      char *ReceiveTopicMessage0mq(void *socket, char *topic, int *rlen)
/// @brief   zmq message data 수신 (topic based)
/// @param   socket 수신 socket
/// @param   topic  [OUT] 수신한 topic을 담을 buffer. 길이 인자가 없으므로
///                 호출자가 충분히 큰 buffer를 준비해야 하며, null
///                 terminate되지 않는다 (crlf 없음, 수신 크기만큼만 채워짐)
/// @param   rlen   [OUT] 수신한 data(topic 다음 프레임) size
/// @return 수신 DATA Pointer
////////////////////////////////////////////////////////////////////////////////
char *ReceiveTopicMessage0mq(void *socket, char *topic, int *rlen)
{
zmq_msg_t message, tpmsg;
int       size, tlen;
char      *data;
	// Topic
	zmq_msg_init(&tpmsg);
	zmq_recvmsg(socket, &tpmsg, 0);
	tlen = zmq_msg_size(&tpmsg);
	memcpy(topic, zmq_msg_data(&tpmsg), tlen);
	zmq_msg_close(&tpmsg);

	// Data
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
