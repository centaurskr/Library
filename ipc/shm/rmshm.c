////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 14:27:11 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

extern int Getshmid();

/*#@
**	Function 	: Remove shared memory by shmkey
**	Syntax   	: int Removeshm(key_t shmkey);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**	Return Value: On success , positive value 
**				  Otherwise,   error -1
**  See also 	: Getshmid Openshm Makeshm Attachshm Detachshm Removeshm 
**			      		
**#$ 
*/

int Removeshm(key_t shmkey)
{
	int shmid;
	int rtn;
	if((shmid=Getshmid(shmkey))<0) return -1;
	if(shmctl(shmid,IPC_RMID,0)<0) return -1;
	return 1;
} 
