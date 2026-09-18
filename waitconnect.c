//
// Description : Wait connection
// File Name   : waitconnect.c
// Date        : 2013. 10. 23. (수) 09:38:13 KST
// By          : YJYOON
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <errno.h>
#include <string.h>

/******************************************************************************/
/* Wait Client Connection                                                     */
/* Prototype : int WaitConnect(fd, buff)                                      */
/* Arguments : int   fd; int socket discriptor                                */
/*             char *buff; Client address buffer                              */
/* Return    : new socket fd                                                  */
/******************************************************************************/
int WaitConnect(int fd, char *buff)
{
int                newfd, clilen;
struct sockaddr_in cli_addr;
unsigned char     *p;

	clilen = sizeof(cli_addr);
	newfd=accept(fd,
		(struct sockaddr *)&cli_addr,
		(socklen_t *)&clilen);
	if(buff == NULL) return newfd;
	p = (unsigned char *)&cli_addr.sin_addr;
	sprintf(buff, "%3d.%3d.%3d.%3d", *p, *(p+1), *(p+2), *(p+3));
	return newfd;
}
