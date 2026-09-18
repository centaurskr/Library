//
// Description : Open Listen socket(TCP)
// File Name   : streamsvr.c
// Date        : 2013. 10. 23. (수) 09:36:10 KST
// By          : YJYOON
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <errno.h>
#include <unistd.h>

#define MAX_BUFF_LEN   4096

////////////////////////////////////////////////////////////////////////////////
// Description : Open a listening TCP socket bound to INADDR_ANY on the given
//               port. Sets SO_REUSEADDR/SO_KEEPALIVE and a backlog of 200.
// Prototype   : int OpenInetStreamServer(int port)
// Arguments   : port : port number to bind to (host byte order)
// Return      : listening socket fd (>=0) on success,
//               -3001: socket() failed, -1: bind() failed
////////////////////////////////////////////////////////////////////////////////
int OpenInetStreamServer(int port)
{
int                 sockfd, buflen, on = 1;
struct sockaddr_in serv_addr;
struct servent     *sp;

	memset(&serv_addr,0x00,sizeof(serv_addr));
	serv_addr.sin_family        = AF_INET;
	serv_addr.sin_port          =htons(port);
	serv_addr.sin_addr.s_addr   = htonl(INADDR_ANY);

   	if((sockfd=socket(AF_INET,SOCK_STREAM,0)) <0) return -3001;  /* -3001 */

	on = 1;
	buflen = MAX_BUFF_LEN;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
	on = bind(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr));
	if(on <0){ close(sockfd); return -1; }
	listen(sockfd,200);
	return sockfd;
}
