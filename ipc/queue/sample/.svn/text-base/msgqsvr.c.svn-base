/*
 * Library test for message queue. (server)
 * NAME    : msgqsvr.c
 * DATE    : 1996/04/15-11:14:31
 * BY      : Sagittarius(Young_Jin Yoon).
 * REMARKS :
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char *pname;
int   DONE = 1;

#define QKEY    0x0ff00000
#define MAXLEN  3072

main(argc, argv)
int    argc;
char **argv;
{
int qid, rtn, rtype, mytype;
char buff[3080];
	pname = argv[0];
	if(argc < 2){
		printf("%s <Received type>\n", pname);
		exit(0);
	}
	mytype = atoi(argv[1]);
	qid = MakeMsgqueue(QKEY,  MAXLEN);
	if(qid < 0){
		fprintf(stderr, "[%s] Error MakeMsgqueue() KEY[0x%08x]\n", QKEY);
		exit(0);
	}
	memset(buff, 0x00, 3080);
	while(DONE){
		rtn = ReadMsgqueue(qid, mytype, &rtype, 1, buff);
		printf("READ[%d][%d][%s]\n", rtn, rtype, buff);
		if(!strncmp(buff, "END", 3))break;
	}
	CloseMsgqueue(qid);
}
