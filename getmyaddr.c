//
// Description : Get my Host address
// File Name   : getmyaddr.c
// Date        : 2013. 10. 23. (수) 09:29:51 KST
// By          : YJYOON
//
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

/*******************************************************************************
 * Get my host address
 * Prototype : void GetMyHostCharAddress(buff)
 * Argument  : char *buff; Host address buffer
 * Return    : void
 ******************************************************************************/
void GetMyHostCharAddress(char *addr)
{
char name[512];
struct hostent *ent;
	gethostname(name, 512);
	ent = gethostbyname(name);
	strcpy(addr,inet_ntoa(*((struct in_addr *)*ent->h_addr_list)));
	return;
}
