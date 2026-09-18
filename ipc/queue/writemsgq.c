////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:56:00 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#ifndef	_ERRNO_H_
#include <errno.h>
#endif
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

typedef struct _MSGQDATA{
	int  type;
	char data[4096];
}MSGQDATA;
/*****************************************************************/
/* Write data to Message Queue                                   */
/* PROTOTYPE : int WriteMsgqueue(id, type, size, mode, buff)     */
/* ARGUMENT  :                                                   */
/*           int   id;    Message Queue ID                       */
/*           int   type;  Message Type                           */
/*           int   size;  Data length.                           */
/*           int   mode;  WAIT : 1, NO_WAIT : 0                  */
/*           char *buff;  Send Data buffer Pointer               */
/* RETURN    : int                                               */
/*           rtn < 0  : Message Queue was removed,               */
/*                       or Error occured(errno)                 */
/*           rtn  = 1  : Write ok.                               */
/* LIBRARY   : libsaipc.a                                        */
/* REMARKS   : size must be <= sizeof(data.data) (4096 bytes) -  */
/*             it is memcpy'd into a fixed 4096-byte struct      */
/*             member with no bounds check.                     */
/*****************************************************************/
int WriteMsgqueue(int id,int type,int size,int mode,char *buff)
{
MSGQDATA data;
int      rtn, flag =0;
	memset((char *)&data, 0x00, sizeof(MSGQDATA));
	data.type = type;
	memcpy(data.data, buff, size);
	if(!mode) flag = IPC_NOWAIT;
	rtn = msgsnd(id, (void *)&data, size, flag);
	if(rtn < 0) return (errno * -1);
	return 1;
}
