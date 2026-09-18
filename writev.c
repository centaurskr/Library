//
// Description : Write Function using iovector
// File Name   : writev.c
// Date        : 2012. 06. 18. (월) 09:28:07 KST
// By          :
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/uio.h>


////////////////////////////////////////////////////////////////////////////////
// Description : 2바이트 길이 헤더(big-endian) + data 본문을 writev()로 한번에 전송
// Prototype   : int WriteV(int fd, char *data, int len)
// Arguments   : int   fd   : 대상 file/socket descriptor
//               char *data : 전송할 데이터
//               int   len  : data 길이(0~65535, 2바이트 헤더에 들어가야 함)
// Return      : writev()의 반환값(전송된 총 바이트 수), 실패 시 -1(errno 참조)
////////////////////////////////////////////////////////////////////////////////
int WriteV(int fd, char *data, int len)
{
int          rtn;
struct iovec iov[2];
unsigned char sz[2];

	sz[0] = (unsigned char )(len >> 8); // len / 256
	sz[1] = (unsigned char )(len % 256);

	iov[0].iov_base = sz;
	iov[0].iov_len  = 2;
	
	iov[1].iov_base = data;
	iov[1].iov_len  = len;

	return  writev(fd,iov,2);
}
