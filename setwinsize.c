//
// Description :
// File Name   : setwinsize.c
// Date        : 2017. 07. 19. (수) 16:38:52 KST
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
// Description : Set the terminal window size (columns/rows) of a tty fd via TIOCSWINSZ
// Prototype   : int SetWindowSize(int fd, int x, int y)
// Arguments   : fd : file descriptor, must refer to a terminal (checked with isatty())
//               x  : number of columns to set
//               y  : number of rows to set
// Return      : 1 on success, -1 if fd is not a tty
////////////////////////////////////////////////////////////////////////////////
int SetWindowSize(fd, x, y)
int fd, x, y;
{
struct winsize wsz;
	if(!isatty(fd)) return -1;
	ioctl(fd, TIOCGWINSZ, &wsz);
	wsz.ws_col = x;
	wsz.ws_row = y;
	ioctl(fd, TIOCSWINSZ, &wsz);
	return 1;
}

