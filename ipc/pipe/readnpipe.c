////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:51:54 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Read Data from named pipe
**	Syntax   	: int Readdatafromnpipe(int fd, uchar *buff);
**	Remark		:
**				  Wire format written by Senddata2npipe: a leading
**				  4-byte (sizeof(int)) length header, then that many
**				  data bytes. This reads the header first, then reads
**				  exactly that many bytes into buff - buff must be at
**				  least that large or the read overflows it.
**	Return Value: On success, number of data bytes read (>= 0)
**				  Otherwise, error: return value of the failed read()
**				  (typically -1, errno set)
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe,Senddata2npipe
**
**#$
*/

int Readdatafromnpipe(int fd,unsigned char *buff)
{
int size, rtn;
	rtn = read(fd, &size, sizeof(int));
	if(rtn < 0) return rtn;
	return read(fd, buff, size);
}
