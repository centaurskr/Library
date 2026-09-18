//
// Description : Open Unix stream socket for client.
// File Name   : unstrcli.c
// Date        : 2013. 10. 23. (수) 09:36:46 KST
// By          : YJYOON
//
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <errno.h>

#define MAX_BUFF_LEN 4096

////////////////////////////////////////////////////////////////////////////////
// Description : Open a Unix domain stream socket connected to a server listening
//               on the given socket path. Sets SO_REUSEADDR/SO_KEEPALIVE.
// Prototype   : int OpenUnixStreamClient(char *unistr_path)
// Arguments   : unistr_path : filesystem path of the target Unix domain socket
// Return      : connected socket fd (>=0) on success,
//               -1: socket() failed, -2: connect() failed
////////////////////////////////////////////////////////////////////////////////
int OpenUnixStreamClient(char *unistr_path)
{
struct sockaddr_un  serv_addr;
int                 sockfd, buflen, on;
int                 sverlen;

	memset(&serv_addr,0,sizeof(serv_addr));

	serv_addr.sun_family        = AF_UNIX;
	strcpy(serv_addr.sun_path,unistr_path);
#if defined(_RS6000_)
	sverlen = SUN_LEN(&serv_addr);
#else
	sverlen = strlen(serv_addr.sun_path) + sizeof(serv_addr.sun_family);	
#endif
   	if((sockfd=socket(AF_UNIX,SOCK_STREAM,0)) < 0) return (-1); 
	on = 1;
	buflen = MAX_BUFF_LEN;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr)) < 0){
		close(sockfd);
		return (-2);
	}
	return (sockfd);
}
