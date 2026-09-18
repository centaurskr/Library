//
// Description : set subscribe
// File Name   : zmqsetsub.c
// Date        : 2021. 10. 22. (금) 16:23:12 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <zmq.h>

////////////////////////////////////////////////////////////////////////////////
// SUB socket에 topic filter를 추가(구독)한다 (ZMQ_SUBSCRIBE)
// Prototype : int SetSubTopic0mq(void *socket, char *topic)
// Arguments : void *socket : SUB socket
//             char *topic  : 구독할 topic 문자열 (빈 문자열 ""이면 전체 구독)
// Return    : 성공 : 1, 실패 : 0
////////////////////////////////////////////////////////////////////////////////
int SetSubTopic0mq(void *socket, char *topic)
{
int rtn;
	rtn = zmq_setsockopt(socket, ZMQ_SUBSCRIBE, topic, strlen(topic));
	if(rtn) return 0;
	return 1;
}

////////////////////////////////////////////////////////////////////////////////
// SUB socket에서 topic filter를 제거(구독 해제)한다 (ZMQ_UNSUBSCRIBE)
// Prototype : int SetUnsubTopic0mq(void *socket, char *topic)
// Arguments : void *socket : SUB socket
//             char *topic  : 구독 해제할 topic 문자열
// Return    : 성공 : 1, 실패 : 0
////////////////////////////////////////////////////////////////////////////////
int SetUnsubTopic0mq(void *socket, char *topic)
{
int rtn;
	rtn = zmq_setsockopt(socket, ZMQ_UNSUBSCRIBE, topic, strlen(topic));
	if(rtn) return 0;
	return 1;
}
