////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:51:40 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Open named pipe 
**	Syntax   	: int Opennamedpipe(uchar *path ,int mode);
**  Prototype in: 
**  Libary    in:
**	Remark		:
**				  path -> pipe name path
**				  Does not create the fifo or set permissions (see
**				  Makenamedpipe) - just open()s an existing node.
**				  mode -> O_RDONLY O_WRONLY O_RDWR O_NDELAY ...
**	Return Value: On success , positive value int type file descriptor
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**			      		
**#$ 
*/

int Opennamedpipe(unsigned char *path,int mode)
{
	int pipe;

	pipe=open((char *)path,mode);
	if(pipe<0)	return -1;
	return pipe;
}
