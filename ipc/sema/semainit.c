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
// Create (or attach to) a single-member System V semaphore identified by key,
// and initialize its value to 1 if this call created it.
// Prototype : int InitSemaphore(key_t key)
// Arguments : key_t key : System V IPC key identifying the semaphore set
// Return    : On success, the semaphore set id (>= 0, usable with
//             SemaphoreOperation()). On failure, the negated errno value
//             (e.g. semget()/semctl() failure other than "already exists").
////////////////////////////////////////////////////////////////////////////////
int InitSemaphore(key)
key_t key;
{
int id, rtn =1;
	id = semget(key, 1, 0666 | IPC_CREAT|IPC_EXCL);
	if(id < 0){
		if(errno == EEXIST) id = semget(key, 1, 0);
		else return (errno * -1);
	}
	rtn = semctl(id, 0, SETVAL, &rtn);
	if(rtn < 0) return (errno * -1);
	return id;
}
