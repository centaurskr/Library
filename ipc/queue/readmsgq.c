////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:55:40 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MAX_QLEN 4096

typedef struct _MSGQDATA{
	int  type;
	char data[4096];
}MSGQDATA;
/*****************************************************************/
/* Read Data from Message Queue                                  */
/* PROTOTYPE : int ReadMsgqueue(id, type, rtype, mode, buff)     */
/* ARGUMENT  :                                                   */
/*           int   id;    Message Queue ID                       */
/*           int   type;  Message Type for read.                 */
/*           int  *rtype; Received data type.                    */
/*           int   mode;  WAIT : 1, NO_WAIT : 0                  */
/*           char *buff;  Received Data buffer                   */
/* RETURN    : int                                               */
/*           rtn < 0  : Message Queue was removed,               */
/*                       or Error occured.                       */
/*           rtn > 0  : Read size                                */
/*           rtn = 0  : if mode = NO_WAIT, did not received data.*/
/* LIBRARY   : libsaipc.a                                        */
/*****************************************************************/
int ReadMsgqueue(int id,int type,int *rtype,int mode,char *buff)
{
MSGQDATA data;
int     rtn, flag = 0;
	flag = MSG_NOERROR;
	if (!mode) flag |= IPC_NOWAIT;
	rtn = msgrcv(id, &data, MAX_QLEN, type, flag);
#if defined(_RS6000_)
	if(rtn < 0 && errno == ENOMSG) return 0;
	else if(rtn < 0) return -1; /* in AIX 4.3.1 99.01.22 */
#else
	if(rtn < 0 && errno == ENOMSG) return 0;
#endif

	memcpy(buff, data.data, rtn);
	*rtype =  data.type;
	return rtn;
}
