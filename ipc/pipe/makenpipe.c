#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Make named pipe 
**	Syntax   	: int Makenamedpipe(uchar *path ,int mode);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**			      permission default 0666 
**				  path -> pipe name path	
**				  mode -> O_RDONLY O_WRONLY O_RDWR O_NDELAY ...	
**	Return Value: On success , positive value int type file descriptor
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**			      		
**#$ 
*/
#define PERM 0666
int Makenamedpipe(char *path,int mode) 
{
int pipe;

	if(mknod((char *)path,S_IFIFO,0)<0) return -1;	
	chmod((char *)path, PERM);
	pipe=open((char *)path,mode);
	if(pipe<0)	return -1;
	return pipe;
}
