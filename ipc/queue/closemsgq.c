////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:54:51 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/******************************************************************************/
/* Destroy message queue                                                      */
/* Library in   : libsaipc.a                                                  */
/* Prototype    : int CloseMsgqueue(id)                                       */
/* Arguments    : int              id;  Queue ID                              */
/* Return value : int  Result of msgctl(IPC_RMID): 0 on success, -1 on error  */
/******************************************************************************/
int CloseMsgqueue(int id)
{
	return msgctl(id, IPC_RMID, (struct msqid_ds *)0);
}

