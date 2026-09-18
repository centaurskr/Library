///
/// ZMQ socket에서 socket fd를 구한다
/// @file zmqgetfd.c
/// @date 2024. 01. 05. (금) 18:11:06 KST
/// @author Cento 
///
#include <stdio.h>
#include <stdlib.h>
#include <zmq.h>
////////////////////////////////////////////////////////////////////////////////
/// ZMQ socket에서 socket fd를 구한다
/// @fn        int GetFdFromZmqSocket(void *socket)
/// @brief     socket fd 구하기
/// @param     socket  zmq socket
/// @return    성공 : socketfd, 실패: -1
////////////////////////////////////////////////////////////////////////////////
int GetFdFrom0mqSocket(void *socket)
{
int zmq_fd, rtn;
size_t fd_size = sizeof(int);
	rtn = zmq_getsockopt(socket, ZMQ_FD, &zmq_fd, &fd_size);
	if(rtn) return -1;
	return zmq_fd;
}
