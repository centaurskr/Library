////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:49:04 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

extern int Opennamedpipe();

/*#@
**	Function 	: Client processor open name pipe 
**	Syntax   	: int Clientpipe(uchar *path );
**  Prototype in: 
**  Libary    in:
**	Remark		:
**				  Opens with O_WRONLY|O_NDELAY (write-only, non-blocking)
**				  path -> pipe name path
**				  Ineternal Call Opennamedpipe
**	Return Value: On success , positive value int type File descriptor 
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**			      		
**#$ 
*/

int Clientpipe(char *pname)
{
	int pipe;

	pipe= Opennamedpipe(pname,O_WRONLY|O_NDELAY);
	if(pipe<0) return -1;
	return (pipe);
}
