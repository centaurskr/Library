////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 14:26:33 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

/*#@
**	Function 	: Make shared memory area  
**	Syntax   	: int Makeshm(key_t shmkey,long nbyte);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**	Return Value: On success , positive value int type shmid 
**				  Otherwise,   error -1
**  See also 	: Getshmid Openshm Makeshm Attachshm Detachshm Removeshm 
**			      		
**#$ 
*/

extern int Getshmid();
extern int Removeshm();

int Makeshm(key_t shmkey,long nbyte)
{
	int shmid;

	if(shmkey == IPC_PRIVATE) 			return -1;
	if((shmid = Getshmid(shmkey))>0){
		if(Removeshm(shmkey)<0) 		return -2;
	}
	if((shmid = shmget(shmkey,nbyte,0666|IPC_CREAT))<0) return -3;
	return shmid;
}
