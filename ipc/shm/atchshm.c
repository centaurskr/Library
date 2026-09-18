////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 14:25:15 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*#@
**	Function 	: Attach region area to processor region 
**	Syntax   	: int Attachshm(int shmid,char *vaddr,int flag)
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**				  System Call shmat
**	Return Value: On success , reginon aread vertural address 
**				  Otherwise,   error NULL
**  See also 	: Getshmid Openshm Makeshm Attachshm Detachshm Removeshm 
**			      		
**#$ 
*/

char *Attachshm(int shmid,char *shmaddr,int flag)
{
	return shmat(shmid,shmaddr,flag);
}
