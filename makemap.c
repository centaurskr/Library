//
// Description : Create Memory maped file
// File Name   : makemap.c
// Date        : 2012. 02. 02. (목) 15:23:36 KST
// By          : Yjyoon
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <errno.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Create memory maped file. If the file is exist, erase first!
// Prototype   : char *MakeMapFile(char *fname, size_t size)
// Arguments   : char *fname : Map filename
//               off_t size : map file size
// Return      : Maped pointer
////////////////////////////////////////////////////////////////////////////////
char *MakeMapFile(char *fname, off_t size)
{
struct stat mst;
char        *pmap;
int          fd, rtn;
	if(!stat(fname, &mst)) unlink(fname);

	fd = open(fname, O_RDWR|O_CREAT, 0666);
	if(fd < 0) return NULL;
	lseek(fd, size - 1, SEEK_SET);
	rtn = write(fd, " ", 1);
	close(fd);
	
	fd = open(fname, O_RDWR);
	pmap = (char *)mmap(NULL, size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);
	if(pmap == MAP_FAILED)return NULL;
	close(fd);
	return pmap;

}
////////////////////////////////////////////////////////////////////////////////
// Description : 이미 존재하는 파일을 memory mapped file로 attach한다
//               (MakeMapFile과 달리 파일을 새로 만들거나 지우지 않는다)
// Prototype   : void *AttachMapFile(char *filename, off_t size)
// Arguments   : char *filename : attach할 파일 이름
//               off_t size     : mmap()에 넘길 매핑 크기(바이트)
// Return      : 성공 시 매핑된 포인터, 실패(open 실패 또는 mmap 실패) 시 NULL
////////////////////////////////////////////////////////////////////////////////
void *AttachMapFile(char *filename, off_t size)
{
void *pmap;
int   fd;
	fd = open(filename, O_RDWR);
	if(fd < 0){
		 /*printf("FD [%d] errno[%d]. %s\n", fd, errno, strerror(errno));*/
		return NULL;
	}
	pmap = mmap(NULL, size, PROT_WRITE|PROT_READ, MAP_SHARED, fd, 0);	
	close(fd);
	if(pmap == MAP_FAILED ){
		/*printf("FD---- [%d] errno[%d]. %s\n", fd, errno, strerror(errno));*/
		return NULL;
	}

	return pmap;
}
