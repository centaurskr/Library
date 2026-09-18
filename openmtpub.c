//
// Description :
// File Name   : openmtpub.c
// Date        : 2017. 06. 30. (금) 10:47:55 KST
// By          : centaurskr@gmail.com
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sys/ioctl.h>
#include <net/if.h>
#include <netdb.h>

////////////////////////////////////////////////////////////////////////////////
// Description : open multicasting socket for publishing message
// Prototype   : int OpenMtPublish(addr, dev, group, port, ttl)
// Arguments   : struct sockaddr_in *addr  - To be used later on "sendto()"
//               const char         *dev   - network device name
//               const char         *group - multicasting group (224.x.x.x)
//               int                 port  - 
//               int                 ttl   - Value of Time to live (1:local)
// Return      : positive number(include 0) that is a socketfd for success
//               -1 : socket() error
//               -2 : setsockopt(TTL) error
//               -3 : ioctl() for interface name error
//               -4 : setsockopt(IP_MULTICAST_IF) error
////////////////////////////////////////////////////////////////////////////////
int OpenMtPublish(struct sockaddr_in *addr, const char *dev, const char *group, int port, int ttl)
{
int sfd, rtn;
struct in_addr localif;
struct ifreq   ifreq;

	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd < 0) return -1;
	memset((void *)addr, 0x00, sizeof(struct sockaddr_in));

	addr->sin_port = htons(port);
	addr->sin_addr.s_addr = inet_addr(group);
	addr->sin_family = AF_INET;
	rtn = setsockopt(sfd, IPPROTO_IP, IP_MULTICAST_TTL, (void *)&ttl, sizeof(ttl));
	if(rtn < 0) return -2;
	if(dev != NULL){
		memset(&ifreq, 0x00, sizeof(ifreq));
		strncpy(ifreq.ifr_name, dev, IFNAMSIZ);
		rtn = ioctl(sfd, SIOCGIFADDR, &ifreq);
		if(rtn < 0) return -3;
		localif.s_addr = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
		rtn = setsockopt(sfd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localif, sizeof(localif));
		if(rtn < 0) return -4;
	}

	return sfd; 
}
