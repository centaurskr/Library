//
// Description : 
// File Name   : getwinsize.c
// Date        : 2017. 07. 19. (수) 16:28:19 KST
// By          : centaurskr@gmail.com
//

#include <stdio.h>
#include <stdlib.h>
#ifndef _MAC_
#include <termio.h>
#endif
#include <sys/ioctl.h>
#include <unistd.h>

////////////////////////////////////////////////////////////////////////////////
// Description : Get the terminal window size (columns/rows) of a tty fd via TIOCGWINSZ
// Prototype   : int GetWindowSize(int fd, int *x, int *y)
// Arguments   : fd : file descriptor, must refer to a terminal (checked with isatty())
//               x  : out - number of columns
//               y  : out - number of rows
// Return      : 1 on success, -1 if fd is not a tty
////////////////////////////////////////////////////////////////////////////////
int GetWindowSize(int fd, int *x, int *y)
{
struct winsize wsz;
	if(!isatty(fd)) return -1;
	ioctl(fd, TIOCGWINSZ, &wsz);
	*x = wsz.ws_col;
	*y = wsz.ws_row;
	return 1;
}
