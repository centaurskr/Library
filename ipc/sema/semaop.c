////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:59:35 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// Lock (P) or unlock (V) semaphore #0 of the given set, with SEM_UNDO so the
// kernel automatically reverts the operation if this process exits/crashes
// while still holding the lock.
// Prototype : int SemaphoreOperation(int id, int op)
// Arguments : int id : semaphore set id, as returned by InitSemaphore()
//             int op : 1 to lock (decrement, blocks while count is 0),
//                       any other value to unlock (increment)
// Return    : 1 on success. On failure, the negated errno value from semop().
////////////////////////////////////////////////////////////////////////////////
int SemaphoreOperation(id, op)
int id, op;
{
struct sembuf pv;
	pv.sem_num = 0;
	if(op == 1) pv.sem_op  = -1; /* semaphore on  */
	else pv.sem_op  = 1;         /* semaphore off */
	pv.sem_flg = SEM_UNDO;

	if(semop(id, &pv, 1) == -1)return (errno * -1);
	else return 1;
}
