//
// Description : Unmap for memory maped file.
// File Name   : unmmap.c
// Date        : 2012. 02. 02. (목) 18:52:27 KST
// By          :
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Unmap for memory maped file
// Prototype   : void UnmapMapFile(void *point, size_t size)
// Arguments   : void *point : Maped point
//               size_t size : Maped size
// Return      : void
////////////////////////////////////////////////////////////////////////////////
void UnmapMapFile(void *point, size_t size)
{
	munmap(point, size);
}
