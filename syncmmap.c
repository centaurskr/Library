//
// Description :
// File Name   : syncmmap.c
// Date        : 2023. 01. 18. (수) 14:41:07 KST
// By  : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Flush a memory-mapped region to its backing file asynchronously
//               via msync(..., MS_ASYNC). Does NOT unmap - see unmmap.c for that.
// Prototype   : int SyncMapFile(void *point, size_t size)
// Arguments   : point : mapped address (as returned by mmap())
//               size  : length of the mapped region, in bytes
// Return      : declared int but has no return statement (msync()'s result is
//               discarded); treat the return value as undefined
////////////////////////////////////////////////////////////////////////////////
int SyncMapFile(void *point, size_t size)
{
	msync(point, size, MS_ASYNC);
}
