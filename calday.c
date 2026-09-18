//
// Description : 일계산 Function
// File Name   : calday.c
// Date        : 2022. 02. 03. (목) 13:33:32 KST
// By  : Cento
//
#define _XOPEN_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#define  SEC_ONEDAY  86400 // 24 X 60 X 60

////////////////////////////////////////////////////////////////////////////////
/// @brief  기준 시각으로 부터 incdec 만큼 분단위 증감
/// @fn      unsigned long IncDecNumMin(unsigned long base, int incdec)
/// @param   base 기준시각(yyyymmddhhmm)
/// @param   incdec 증감 분
/// @return  다음 시각 (yyyymmddhhmm)
////////////////////////////////////////////////////////////////////////////////
unsigned long IncDecNumMin(unsigned long base, int incdec)
{
char      instr[20];
time_t    nt;
struct tm tm = {0};
const char *format = "%Y%m%d%H%M";

	memset(instr, 0x00, 20);
	sprintf(instr, "%ld", base);
	strptime(instr, format, &tm);

	nt = mktime(&tm) + incdec * 60L;
	localtime_r(&nt, &tm);

	// 2023 12 01 12 23
	return (unsigned long)(
		 (tm.tm_year + 1900) * 100000000L
		+(tm.tm_mon  + 1   ) * 1000000L
		+(tm.tm_mday       ) * 10000L
		+(tm.tm_hour       ) * 100L
		+tm.tm_min);
}
////////////////////////////////////////////////////////////////////////////////
/// @brief  일자계산 (기준일로 부터 incdec 만큼 증감)
/// @fun      int IncDecNumyyyymmdd(int base, int incdec)
/// @param   base    yyyymmdd(기준일)
/// @param   incdec  증감일 (기준일 +- incdec)
/// @return   int        : 증감일 (yyyymmdd)
////////////////////////////////////////////////////////////////////////////////
int IncDecNumYyyymmdd(int base, int incdec)
{
time_t nt;
struct tm tm;
	memset((void *)&tm, 0x00, sizeof(tm));
	tm.tm_year = (int)(base  / 10000) - 1900;
	tm.tm_mon  = (int)((base % 10000)/100) - 1;
	tm.tm_mday = base % 100;
	nt = mktime(&tm);
	nt = (time_t)((unsigned long)nt + SEC_ONEDAY * incdec);
	localtime_r(&nt, &tm);
	return (int)((tm.tm_year + 1900) * 10000 + (tm.tm_mon+1) * 100 
		+ tm.tm_mday);
}
////////////////////////////////////////////////////////////////////////////////
//  기준일로 부터 다음 주일
// Prototype : int NextSundayNumYyyymmdd(int base)
// Arguments : int base : yyyymmdd 기준일
// Return    : int : 다음 주일 (yyyymmdd)
////////////////////////////////////////////////////////////////////////////////
int NextSundayNumYyyymmdd(int base)
{
time_t    nt;
struct tm tm;
int       df;
	memset((void *)&tm, 0x00, sizeof(tm));
	tm.tm_year = (int)(base  / 10000) - 1900;
	tm.tm_mon  = (int)((base % 10000)/100) - 1;
	tm.tm_mday = base % 100;
	nt = mktime(&tm);
	localtime_r(&nt, &tm);

	df = 6 - tm.tm_wday + 1;

	nt = (time_t)((unsigned long)nt + SEC_ONEDAY * df);
	localtime_r(&nt, &tm);
	return (int)((tm.tm_year + 1900) * 10000 + (tm.tm_mon+1) * 100 
		+ tm.tm_mday);
}
////////////////////////////////////////////////////////////////////////////////
// 기준일의 주일 구하기
// Prototype :  int ThisSundayNumYyyymmdd(int base)
// Arguments : int base : yyyymmdd 기준일
// Return    : int : 다음 주일 (yyyymmdd)
////////////////////////////////////////////////////////////////////////////////
int ThisSundayNumYyyymmdd(int base)
{
time_t    nt;
struct tm tm;
int       df;
	memset((void *)&tm, 0x00, sizeof(tm));
	tm.tm_year = (int)(base  / 10000) - 1900;
	tm.tm_mon  = (int)((base % 10000)/100) - 1;
	tm.tm_mday = base % 100;
	nt = mktime(&tm);
	localtime_r(&nt, &tm);

	df = tm.tm_wday * -1;

	nt = (time_t)((unsigned long)nt + SEC_ONEDAY * df);
	localtime_r(&nt, &tm);
	return (int)((tm.tm_year + 1900) * 10000 + (tm.tm_mon+1) * 100 
		+ tm.tm_mday);

}
////////////////////////////////////////////////////////////////////////////////
//  기준일로 부터 다음 주 주초일(월요일기준)
// Prototype : int NextMondayNumYyyymmdd(int base)
// Arguments : int base : yyyymmdd 기준일
// Return    : int : 다음 주일 (yyyymmdd)
////////////////////////////////////////////////////////////////////////////////
int NextMondayNumYyyymmdd(int base)
{
time_t    nt;
struct tm tm;
int       df;
    memset((void *)&tm, 0x00, sizeof(tm));
    tm.tm_year = (int)(base  / 10000) - 1900;
    tm.tm_mon  = (int)((base % 10000)/100) - 1;
    tm.tm_mday = base % 100;
    nt = mktime(&tm);
    localtime_r(&nt, &tm);

    df =(tm.tm_wday - 1 + 7) % 7;

    nt = (time_t)((unsigned long)nt - SEC_ONEDAY*df + SEC_ONEDAY*7);
    localtime_r(&nt, &tm);
    return (int)((tm.tm_year + 1900) * 10000 + (tm.tm_mon+1) * 100
        + tm.tm_mday);
}
////////////////////////////////////////////////////////////////////////////////
// 주초일 구하기 (월요일기준)
// Prototype :  int ThisMondayNumYyyymmdd(int base)
// Arguments : int base : yyyymmdd 기준일
// Return    : int : 다음 주일 (yyyymmdd)
////////////////////////////////////////////////////////////////////////////////
int ThisMondayNumYyyymmdd(int base)
{
time_t    nt;
struct tm tm;
int       df;
    // base 를 tm으로 변경
    memset((void *)&tm, 0x00, sizeof(tm));
    tm.tm_year = (int)(base  / 10000) - 1900;
    tm.tm_mon  = (int)((base % 10000)/100) - 1;
    tm.tm_mday = base % 100;
    nt = mktime(&tm);
    localtime_r(&nt, &tm);

    df = (tm.tm_wday  -1 + 7) % 7; // 주중 몇일 째?(0:SUN, 1:MON....)

    nt = (time_t)( (unsigned long)nt - SEC_ONEDAY * df );
    localtime_r(&nt, &tm);
    return (int)((tm.tm_year + 1900) * 10000 + (tm.tm_mon+1) * 100
        + tm.tm_mday);

}

