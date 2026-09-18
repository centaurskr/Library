////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 14:25:42 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

/*#@
**	Function 	: Detach region area from processor region 
**	Syntax   	: int Detachshm(char *vaddr);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**	Return Value: On success , positive value 
**				  Otherwise,   error -1
**  See also 	: Getshmid Openshm Makeshm Attachshm Detachshm Removeshm 
**			      		
**#$ 
*/

int Detachshm(char *vaddr)
{
	int rtn;
	if((rtn = shmdt(vaddr))<0) return -1;
	return 1;
}
