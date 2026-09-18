/*
 * Library test for message queue. (client)
 * NAME    : msgqcli.c
 * DATE    : 1996/04/15-11:29:32
 * BY      : Sagittarius(Young_Jin Yoon).
 * REMARKS :
 */
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

char *pname;
#define QKEY    0x0ff00000

main(argc, argv)
int    argc;
char **argv;
{
int qid, i, rtn, type, k;
	pname = argv[0];
	if(argc < 3){
		fprintf(stderr, "[%s] <Messaage type> <Message for sending>\n", pname);
		exit(0);
	}
	type = atoi(argv[1]);
	k = 10;
	if(argc > 3) k = atoi(argv[3]);

	qid = OpenMsgqueue(QKEY);
	if(qid < 0){
		fprintf(stderr, "[%s] - OpenMsgqueue() KEY[0x%08x] errorno[%d]\n",
			pname, QKEY, errno);
		exit(0);
	}
	for(i = 0; i < k; i++){
		rtn = WriteMsgqueue(qid, type, strlen(argv[2]), 1, argv[2]);
		printf("Write - RTN[%d]\n",  rtn);
	}
	rtn = WriteMsgqueue(qid, type, 3, 1, "END");
	printf("Write - RTN[%d]\n",  rtn);
	exit(0);
}
