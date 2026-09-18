////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:55:06 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define PERMS 0666    /* -wr-wr-wr- */
/******************************************************************************/
/* Change Max bytes of Message Queue                                          */
/* Library in   : libsaipc.a                                                  */
/* Prototype    : int ChangeQueueSize(id, size)                               */
/* Arguments    : unsigned short size;  Queue size                            */
/*                int              id;  Queue ID                              */
/* Return value : int   0 : msgctl(IPC_STAT) failed for id                    */
/*                       1 : ok (size only raised if current size < size)     */
/******************************************************************************/
int ChangeQueueSize(int id, unsigned short size)
{
struct msqid_ds st;
	if(msgctl(id, IPC_STAT, &st) < 0)return 0;
	if(st.msg_qbytes < size){
		st.msg_qbytes = size;
		msgctl(id, IPC_SET, &st);
	}
	return 1;
}

/******************************************************************************/
/* Make message queue                                                         */
/* Library in   : libsaipc.a                                                  */
/* Prototype    : int MakeMsgqueue(key, size)                                 */
/* Arguments    : key_t key;            IPC key (int)                        */
/*                unsigned short size;  Queue size, passed to                */
/*                                      ChangeQueueSize() after creation      */
/* Return value : int  Queue ID, or -1 on error                               */
/* CAUTION      : If a queue already exists at `key`, it is destroyed         */
/*                (msgctl IPC_RMID) and a fresh empty queue is created in     */
/*                its place - any pending messages on the old queue are lost. */
/******************************************************************************/
int MakeMsgqueue(key_t key, unsigned short size)
{
int id;
	id = msgget(key, 0);
	if(id > 0) msgctl(id, IPC_RMID, (struct msqid_ds *)0);
	id = msgget(key, PERMS | IPC_CREAT);
	if(!id){
		msgctl(id, IPC_RMID, (struct msqid_ds *)0);
		id = msgget(key, PERMS | IPC_CREAT);
	}
	if(id < 0) return -1;
	ChangeQueueSize(id, size);
	return id;
}
