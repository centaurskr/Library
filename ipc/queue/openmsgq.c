////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:55:27 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

/*****************************************************************/
/* Get Message Queue ID for Client processor.                    */
/* PROTOTYPE : int OpenMsgqueue(key)                             */
/* ARGUMENT  :                                                   */
/*           key_t key; Message Access key (unsigned long).      */
/* RETURN    : int                                               */
/*           rtn < 0  : Can't get Message Queue ID.              */
/*           rtn > 0  : Message Queue ID.                        */
/* LIBRARY   : libsaipc.a                                        */
/*****************************************************************/
int OpenMsgqueue(key)
key_t          key;
{
int id;
	id = msgget(key, 0);
	if(id < 0) return -1;
	return id;
}
