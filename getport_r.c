//
// Description : get service port number(host byte order) MT_safe
// File Name   : getport_r.c
// Date        : 2017. 07. 19. (수) 15:56:21 KST
// By          : centaurskr@gmail.com
//

#ifndef _MAC_

#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netdb.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Thread-safe service port lookup via getservbyname_r() (not built on _MAC_)
// Prototype   : int GetPortNumberR(char *name, char *proto)
// Arguments   : name  : service name, e.g. "http"
//               proto : protocol name, e.g. "tcp" or "udp"
// Return      : port number in host byte order on success, -1: fail (service not found)
//////////////////////////////////////////////////////////////////////////////
int GetPortNumberR(char *name, char *proto)
{
struct servent sv, *sp;
char buff[1024];
int  rtn;
	rtn = getservbyname_r(name, proto, &sv, buff, 1024, &sp);
	if(rtn) return -1;
	return (int)ntohs(sv.s_port);
}
#endif
