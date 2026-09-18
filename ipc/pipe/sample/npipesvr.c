/***************************************************/
/* Name    : npipesvr.c                            */
/* Create  : 1995.05.19                            */
/* By      : Daewon System Co., Ltd.(Sagittarrius) */
/* Remarks : Test named pipe library(Srever domain)*/
/*           $(HOME)/lib/libnpipe.a                */
/***************************************************/
#include <stdio.h>
#include <stdlib.h>
#define  TEST_PIPE_NAME "./TEST.PIPE"
char *pname;

void MsgRead(id)
int  id;
{
char    buff[255];
int     size  = 0;
int     len, rtn;
	while(1){
		memset(buff, 0x00, 255);
		Readdatafromnpipe(id, buff);
		printf("BUFF[%s]\n", buff);
	}
}
void MsgWrite(pipeid, size, times)
int pipeid;
int size;
int times;
{
int   i, rtn;
char  buff[1024];
	memset(buff, 'a', size);
	for(i = 0; i < times; i++){
		rtn = write(pipeid, size, buff);
		/*
		if(rtn != size)
		*/
			printf("INDEX[%d] size[%d] RTN[%d]\n", i, size, rtn);
	}
}
main(argc, argv)
int    argc;
char **argv;
{
int   mainid, dumyid;
char buff[1024];
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
	MsgRead(mainid);
	Deletenamedpipe(mainid, TEST_PIPE_NAME);
	printf("(%s)----Terminated named pipe server !\n", pname);
}
