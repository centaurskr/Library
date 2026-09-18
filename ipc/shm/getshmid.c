////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 14:26:08 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>


/*#@
**	Function 	: Get shmid 
**	Syntax   	: int Getshmid(key_t shmkey);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**				  if not shmkey exist error 
**	Return Value: On success , positive value int type shmid 
**				  Otherwise,   error -1
**  See also 	: Getshmid Openshm Makeshm Attachshm Detachshm Removeshm 
**			      		
**#$ 
*/

int Getshmid(key_t shmkey)
{
	int shmid;
	shmid = shmget(shmkey,0,0666|IPC_EXCL);
	if(shmid==-1) return(-1);
	return shmid; 
}

