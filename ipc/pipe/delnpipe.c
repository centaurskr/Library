#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Delete name pipe 
**	Syntax   	: int Deletenamedpipe(int pipe,uchar *path); 
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**				  path -> pipe name path	
**				  system call unlink
**	Return Value: On success , positive value 
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**			      		
**#$ 
*/

int Deletenamedpipe(int pipe,char *pname)
{
	close(pipe);
	if(unlink(pname)<0) return -1;
	return 1;
}
