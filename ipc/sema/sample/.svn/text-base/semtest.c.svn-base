/*
 * Header를 읽어 Check print형태로 변환한다.
 * NAME : heaconv.c
 * DATE : 1999 년 04월 08일 목 12:45:59 오후
 * BY   : KSY(Pentasoft)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "nhrdm.h"
#include "nhnews.h"

#include "koscom.h"
#include "nhcoproto.h"


#define TEST_SEM 0x85000000


int main(argc,argv)
int argc;
char **argv;
{
	int  rtn,ii,sid;

	sid = InitSemaphore(TEST_SEM);
	if(sid < 0) {
		printf("Semaphore init error ... [%d]\n",rtn);
		exit(1);
	}

	for(ii=0;ii<10;ii++) {
		rtn = SemaphoreOperation(sid,1);
		printf("[%6d] [%4d] lock....\n",getpid(),ii);
		sleep(1);
		rtn = SemaphoreOperation(sid,0);
	}

}




