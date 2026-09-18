////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:49:29 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Close name pipe 
**	Syntax   	: int Closenamedpipe(int pipe);
**  Prototype in: 
**  Libary    in: libcil.a
**	Remark		:  
**	Return Value: On success , positive value 
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**			      		
**#$ 
*/

int Closenamedpipe(int pipe)
{
	if(close(pipe)<0) return -1;
	return 1;
}
