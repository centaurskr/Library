//
/// @brief   File Lock을 이용한 중복실행 방지 TEST
/// @file    IsRun.c
/// @date    2024. 12. 18. (수) 11:36:08 KST
/// @author  Cento
///
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>


////////////////////////////////////////////////////////////////////////////////
/// 중복실행 검사 (/tmp/<name> 파일에 대한 fcntl() 독점 쓰기 락으로 판단)
/// @fn      int IsRunning(char *name)
/// @param   name : Unique name (락 파일 "/tmp/<name>"으로 사용됨)
/// @return  0 : 실행중 아님(락 획득 성공, fd는 프로세스 종료까지 열려있음)
/// @return  1 : 이미 실행중(락 획득 실패, EACCES/EAGAIN)
/// @return  -1 : 락 파일 open 실패
/// @return  -2 : 기타 fcntl() 에러
////////////////////////////////////////////////////////////////////////////////
int IsRunning(char *name)
{
int fd, rtn;
struct flock lock;
char   lockname[255];
    sprintf(lockname, "/tmp/%s", name);
    fd = open(lockname, O_RDWR | O_CREAT, 0666);
    if (fd == -1){return -1;}

    lock.l_type = F_WRLCK; // Exclusive write lock
    lock.l_whence = SEEK_SET;
    lock.l_start = 0;
    lock.l_len   = 0;     // Lock the entire file

    rtn = fcntl(fd, F_SETLK, &lock);
    if(rtn == -1){
        if(errno == EACCES || errno == EAGAIN){
            close(fd);
            return 1; // Program is already running
        }
        // Error 
        close(fd);
        return -2;
    }

    return 0; // not running
}
