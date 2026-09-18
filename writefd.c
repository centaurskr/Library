//
// Description :
// File Name   : writefd.c
// Date        : 2013. 11. 01. (금) 11:17:18 KST
// By          :
//
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////////
//  FD 전송 - send a file descriptor over a Unix domain socket (SCM_RIGHTS
//  ancillary data), along with an ordinary data payload. Pairs with ReadFd().
// Prototype : int WriteFd(int fd, int sendfd, void *ptr, size_t nbytes)
// Arguments : fd     : Unix domain socket to send over
//             sendfd : the file descriptor to pass to the receiver
//             ptr    : data payload to send alongside the fd
//             nbytes : size of the data payload
// Return    : number of bytes sent on success (as returned by sendmsg()), -1 on error (errno set)
////////////////////////////////////////////////////////////////////////////////
int WriteFd(int fd, int sendfd, void *ptr, size_t nbytes)
{
struct msghdr	msg;
struct iovec	iov[1];
union {
    struct cmsghdr cm;
    char  control[CMSG_SPACE(sizeof(int))];
} control_un;
struct cmsghdr *cmptr;

	msg.msg_control = control_un.control;
	msg.msg_controllen = sizeof(control_un.control);

	cmptr = CMSG_FIRSTHDR(&msg);
	cmptr->cmsg_len = CMSG_LEN(sizeof(int));
	cmptr->cmsg_level = SOL_SOCKET;
	cmptr->cmsg_type = SCM_RIGHTS;
	*((int *) CMSG_DATA(cmptr)) = sendfd;
	msg.msg_name = NULL;
	msg.msg_namelen = 0;

	iov[0].iov_base = ptr;
	iov[0].iov_len = nbytes;
	msg.msg_iov = iov;
	msg.msg_iovlen = 1;
	return(sendmsg(fd, &msg, 0));
}
