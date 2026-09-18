////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:52:18 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

extern int Makenamedpipe();
extern int Opennamedpipe();

/*#@
**	Function 	: Server processor make name pipe 
**	Syntax   	: int Serverpipe(uchar *path,int *wpipe, delay);
**  Prototype in: 
**  Libary    in:
**	Remark		:  
**				  path -> pipe name path	
**				  Ineternal Call Makenamedpipe
**				  wpipe -> Not use file descriptor (open WRONLY )
**                delay -> 1 : Default
**                         0 : O_NDELAY mode.
**	Return Value: On success , positive value int type File descriptor 
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
*			      		
* 
*/
#define PERM 0666

int Serverpipe(char *pname,int *wpipe,int delay)
{
int pipe, st = 0;
	unlink(pname);	
	if(mknod(pname,S_IFIFO | PERM,0)<0) return -1;	
	chmod(pname, PERM);
	pipe = Opennamedpipe(pname, O_RDWR | O_NDELAY);
	if(pipe<0)	return -1;
	if(delay){
		st = fcntl(pipe, F_GETFL);
		st &= ~O_NDELAY;
		fcntl(pipe, F_SETFL, st);
	}
	*wpipe = Opennamedpipe(pname,O_WRONLY);
	return (pipe);
}
/*#@
**	Function 	: Server processor make name pipe (Makenamedpipe variant)
**	Syntax   	: int Serverpipe_t(uchar *pname, int *wpipe, int delay);
**  Prototype in:
**  Libary    in:
**	Remark		:
**				  Same purpose as Serverpipe, but creates the read side
**				  via Makenamedpipe(pname, mode) instead of mknod+chmod+
**				  Opennamedpipe, and has no fcntl() blocking-mode toggle.
**				  path -> pipe name path (existing node is unlink()'d first)
**				  delay -> non-zero : open read side O_RDONLY (blocking)
**				            0        : open read side O_RDONLY|O_NDELAY
**				  wpipe -> out param: write-side fd, opened O_WRONLY
**	Return Value: On success, positive value int type File descriptor
**				  Otherwise,   error -1
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe
**#$
*/
int Serverpipe_t(pname,wpipe, delay)
char *pname;
int  *wpipe;
int   delay;
{
int pipe, st = 0;

	unlink(pname);	
	if(delay)
		pipe = Makenamedpipe(pname, O_RDONLY);
	else 
		pipe = Makenamedpipe(pname, O_RDONLY|O_NDELAY);
	*wpipe = Opennamedpipe(pname,O_WRONLY);
	return (pipe);
}
