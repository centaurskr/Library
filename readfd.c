//
// Description :
// File Name   : readfd.c
// Date        : 2013. 11. 01. (금) 11:16:56 KST
// By          :
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
//  FD 수신 - receive a file descriptor passed over a Unix domain socket
//  (SCM_RIGHTS ancillary data), along with an ordinary data payload.
// Prototype : int ReadFd(int fd, char *buf, size_t buflen)
// Arguments : fd     : Unix domain socket to receive from
//             buf    : buffer to receive the accompanying data payload into
//             buflen : size of buf
// Return    : received file descriptor (>=0) on success,
//             -errno if recvmsg() failed, -2 if no SCM_RIGHTS control message arrived
////////////////////////////////////////////////////////////////////////////////
int ReadFd(int fd, char *buf, size_t buflen)
{
int n;
int recvfd;
char ptr;

struct iovec iov[1];
struct cmsghdr *cmptr;
struct msghdr msg;

union {
    struct cmsghdr cm;
    char control[CMSG_SPACE(sizeof(int))];
} control_un;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = buf;
	iov[0].iov_len = buflen;

	msg.msg_iov = iov;
	msg.msg_iovlen = 1;


	if ( (n = recvmsg(fd, &msg, 0)) <= 0) return -1 * errno;

	cmptr = CMSG_FIRSTHDR(&msg); 
	if (cmptr == NULL) return -2;

	recvfd = *((int *) CMSG_DATA(cmptr));
	return recvfd;
}
