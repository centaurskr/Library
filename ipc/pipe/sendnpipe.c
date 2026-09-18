////////////////////////////////////////////////////////////////////////////////
// File Name : 
// Date      : 2011. 07. 11. (월) 13:52:06 KST
// By        : YjYoon
////////////////////////////////////////////////////////////////////////////////

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

/*#@
**	Function 	: Send Data to named pipe
**	Syntax   	: int Senddata2npipe(int fd, uchar *data, int size);
**	Remark		:
**				  Writes a 4-byte (sizeof(int)) length header followed
**				  by `size` bytes of data, matching what
**				  Readdatafromnpipe expects to read back.
**				  CAUTION: header+data is assembled in a fixed 4096-byte
**				  stack buffer, so size must be <= 4096 - sizeof(int)
**				  (4092 bytes); larger sizes overflow that buffer.
**	Return Value: On success, number of data bytes written (size)
**				  Otherwise, error: return value of the failed write()
**  See also 	: Makenamedpipe Opennamedpipe,Closenamedpipe,Deletenamedpipe
**				  Serverpipe,Clientpipe,Readdatafromnpipe
**
**#$
*/

int Senddata2npipe(int fd,unsigned char *data,int size)
{
unsigned char buff[4096];
int  rtn;
	memset(buff, 0x00, 4096);
	memcpy(buff, (unsigned char *)&size, sizeof(int));
	memcpy(buff + sizeof(int), data, size);
	size += sizeof(int);
	rtn = write(fd, buff, size);
	if(rtn > 0)
		return rtn - sizeof(int);
	else return rtn;
}
