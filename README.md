# libsrc

Linux 시스템 프로그래밍용 C(및 C++) 유틸리티 라이브러리

네트워크, IPC, 이벤트 루프, 로깅, JSON, ZeroMQ, 날짜/시간 처리를 포함한 리눅스 시스템 프로그래밍용 저수준 유틸리티를 하나의 코드베이스로 정리한 프로젝트입니다.

**기본 설계 원칙**: 함수 1개 = 파일 1개 (재사용성과 유지보수성 중심)

> **참고**: 함수 프로토타입은 `~/Project/include/{TbCapi.h, Tbzmqapi.h, old/tblibC.h}`에 선언되어 있습니다.

---

## 개요

### 프로젝트 목적

리눅스 환경에서 자주 사용되는 시스템 프로그래밍 기능을 간단하고 재사용 가능한 C API 형태로 제공합니다.

**주요 사용 분야:**
- 네트워크 서버/클라이언트
- 프로세스 간 통신 (IPC)
- 이벤트 루프 기반 서비스
- 로그 수집 및 모니터링
- 시간/날짜 처리
- JSON 메시지 처리
- ZeroMQ 기반 분산 메시징
- 고정밀 숫자 연산

### 핵심 특징

| 특징 | 설명 |
|------|------|
| 함수 단위 독립 구현 | 각 함수는 별도 파일로 구성되어 재사용성 극대화 |
| 네트워크/IPC | Linux 중심의 저수준 소켓 및 IPC 기능 |
| 다중 이벤트 루프 | select / epoll / ZeroMQ 중 선택 |
| 로깅 옵션 | 파일 또는 멀티캐스트 기반 선택 |
| 고정밀 계산 | 문자열 기반 부동소수점 오차 제거 |

---

## 시작하기

### 빌드

```bash
make
```

### 선택 옵션

```bash
make LOGOUT_TYPE=0          # 파일 기반 로깅
make LOGOUT_TYPE=2          # 멀티캐스트 로깅
make WITH_ZMQ=1             # ZeroMQ 포함
```

### 의존성

**필수:**
- Linux
- glibc, pthreads
- POSIX headers

**선택:**
- libzmq (ZeroMQ 사용 시)
- libmpfr (고정밀 연산 사용 시)

### 기본 예제

```c
#include <stdio.h>
#include <TbCapi.h>

int main(void) {
    unsigned long ts = GetTimestamp();
    printf("timestamp = %lu\n", ts);
    return 0;
}
```

---

## 라이브러리 구조

### 1. 문자열 / 날짜 / 데이터 유틸리티

날짜 계산, 문자열 처리, 메모리 맵, 연결 리스트 등을 제공합니다.

```
BinSearch              : 이진 탐색 (LT/LE/EQ/GE/GT 지원)
GetTimestamp()         : unix timestamp (초)
GetNumYyyymmdd()       : 현재 날짜 (yyyymmdd)
TbListInit/Insert      : 연결 리스트
MakeMapFile()          : 파일 기반 메모리 맵
StrTrim()              : 문자열 trim
AddDoubleStr()         : 고정밀 십진 덧셈
```

### 2. 네트워크 / 소켓 유틸리티

TCP, Unix Domain Socket, 파일 디스크립터 전달 등을 제공합니다.

```
OpenInetStreamClient()      : TCP 클라이언트 연결
OpenInetStreamServer()      : TCP 리스닝 소켓
OpenUnixStreamClient/Svr()  : Unix domain socket
WaitConnect()               : 클라이언트 accept
ReadFd/WriteFd()            : 파일 디스크립터 전달
ReadStream/WriteStream()    : 길이 prefix 기반 송수신
```

### 3. 이벤트 루프

여러 구현 중 필요에 맞게 선택합니다.

| 이름 | 기반 | 사용처 |
|------|------|--------|
| `event.c` | select | 소규모 서버, 이식성 |
| `event_v2.c` | epoll | Linux 고성능 서버 |
| `zmqevent.c` | ZeroMQ | 분산 메시징 시스템 |
| `tbevent.cc` | C++ epoll | C++ 프로젝트 |

**선택 가이드:**
- 단순하고 이식적인 서버 → select
- Linux 고성능 필요 → epoll
- ZeroMQ 메시징 → zmqevent
- C++ 코드베이스 → tbevent

### 4. 로깅 / 프로세스 관리

```
IsRunning()           : 중복 실행 방지
InitLogout/Logout()   : 파일 로깅
NetLogout()           : JSON 기반 로깅
InitNetLogout()       : 로거 초기화
```

### 5. IPC

프로세스 간 통신을 위한 선택지를 제공합니다.

```
Named Pipe    : 단순 FIFO
Message Queue : 큐 기반 메시징
Semaphore     : 동기화 및 락
Shared Memory : 고속 공유 메모리
```

### 6. JSON 처리

평면 JSON 객체 처리를 지원합니다.

```
JsonParser/Get()  : JSON 파싱
JsonInit/Add()    : JSON 조립
JsonGetStr()      : 문자열 변환
```

### 7. ZeroMQ 래퍼

분산 메시징 패턴 (Pub/Sub, Req/Rep, Push/Pull)을 구현합니다.

```
zmqopenpub/sub()  : Pub/Sub 소켓
zmqopenreq/rep()  : Req/Rep 소켓
zmqopenpush/pull(): Push/Pull 소켓
```

---

## 파일 구성

**원칙**: 함수 1개 = 파일 1개

```
binsearch.c         → BinSearch()
event.c             → select 이벤트 루프
event_v2.c          → epoll 이벤트 루프
tbjson.c            → JSON 처리
tbevent.cc          → C++ 이벤트 루프
ipc/                → IPC 관련 함수들
```

---

## API 상세

### BinSearch

```c
void *BinSearch(const void *key, int mode, void *base, 
                int cnt, int size, int (*compare)());
```

정렬된 배열에서 이진 탐색을 수행합니다. 표준 `bsearch`와 달리 LT/LE/GE/GT 조건도 지원합니다.

```c
// 예: 4 이상인 첫 원소 찾기
Item *p = BinSearch(&key, BS_GE, arr, 4, sizeof(Item), cmp);
```

---

### 날짜/시간 함수

```c
unsigned long GetTimestamp(void);        // unix timestamp (초)
unsigned long GetTimestampMs(void);      // unix timestamp (밀리초)
int GetNumYyyymmdd(void);                // 현재 날짜 (yyyymmdd)
int IncDecNumYyyymmdd(int date, int d);  // 날짜 ± d일
```

---

### 네트워크 함수

```c
int OpenInetStreamClient(char *host, int port);
int OpenInetStreamServer(int port);

int OpenUnixStreamClient(char *path);
int OpenUnixStreamServer(char *path);

int WaitConnect(int lfd, char *addr);
```

---

### 이벤트 루프 (select)

```c
void *AppEventInitSelect(void);
int AppAddEventAutoIdSelect(void *ev, int fd, int timeout, 
                            int answer, int (*handler)());
void AppEventLoopSelect(void *ev);
void AppEventLoopShutdownSelect(void *ev);
```

---

### 이벤트 루프 (epoll)

```c
void *Event_CreateNew(void);
int Event_AddEvent(void *ctx, int fd, int (*handler)(), 
                   void *arg, int timeout, int state);
void Event_StartLoop(void *ctx);
void Event_DeleteEvent(void *ctx, int fd);
```

---

### 로깅

```c
int IsRunning(char *name);              // 중복 실행 방지

int InitNetLogout(const char *pname, char *extra);
#define NetLogout(level, fmt, ...)      // JSON 로거
```

---

### JSON

```c
void *JsonParser(char *json);
char *JsonParserGet(void *ctx, char *key);

void *JsonInit(void);
void JsonAdd(void *jp, char *key, char *val);
char *JsonGetStr(void *jp);
int JsonGetLen(void *jp);
```

---

### 고정밀 연산

```c
int SetDoubleStr_Str(DOUBLESTR *ds, char *str);
void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
```

예:
```c
DOUBLESTR a, b, r;
SetDoubleStr_Str(&a, "9.3");
SetDoubleStr_Str(&b, "3");
AddDoubleStr(&r, &a, &b);  // r = 12.3 (정확한 계산)
```

---

## 에러 처리

### 반환값 규칙

| 반환값 | 의미 |
|--------|------|
| `fd >= 0` | 소켓 성공 |
| `fd < 0` | 소켓 실패 |
| `ptr != NULL` | 메모리 할당 성공 |
| `ptr == NULL` | 메모리 할당 실패 |
| `0` | 성공 (일부 함수) |
| `1` | 성공 (일부 함수) |
| `-1` | 실패 (일부 함수) |

### 주의사항

⚠️ **버그 및 제약사항**

| 함수 | 문제 | 해결방법 |
|------|------|---------|
| `DStrStrDiv` | 실제로는 곱셈 수행 | `DStrStrMul` 사용 |
| `WriteStream` | 버퍼 크기 제한 (62KB) | 큰 데이터는 분할 |
| `WriteSize` | 무한루프 위험 | 에러 처리 강화 |
| `MemCopy` | UB 가능성 | `memcpy` 권장 |
| `JsonParser` | 중첩 구조 미지원 | 평면 JSON만 사용 |
| `TB_DeleteEvent` | 크래시 위험 | 등록된 fd만 전달 |

---

## FAQ

**Q: 어떤 이벤트 루프를 선택해야 하나?**

- 단순 서버 → `event.c` (select)
- Linux 고성능 → `event_v2.c` (epoll)
- ZeroMQ 시스템 → `zmqevent.c`
- C++ 프로젝트 → `tbevent.cc`

**Q: 어떤 IPC를 선택해야 하나?**

- 단순 FIFO → Named Pipe
- 메시지 큐 → Message Queue
- 동기화 → Semaphore
- 고속 공유 → Shared Memory

**Q: 고정밀 계산은?**

`DOUBLESTR` 기반 함수 사용 (`AddDoubleStr`, `SubDoubleStr` 등)

**Q: JSON 중첩 구조는?**

지원하지 않습니다. 평면 JSON만 사용 가능합니다.

**Q: 스레드 안전성은?**

`*R` 접미사 함수 (예: `GetPortNumberR`)가 thread-safe입니다.
일반 함수는 뮤텍스 추가 필요합니다.

---

## 참고 문서

- `CLAUDE.md` - 빌드 방법, 구조, 개발 컨벤션
- `Makefile` - 빌드 설정 파일

---

## 목차

- [개요](#개요)
- [시작하기](#시작하기)
- [라이브러리 구조](#라이브러리-구조)
- [API 상세](#api-상세)
- [에러 처리](#에러-처리)
- [FAQ](#faq)
