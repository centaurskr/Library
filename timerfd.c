//
// Description : timerfd 
// File Name   : timerfd.c
// Date        : 2021. 10. 18. (월) 12:22:49 KST
// By  : Cento
//

#include <time.h>
#include <sys/timerfd.h>

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdint.h>

////////////////////////////////////////////////////////////////////////////////
// Description : 이미 생성된 timer fd를 재설정한다. 최초 만료 시각을
//               "지금 + sec"로, 이후 반복 주기를 sec초로 설정한다
//               (TFD_TIMER_ABSTIME이므로 it_value는 절대시각).
// Prototype   : int ResetTimerfd(int fd, int sec)
// Arguments   : fd  : CreateTimerfd() 등으로 생성된 timer fd
//               sec : 만료 주기(초)
// Return      : 선언은 int이나 실제로 return 문이 없어 반환값은 정의되지
//               않는다(호출측인 CreateTimerfd()도 반환값을 쓰지 않음). 실패
//               여부가 필요하면 timerfd_settime()의 반환값을 직접 확인해야 한다.
////////////////////////////////////////////////////////////////////////////////
int ResetTimerfd(int fd, int sec)
{
struct itimerspec value;
struct timespec  now;
	clock_gettime(CLOCK_REALTIME, &now);
	value.it_value.tv_sec  = now.tv_sec + sec;
	value.it_value.tv_nsec = now.tv_nsec;

	value.it_interval.tv_sec  = sec;
	value.it_interval.tv_nsec = 0;
	timerfd_settime(fd, TFD_TIMER_ABSTIME, &value, NULL);
}

////////////////////////////////////////////////////////////////////////////////
/// select, epoll)에 사용할 Timer fd를 생성한다\n
/// timer를 처리하는 function sample\n
///     uint64_t exp, s;\n
///     s = read(fd,&exp, sizeof(uint64_t));
///
/// @fn        int CreateTimerfd(int sec)
/// @brief     timer fd 생성
/// @param     sec   timer를 위한 초
/// @return   -1:실패, timer_fd : 성공
////////////////////////////////////////////////////////////////////////////////
int CreateTimerfd(int sec)
{
int fd;
	fd = timerfd_create(CLOCK_REALTIME, 0);
	if(fd == -1) return -1;
	ResetTimerfd(fd, sec);
	return fd;
}
