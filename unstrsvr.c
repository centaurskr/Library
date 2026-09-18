//
// Description : Open Unix Stream socket for listener.
// File Name   : unstrsvr.c
// Date        : 2013. 10. 23. (수) 09:37:39 KST
// By          : YJYOON
//
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <errno.h>
#include <unistd.h>

#define MAX_BUFF_LEN 4096

////////////////////////////////////////////////////////////////////////////////
// Description : Open a listening Unix domain stream socket at the given path.
//               If bind() fails (e.g. stale socket file), unlinks the path and
//               retries bind() once before giving up. Backlog is 10.
// Prototype   : int OpenUnixStreamServer(char *unistr_path)
// Arguments   : unistr_path : filesystem path to create/bind the Unix domain socket
// Return      : listening socket fd (>=0) on success,
//               -1: socket() failed, -2: bind() failed even after unlink+retry
////////////////////////////////////////////////////////////////////////////////
int OpenUnixStreamServer(char *unistr_path)
{
int                 sockfd;
int                 servlen, buflen, on = 1;
struct sockaddr_un  serv_addr;

	memset(&serv_addr,0,sizeof(serv_addr));
	serv_addr.sun_family        = AF_UNIX;
	strcpy(serv_addr.sun_path,unistr_path);
#if defined(_RS6000_)
	servlen = SUN_LEN(&serv_addr);
#else
	servlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);	
#endif

   	if((sockfd=socket(AF_UNIX,SOCK_STREAM,0)) < 0) return (-1);
	buflen = MAX_BUFF_LEN;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
	if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen) < 0)
	{
		unlink(unistr_path);
		if(bind(sockfd,(struct sockaddr *)&serv_addr,servlen) < 0)
		{
			close(sockfd);
			return(-2);
		}		
	}
	listen(sockfd,10);

	return(sockfd);
}
