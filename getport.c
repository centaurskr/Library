//
// Description : get service port number(host byte order) MT_Unsafe
// File Name   : getport.c
// Date        : 2017. 07. 19. (수) 15:56:21 KST
// By          : centaurskr@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Look up a service's port number via getservbyname() (/etc/services)
// Prototype   : int GetPortNumber(char *name, char *proto)
// Arguments   : name  : service name, e.g. "http"
//               proto : protocol name, e.g. "tcp" or "udp"
// Return      : port number in host byte order on success, -1: fail (service not found)
//////////////////////////////////////////////////////////////////////////////
int GetPortNumber(char *name, char *proto)
{
struct servent *sp;

	sp = getservbyname(name, proto);
	if(!sp) return -1;
	return (int)ntohs(sp->s_port);
}
