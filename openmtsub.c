//
// Description :
// File Name   : openmtsub.c
// Date        : 2017. 06. 30. (금) 15:20:58 KST
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
// Description : open multicasting socket for subscribing message
// Prototype   : int OpenMtSubscribe(dev, group, port)
// Arguments   : const char         *dev   - network device name
//               const char         *group - multicasting group (224.x.x.x)
//               int                 port  - 
// Return      : positive number(include 0) that is a socketfd for success
//               -1 : socket() error
//               -2 : bind() error
//               -3 : ioctl() for interface name error
//               -4 : setsockopt(IP_ADDMEMBERSHIP) error
////////////////////////////////////////////////////////////////////////////////
int OpenMtSubscribe(const char *dev, const char *group, int port)
{
int                sfd, on = 1, rtn;
struct sockaddr_in addr;
struct ip_mreq     imr;
struct ifreq       ifreq;
	
	sfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sfd < 0) return -1;
	setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, (char *) &on, sizeof(on));
	setsockopt(sfd, SOL_SOCKET, SO_REUSEPORT, (char *) &on, sizeof(on));

	addr.sin_family      = AF_INET;
	addr.sin_port        = htons(port);
	addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) < 0)
		return -2;

	imr.imr_multiaddr.s_addr = inet_addr(group);
	if(dev != NULL){
		memset(&ifreq, 0x00, sizeof(ifreq));
		strncpy(ifreq.ifr_name, dev, IFNAMSIZ);
		rtn = ioctl(sfd, SIOCGIFADDR, &ifreq);
		if(rtn < 0) return -3;
		imr.imr_interface.s_addr = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr.s_addr;
	}else
		imr.imr_interface.s_addr = htonl(INADDR_ANY);
	rtn = setsockopt(sfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *) &imr,
			sizeof(struct ip_mreq));
	if(rtn < 0) return -4;

	return sfd;
}
