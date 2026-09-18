/***************************************************/
/* Name    : npipesvr.c                            */
/* Create  : 1995.05.19                            */
/* By      : Daewon System Co., Ltd.(Sagittarrius) */
/* Remarks : Test named pipe library(Srever domain)*/
/*           $(HOME)/lib/libnpipe.a                */
/***************************************************/
#include <stdio.h>
#include <stdlib.h>
#include "event.h"

#define  TEST_PIPE_NAME "./TEST.PIPE"
char *pname;

int EventHandler(event, fd, type)
EVENT *event;
int  fd, type;
{
char    buff[255];
int     size  = 0;
int     len, rtn;
	memset(buff, 0x00, 255);
	rtn = Readdatafromnpipe(fd, buff);
	if(rtn <= 0) return 1;
	printf("BUFF[%s]\n", buff);
	return 1;
}
main(argc, argv)
int    argc;
char **argv;
{
int   mainid, dumyid;
char buff[1024];
EVENT *event;
	pname = argv[0];
	printf("\nStarted! Named pipe Server domain!\n");
	printf("Pipe Path is : %s\n", TEST_PIPE_NAME);
	printf("Wait message from some client........\n\n");
	if((mainid = Serverpipe(TEST_PIPE_NAME, &dumyid, 1)) < 0){
		printf("\n%s --- Can't create a named pipe;[%s]\n\n", 
				pname, TEST_PIPE_NAME);
		exit(1);
	}
	printf("OK Message --------------- INIT[%d]\n", mainid);
	event = AppEventInit();
	AppAddEventAutoId(event, mainid, 30000, 0, EventHandler);
	AppEventLoop(event);
	Deletenamedpipe(mainid, TEST_PIPE_NAME);
	printf("(%s)----Terminated named pipe server !\n", pname);
}
