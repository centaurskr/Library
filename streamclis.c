//
// Description : Open Client socket(TCP) by service name
// File Name   : streamclis.c
// Date        : 2013. 10. 23. (수) 09:35:10 KST
// By          : YJYOON
//
#include <stdio.h>
#include <stdlib.h>

#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

#define MAX_BUFF_LEN    4096

////////////////////////////////////////////////////////////////////////////////
// Description : Like OpenInetStreamClient(), but resolves the destination port
//               from a TCP service name (/etc/services) instead of a numeric port.
// Prototype   : int OpenInetStreamClientS(char *host, char *sname)
// Arguments   : host  : hostname or IP address string to connect to
//               sname : TCP service name, e.g. "http"
// Return      : connected socket fd (>=0) on success,
//               -1: gethostbyname() failed, -2: getservbyname() failed,
//               -3: socket() failed, -4: connect() failed
////////////////////////////////////////////////////////////////////////////////
int OpenInetStreamClientS(host, sname)
char    *host;
char    *sname;
{
struct sockaddr_in  serv_addr;
struct hostent     *hp;
struct servent     *sp;
int                 sockfd, buflen, on;
char                buff[256];

	memset(&serv_addr,0x00,sizeof(serv_addr));

	memset(buff, 0x00, 256);
	hp = gethostbyname(host);	
	if(!hp) return -1;
	sp = getservbyname(sname, "tcp");
	if(!sp) return -2;
	serv_addr.sin_family        = AF_INET;
	serv_addr.sin_port          = sp->s_port;
	serv_addr.sin_addr.s_addr   = ((struct in_addr *)(hp->h_addr))->s_addr;
   	if((sockfd=socket(AF_INET,SOCK_STREAM,0)) <0) return -3;

	if(connect(sockfd,(struct sockaddr *) 
						&serv_addr,sizeof(serv_addr)) <0){
		close(sockfd);
		return -4;
	}

	on = 1;
	buflen = MAX_BUFF_LEN;
	setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, (char *)&buflen, sizeof(buflen));
	setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (char *)&on, sizeof(on));
	return sockfd;
}
