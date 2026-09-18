//
// Description : 일자 함수
// File Name   : getYmd.c
// Date        : 2021. 10. 19. (화) 15:23:49 KST
// By  : Cento
//
#define _XOPEN_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
////////////////////////////////////////////////////////////////////////////////
// yyyymmddhhmmss 형식의 숫자를 unix timestamp(초)로 변환
// Prototype : unsigned long ConvertTimeStemp(unsigned long ymd)
// Arguments : unsigned long ymd : yyyymmddhhmmss (local time)
// Return    : unix timestamp(초 단위)
////////////////////////////////////////////////////////////////////////////////
unsigned long ConvertTimeStemp(unsigned long  ymd)
{
struct tm tm;
char temp[32];
	memset((void *)&tm, 0x00, sizeof(tm));
	sprintf(temp, "%ld", ymd);
	strptime(temp, "%Y%m%d%H%M%S", &tm);
	return (unsigned long)mktime(&tm);
}
////////////////////////////////////////////////////////////////////////////////
// Description : 현재 시각을 yyyymmdd(8자리) 문자열로 얻는다
// Prototype   : void GetStrYyyymmdd(char *buff)
// Arguments   : char *buff : [OUT] 최소 9바이트(8자리 + NUL은 미보장, 9바이트 권장)
// Return      : 없음 (buff에 8자리 채움)
////////////////////////////////////////////////////////////////////////////////
void GetStrYyyymmdd(char *buff)
{
char tmp[10];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 10, "%Y%m%d", &newtime);
    strncpy(buff, tmp, 8);
    return ;
}
////////////////////////////////////////////////////////////////////////////////
/// 문자열 YYMMDD를 구한다
/// @fn  void GetStrYymmdd(char *ymd)
/// @param [OUT]  ymd 문자열 buffer (최소 6바이트, NUL 미보장)
/// Return      : 없음 (ymd에 6자리 채움)
////////////////////////////////////////////////////////////////////////////////
void GetStrYymmdd(char *ymd)
{
char tmp[10];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 10, "%y%m%d", &newtime);
	memcpy(ymd, tmp, 6);
}

////////////////////////////////////////////////////////////////////////////////
// Description : 현재 시각의 년월일을 yymmdd(6자리) 숫자로 얻는다
// Prototype   : int GetNumYymmdd()
// Arguments   : 없음
// Return      : yymmdd 숫자 (예: 240102)
////////////////////////////////////////////////////////////////////////////////
int GetNumYymmdd()
{
char tmp[10];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 10, "%y%m%d", &newtime);
    return atoi(tmp);
}
////////////////////////////////////////////////////////////////////////////////
// Description : 현재 시각을 yymmddhhmm(10자리) 숫자로 얻는다
// Prototype   : unsigned long GetNumYymmddhhmm()
// Arguments   : 없음
// Return      : yymmddhhmm 숫자
////////////////////////////////////////////////////////////////////////////////
unsigned long GetNumYymmddhhmm()
{
char tmp[20];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 20, "%y%m%d%H%M", &newtime);
    return atol(tmp);
}

////////////////////////////////////////////////////////////////////////////////
// Description : 현재 시각의 년월일을 yyyymmdd(8자리) 숫자로 얻는다
// Prototype   : int GetNumYyyymmdd()
// Arguments   : 없음
// Return      : yyyymmdd 숫자 (예: 20240102)
////////////////////////////////////////////////////////////////////////////////
int GetNumYyyymmdd()
{
char tmp[10];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 10, "%Y%m%d", &newtime);
    return atoi(tmp);
}

////////////////////////////////////////////////////////////////////////////////
// 초 단위 timestamp 값구하기
// Prototype : unsigned long GetTimestamp();
// Arguments : 없음
// Return    : unix timestamp(초 단위)
////////////////////////////////////////////////////////////////////////////////
unsigned long GetTimestamp()
{
struct timespec tm;
    clock_gettime(CLOCK_REALTIME, &tm);
    return (unsigned long)tm.tv_sec;
}

////////////////////////////////////////////////////////////////////////////////
// Millisecond(1/1000초) timestamp 값구하기
// Prototype : unsigned long GetTimestampMs()
// Arguments :
// Return    : 1/1000초 time stamp
////////////////////////////////////////////////////////////////////////////////
unsigned long GetTimestampMs()
{
unsigned long tmstamp;
struct timespec tms;
	clock_gettime(CLOCK_REALTIME, &tms);
	tmstamp  = tms.tv_sec *  1000;	
	tmstamp += tms.tv_nsec / 1000000; // 10억 / 천만 --> millisecond(ms)
	return tmstamp;
}
////////////////////////////////////////////////////////////////////////////////
// 현재 시각을 yymmddhhmmsscc(년월일시분초 + 1/100초 2자리, 14자리) 문자열로 얻는다
// Prototype : void GetStrYymmddhhmmsscc(char *buff)
// Arguments : char *buff : [OUT] 최소 15바이트 (sprintf로 NUL 종료됨)
// Return    : 없음 (buff에 14자리 문자열을 채움)
////////////////////////////////////////////////////////////////////////////////
void GetStrYymmddhhmmsscc(char *buff)
{
char tmp[20];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 20, "%y%m%d%H%M%S", &newtime);
    sprintf(buff, "%.12s%02d", tmp, (int)(tm.tv_nsec / 10000000));
    return ;
}
////////////////////////////////////////////////////////////////////////////////
// 현재 시각을 yyyymmddhhmmsscc(년월일시분초 + 1/100초 2자리) 숫자로 얻는다
// Prototype : unsigned long GetNumYyyymmddhhmmsscc()
// Arguments : 없음
// Return    : yyyymmddhhmmsscc 숫자 (16자리)
////////////////////////////////////////////////////////////////////////////////
unsigned long GetNumYyyymmddhhmmsscc()
{
char tmp[20], buff[20];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 20, "%Y%m%d%H%M%S", &newtime);
    sprintf(buff, "%.14s%02d", tmp, (int)(tm.tv_nsec / 10000000));
    return (unsigned long)atol(buff);
}
////////////////////////////////////////////////////////////////////////////////
// 현재 시각을 yyyymmddhhmmssmmm(년월일시분초 + 밀리초 3자리) 숫자로 얻는다
// Prototype : unsigned long GetNumYyyymmddhhmmssmmm()
// Arguments : 없음
// Return    : yyyymmddhhmmssmmm 숫자 (17자리)
////////////////////////////////////////////////////////////////////////////////
unsigned long GetNumYyyymmddhhmmssmmm()
{
char tmp[20], buff[20];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 20, "%Y%m%d%H%M%S", &newtime);
    sprintf(buff, "%.14s%03d", tmp, (int)(tm.tv_nsec / 1000000));
    return (unsigned long)atol(buff);
}
////////////////////////////////////////////////////////////////////////////////
// 현재 시각을 yyyymmddhhmm(년월일시분, 12자리) 숫자로 얻는다
// Prototype : unsigned long GetNumYyyymmddhhmm()
// Arguments : 없음
// Return    : yyyymmddhhmm 숫자
////////////////////////////////////////////////////////////////////////////////
unsigned long GetNumYyyymmddhhmm()
{
char tmp[20], buff[20];
struct timespec tm;
struct tm newtime;
    clock_gettime(CLOCK_REALTIME, &tm);
    localtime_r((time_t *)&tm.tv_sec, &newtime);
    strftime(tmp, 20, "%Y%m%d%H%M", &newtime);
    return (unsigned long)atol(tmp);
}
