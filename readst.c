//
// Description : Read stream socket 
// File Name   : readst.c
// Date        : 2013. 10. 23.  09:30:40 KST
// By          : YJYOON
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
/*******************************************************************************
 * Read exactly sz bytes from fd into buff, looping over short reads.
 * Prototype : int ReadSize(fd, buff, size)
 * Arguments : int fd    : socket/file descriptor to read from
 *             char *buff: destination buffer, must hold at least sz bytes
 *             int size  : number of bytes to read
 * Return    : 1 on success (all sz bytes read), -1 on read() error or EOF (rtn <= 0)
 ******************************************************************************/
int ReadSize(int fd,char *buff,int sz)
{
int rtn;
char *p;
    p = buff;
    do{
        rtn= read(fd, p, sz);
	if(rtn <=0) return -1;
        if(rtn < sz){ sz -=rtn;p += rtn;}
        else if(rtn == sz) break;
    }while(1);
    return 1;
}
/******************************************************************************/
/* Read data                                                                  */
/* Prototype : int ReadStream(fd, buff)                                       */
/* Arguments : int   fd; Socket FD(I)                                         */
/*             char *buff;Data buffer(O)                                      */
/* Return    : int ; Received data size (payload preceded by a 4-byte length  */
/*             prefix), or <=0 on error/EOF (propagated from ReadSize)        */
/******************************************************************************/
int ReadStream(int fd,char *buff)
{
int sz, rtn;
	rtn = ReadSize(fd, (char *)&sz, sizeof(int));
	if(rtn <= 0) return rtn;
	rtn = ReadSize(fd, (char *)buff, sz);
	if(rtn <= 0) return rtn;
	return sz;
}
/******************************************************************************/
/* Read data                                                                  */
/* Prototype : int ReadStream2(fd, buff)                                      */
/* Arguments : int   fd; Socket FD(I)                                         */
/*             char *buff;Data buffer(O)                                      */
/* Return    : int ; Received data size (payload preceded by a 2-byte         */
/*             big-endian length prefix, max 65535), or <=0 on error/EOF      */
/******************************************************************************/
int ReadStream2(int fd,char *buff)
{
int sz, rtn;
unsigned char size[2];

	rtn = ReadSize(fd, (char *)size, 2);
	if(rtn <= 0) return rtn;
	sz = size[0] * 256 + size[1];
	rtn = ReadSize(fd, buff, sz);
	if(rtn <= 0) return rtn;
	return sz;
}
