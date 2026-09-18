/***************************************************/
/* Name    : npipecil.c                            */
/* Create  : 1995.05.19                            */
/* By      : Daewon System Co., Ltd.(Sagittarrius) */
/* Remarks : Test named pipe library(Client domain)*/
/*           $(HOME)/lib/libnpipe.a                */
/***************************************************/
#include <stdio.h>
#include <stdlib.h>

#define  TEST_PIPE_NAME "./TEST.PIPE"
char *pname;
main(argc, argv)
int    argc;
char **argv;
{
int   id;
int   rtn, i, times;
char  buff[255];
	pname = argv[0];
	if(argc < 3){
		printf("Usage :  %s \"Message\" [times]\n", pname);
		exit(1);
	}
	times = atoi(argv[2]);
	if((id = Clientpipe(TEST_PIPE_NAME)) < 0){
		printf("\n%s --- Can't open a named pipe for client;[%s]\n\n", 
				pname, TEST_PIPE_NAME);
		exit(1);
	}
	for (i = 0 ; i < times; i++){
		sprintf(buff, "-%4d-[%s]", i, argv[1]);
		rtn = Senddata2npipe(id, buff, strlen(buff));
	}
	printf("Send Message size index[%d] size[%d]\n", i, rtn);
	Closenamedpipe(id);
}
