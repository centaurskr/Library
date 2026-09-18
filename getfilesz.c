//
// Description : File 길이를 얻는다
// File Name   : getfilesz.c
// Date        : 2021. 12. 03. (금) 11:29:47 KST
// By          : Cento
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>


////////////////////////////////////////////////////////////////////////////////
// file size를 얻는다
// Prototype : GetFileSize(char *fname)
// Arguments : char *fname : 파일이름
// Return    : size_t : 성공, -1:파일없음
////////////////////////////////////////////////////////////////////////////////
int GetFileSize(char *fname)
{
struct stat  st;
int          rtn;
	rtn =  stat(fname, &st);
	if(rtn) return -1;
	return (int)st.st_size;
}
