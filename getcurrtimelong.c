//
// Description : Get Current time with long
// File Name   : getcurrtimelong.c
// Date        : 2017. 08. 04. (금) 09:52:27 KST
// By          : centaurskr@gmail.com
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// Description : 현재 시각을 초 단위 unix timestamp(long)로 얻는다
// Prototype   : long GetCurrTimeLong()
// Arguments   : 없음
// Return      : unix timestamp(초 단위)
////////////////////////////////////////////////////////////////////////////////
long GetCurrTimeLong()
{
struct timespec tm;
	clock_gettime(CLOCK_REALTIME, &tm);
	return (long)tm.tv_sec;
}
