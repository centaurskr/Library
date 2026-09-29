# libsrc

Linux 시스템 프로그래밍용 C(및 일부 C++) 유틸리티 라이브러리 모음이다.
이 저장소는 네트워크, IPC, 이벤트 루프, 로깅, JSON, ZeroMQ, 시간/날짜 처리 등을 포함한
리눅스 시스템 프로그래밍용 저수준 유틸리티를 하나의 코드베이스로 정리한 프로젝트다.

기본 규칙은 “함수 1개당 파일 1개”이며, 각 기능은 독립적인 구현 단위로 구성된다.
이 구조는 코드 재사용성과 유지보수성을 높이기 위한 설계 원칙이다.

함수 프로토타입은 이 저장소가 아니라
`~/Project/include/{TbCapi.h, Tbzmqapi.h, old/tblibC.h}` 에 선언되어 있다.

---

## 개요

### 프로젝트 목적
`libsrc`는 리눅스 환경에서 자주 사용되는 시스템 프로그래밍 기능을 간단하고 재사용 가능한
C API 형태로 정리한 라이브러리다.

주요 사용 분야:
- 네트워크 서버/클라이언트 구현
- 프로세스 간 통신(IPC)
- 이벤트 루프 기반 서비스
- 로그 수집 및 모니터링
- 시간/날짜 처리
- JSON 메시지 처리
- ZeroMQ 기반 분산 메시징
- 고정밀 숫자 연산

### 핵심 특징
- 함수 단위의 독립 구현
- Linux 중심의 저수준 네트워크/IPC 기능
- select / epoll / ZeroMQ 기반 이벤트 루프 지원
- 파일 기반 또는 멀티캐스트 기반 로깅
- 문자열 기반 고정밀 계산 지원
- 상태 관리 및 중복 실행 방지 함수 포함

---

## 빠른 시작

### 빌드
저장소 루트의 `Makefile`을 기준으로 빌드한다.

```bash
make
```

필요한 경우 옵션을 조합해 빌드할 수 있다.

```bash
make LOGOUT_TYPE=0
make LOGOUT_TYPE=2
make WITH_ZMQ=1
```

### 의존성
기본적으로 다음 환경을 전제로 한다.

- Linux
- glibc
- POSIX API
- pthreads
- system headers (`sys/socket.h`, `sys/mman.h`, `fcntl.h` 등)

선택적 의존성:
- ZeroMQ: `libzmq`
- MPFR: `libmpfr`
- 멀티캐스트 로깅: 적절한 네트워크 인터페이스 구성 필요

### 기본 사용 예시
```c
#include <stdio.h>
#include <TbCapi.h>

int main(void)
{
    unsigned long ts = GetTimestamp();
    printf("timestamp = %lu\n", ts);
    return 0;
}
```

---

## 라이브러리 구조

이 저장소는 기능 중심으로 구성되어 있으며, 각 기능은 파일 단위로 분리된다.
아래는 전체 구조를 개념적으로 나눈 것이다.

### 1. 문자열 / 날짜 / 데이터 유틸리티
주요 역할:
- 날짜 계산 및 시간 변환
- 문자열 trim, 비교, 정렬
- 파일 크기 조회
- 연결 리스트 및 메모리 맵 처리

대표 함수:
- `BinSearch`
- `IncDecNumMin`
- `IncDecNumYyyymmdd`
- `GetNumYyyymmdd`
- `GetTimestamp`
- `TbListInit`
- `MakeMapFile`
- `StrTrim`

### 2. 네트워크 / 소켓 유틸리티
주요 역할:
- TCP 클라이언트/서버
- Unix Domain Socket
- 파일 디스크립터 전달
- 로컬 주소/포트 조회

대표 함수:
- `OpenInetStreamClient`
- `OpenInetStreamServer`
- `OpenUnixStreamClient`
- `OpenUnixStreamServer`
- `WaitConnect`
- `ReadFd`
- `WriteFd`
- `ReadStream`
- `WriteStream`

### 3. 이벤트 루프
이 저장소에는 여러 이벤트 루프 구현이 존재한다.

| 구현 | 기반 | 특징 |
|------|------|------|
| `event.c` | select | 단순하고 이식성 높음 |
| `event_v2.c` | epoll | Linux 환경에서 고성능 |
| `zmqevent.c` | ZeroMQ poller | 분산 메시징 환경에 적합 |
| `tbevent.cc` | C++ epoll | C++ 환경에서 사용 가능 |

사용 가이드:
- 단순 소규모 서버: `select`
- Linux 고성능 서버: `epoll`
- ZeroMQ 기반 서비스: `zmqevent`
- C++ 프로젝트: `tbevent`

### 4. 로깅 / 프로세스 관리
주요 역할:
- 중복 실행 방지
- 파일 로깅
- 멀티캐스트 로깅
- 프로세스 상태 확인

대표 함수:
- `IsRunning`
- `InitLogout`
- `Logout`
- `NetLogout`
- `InitNetLogout`

### 5. IPC
프로세스 간 통신을 위해 다음 기능을 제공한다.

- Named Pipe
- Message Queue
- Semaphore
- Shared Memory

이 구조는 멀티프로세스 서비스와 고성능 분산 시스템에서 유용하다.

### 6. JSON 처리
기본적으로 평면 JSON 객체 처리와 문자열 기반 JSON 조립 기능을 제공한다.

대표 함수:
- `JsonParser`
- `JsonParserGet`
- `JsonInit`
- `JsonAdd`
- `JsonGetStr`
- `JsonArrayStart`

### 7. ZeroMQ 래퍼
ZeroMQ를 감싸는 얇은 C 레이어를 제공한다.
Pub/Sub, Req/Rep, Push/Pull 등 분산 메시징 패턴을 구현할 때 유용하다.

---

## 파일 구성 규칙

이 라이브러리의 중요한 설계 원칙은 다음과 같다.

- 함수 1개당 파일 1개
- 기능별 단위 파일 구성
- 파일 이름은 함수 또는 기능명을 기반으로 함
- 구현은 단일 책임 원칙을 따름

예시:
- `binsearch.c` → `BinSearch`
- `event.c` → select 기반 이벤트 루프
- `event_v2.c` → epoll 기반 이벤트 루프
- `tbjson.c` → JSON 처리
- `tbevent.cc` → C++ 이벤트 루프

이 구조는 라이브러리를 직관적으로 확장하고 유지보수하기 쉽게 만든다.

---

## 함수별 API 개요

## 문자열 / 날짜 / 데이터 유틸리티

### BinSearch
```c
void *BinSearch(const void *key, int mode, void *base, int cnt, int size, int (*compare)());
```

정렬된 배열에서 `mode`에 따라 이진 탐색을 수행한다.
표준 `bsearch`와 달리 `EQ`뿐 아니라 `LT`, `LE`, `GE`, `GT` 조건의 첫 매칭 위치도 찾을 수 있다.

**Parameters**
- `key` — 검색할 키
- `mode` — `BS_LT`, `BS_LE`, `BS_EQ`, `BS_GE`, `BS_GT`
- `base` — 정렬된 배열의 시작 포인터
- `cnt` — 원소 수
- `size` — 원소 크기
- `compare` — 비교 함수

**Example**
```c
typedef struct { int id; } Item;

int cmp(const void *k, const void *e) {
    return *(int *)k - ((Item *)e)->id;
}

Item arr[] = {{1}, {3}, {5}, {7}};
int key = 4;
Item *p = BinSearch(&key, BS_GE, arr, 4, sizeof(Item), cmp);
```

---

### IncDecNumMin / IncDecNumYyyymmdd
```c
unsigned long IncDecNumMin(unsigned long base, int incdec);
int IncDecNumYyyymmdd(int base, int incdec);
```

기준 시각 또는 기준일에서 특정 분 또는 일 수를 증감한다.

**Example**
```c
unsigned long t = IncDecNumMin(202401021230UL, 90); // 2024-01-02 12:30 + 90분
int d = IncDecNumYyyymmdd(20240102, -3); // 3일 전
```

---

### GetTimestamp / GetTimestampMs
```c
unsigned long GetTimestamp(void);
unsigned long GetTimestampMs(void);
```

현재 시간을 초 단위 또는 밀리초 단위로 반환한다.

**Example**
```c
unsigned long sec = GetTimestamp();
unsigned long ms = GetTimestampMs();
```

---

### GetStrYyyymmdd / GetStrYymmdd
```c
void GetStrYyyymmdd(char *buff);
void GetStrYymmdd(char *ymd);
```

현재 시각을 `yyyymmdd` 또는 `yymmdd` 형식으로 문자열로 변환한다.
호출 시 결과 버퍼는 반드시 충분히 확보해야 한다.

---

### TbListInit / TbListInsert / TbListDelete / TbListFetchByKey
```c
void *TbListInit(void);
int TbListInsert(void *lk, int key, void *data, int len);
int TbListDelete(void *lk, int key);
void *TbListFetchByKey(void *lk, int key);
```

단순 연결 리스트 구현이다.
`TbListInsert`는 내부적으로 데이터를 복사해 저장하므로, 호출자가 별도 소유권을 유지할 필요는 없다.

---

### MakeMapFile / AttachMapFile
```c
char *MakeMapFile(char *fname, off_t size);
void *AttachMapFile(char *filename, off_t size);
```

파일 기반 메모리 맵을 생성하거나 attach한다.

**Example**
```c
char *p = MakeMapFile("/tmp/mydata.map", 4096);
if (p) {
    p[0] = 'A';
    munmap(p, 4096);
}
```

---

### StrTrim
```c
int StrTrim(char *dst, char *src, int len);
```

문자열 앞뒤 공백을 제거한 결과를 반환한다.

**Example**
```c
char out[32];
int n = StrTrim(out, "  hello  ", 9);
```

---

### SetDoubleStr_Str / AddDoubleStr / SubDoubleStr / MulDoubleStr
```c
int SetDoubleStr_Str(DOUBLESTR *dstr, char *str);
void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
```

문자열 기반 고정밀 10진 연산을 수행한다.
기존 `double` 계산에서 발생할 수 있는 오차를 피하기 위해 설계되었다.

---

## 네트워크 / 소켓 유틸리티

### OpenInetStreamClient / OpenInetStreamServer
```c
int OpenInetStreamClient(char *host, int port);
int OpenInetStreamServer(int port);
```

TCP 클라이언트/서버 소켓을 생성하는 함수이다.

**Example**
```c
int fd = OpenInetStreamClient("example.com", 8080);
int lfd = OpenInetStreamServer(9000);
```

---

### OpenUnixStreamClient / OpenUnixStreamServer
```c
int OpenUnixStreamClient(char *unistr_path);
int OpenUnixStreamServer(char *unistr_path);
```

Unix Domain Socket 기반 통신을 위한 함수이다.

**Example**
```c
int lfd = OpenUnixStreamServer("/tmp/my.sock");
int cfd = OpenUnixStreamClient("/tmp/my.sock");
```

---

### WaitConnect
```c
int WaitConnect(int fd, char *buff);
```

리스닝 소켓에서 클라이언트 연결을 accept한다.

---

### ReadFd / WriteFd
```c
int ReadFd(int fd, char *buf, size_t buflen);
int WriteFd(int fd, int sendfd, void *ptr, size_t nbytes);
```

Unix domain socket을 통해 파일 디스크립터를 전송/수신한다.
`SCM_RIGHTS` 기반 전송을 사용한다.

---

### ReadStream / WriteStream
```c
int ReadStream(int fd, char *buff);
int WriteStream(int fd, char *buff, int sz);
```

길이 prefix 기반의 데이터 송수신을 수행한다.

---

## 이벤트 루프 / 로깅 / 프로세스 / JSON 유틸리티

### IsRunning
```c
int IsRunning(char *name);
```

`/tmp/<name>` 파일에 대해 락을 걸어 중복 실행 여부를 판단한다.

**Returns**
- `0` — 실행 중 아님
- `1` — 이미 실행 중
- `-1` — 파일 열기 실패
- `-2` — `fcntl` 에러

---

### InitLogout / Logout
```c
#define InitLogout(arc, arv, lname, valid)
#define Logout(mode, fmt, ...)
```

로그 출력 기능을 초기화하고 기록한다.
컴파일 타임 플래그에 따라 로컬 파일 또는 멀티캐스트 로그로 전송된다.

---

### InitNetLogout / NetLogout
```c
int InitNetLogout(const char *pname, char *extra);
#define NetLogout(level, fmt, ...)
```

UNIX domain datagram 소켓 기반 JSON 로거를 초기화한다.
프로세스별 로그를 중앙 수집기에 전달할 때 사용한다.

---

### JsonParser / JsonParserGet
```c
void *JsonParser(char *json);
char *JsonParserGet(void *ctx, char *key);
```

평면 JSON 객체를 파싱한다.
중첩 구조나 배열 처리는 제한적이다.

**Example**
```c
char json[] = "{\"name\":\"foo\",\"age\":30}";
void *p = JsonParser(json);
if (p) {
    printf("name=%s\n", JsonParserGet(p, "name"));
}
```

---

### JsonInit / JsonAdd / JsonGetStr
```c
void *JsonInit(void);
void JsonAdd(void *jp, char *key, char *val);
char *JsonGetStr(void *jp);
int JsonGetLen(void *jp);
```

JSON 문자열을 단계적으로 조립한다.
완성된 문자열의 길이를 가져와서 정확히 사용해야 한다.

---

## ZeroMQ 래퍼

### 핵심 개념
`libzmq(ZeroMQ)`를 감싸는 얇은 C 래퍼 함수 집합을 제공한다.
분산 메시지 처리 환경에서 유용하며, Pub/Sub, Req/Rep, Push/Pull 패턴을 구현하기 좋다.

대표 함수:
- `SendControlMsg0mq`
- `ReceiveControlMsg0mq`
- `MakeControlMsgSocket0mq`
- `CloseControlMsg0mq`
- `zmqopenpub`, `zmqopensub`, `zmqopenreq`, `zmqopenrep`

---

## IPC: Named Pipe / Message Queue / Semaphore / Shared Memory

### IPC 설계 원칙
각 IPC 메커니즘은 용도가 다르다.

- Named Pipe
  - 단순 FIFO 메시지 전달
- Message Queue
  - 대기열 기반 메시지 전송
- Semaphore
  - 동기화 및 락 제어
- Shared Memory
  - 고속 데이터 공유

이 라이브러리는 모든 IPC 구현을 독립 함수로 분리하여 재사용성과 테스트를 쉽게 한다.

---

## 에러 처리 및 주의사항

### 반환값 규칙
이 라이브러리의 함수는 보통 다음 패턴을 따른다.

- 성공: 유효한 fd 또는 포인터
- 실패: `-1`, `NULL`, 또는 0
- 일부 함수는 “실패”를 표현하는 정수 값이 여러 가지일 수 있음

예시:
```c
int fd = OpenInetStreamClient("example.com", 8080);
if (fd < 0) {
    fprintf(stderr, "connection failed\n");
    return -1;
}
```

### 주의할 점
이 라이브러리에는 일부 구현 상 제한 또는 위험 요소가 존재한다.

| 함수 | 내용 | 영향 |
|------|------|------|
| `DStrStrDiv` | 실제로는 곱셈이 수행되는 버그 가능성 | 계산 오류 |
| `WriteStream` | 내부 버퍼 크기 제한 | 스택 오버플로우 위험 |
| `WriteSize` | 일부 에러 경로에서 무한 루프 가능성 | 서비스 정지 |
| `MemCopy` | UB(Undefined Behavior) 가능성 | 비정상 동작 |
| `JsonParser` | 중첩 구조/배열 지원 제한 | 사용 제한 |
| `TB_DeleteEvent` | 등록되지 않은 fd에 대해 위험 | 크래시 가능 |

---

## FAQ

### Q: 어떤 이벤트 루프를 선택해야 하나?
- 단순 로컬 서버: `event.c`
- Linux 고성능 서버: `event_v2.c`
- ZeroMQ 메시지 기반 시스템: `zmqevent.c`
- C++ 프로젝트: `tbevent.cc`

### Q: 어떤 IPC를 선택해야 하나?
- 단순 FIFO: Named Pipe
- 메시지 큐 기반: Message Queue
- 동기화 제어: Semaphore
- 고속 공유 데이터: Shared Memory

### Q: 고정밀 연산은 어떻게 하나?
`DOUBLESTR` 기반 함수들을 사용한다.

```c
DOUBLESTR a, b, r;
SetDoubleStr_Str(&a, "9.3");
SetDoubleStr_Str(&b, "3");
AddDoubleStr(&r, &a, &b);
```

### Q: JSON은 어느 정도까지 지원하나?
기본적으로 평면 JSON 구조에 초점을 두며, 중첩 객체/배열 처리에는 제한이 있을 수 있다.

---

## 저장소 관련 참고

- `README.md`: 라이브러리 개요 및 API 문서
- `CLAUDE.md`: 빌드 방법, 구조, 개발 컨벤션
- `Makefile`: 빌드 환경 및 컴파일 설정

---

## 목차

- [문자열 / 날짜 / 데이터 유틸리티](#문자열--날짜--데이터-유틸리티)
- [네트워크 / 소켓 유틸리티](#네트워크--소켓-유틸리티)
- [이벤트 루프 / 로깅 / 프로세스 / JSON 유틸리티](#이벤트-루프--로깅--프로세스--json-유틸리티)
- [ZeroMQ 래퍼](#zeromq-래퍼)
- [IPC: Named Pipe](#ipc-named-pipe)
- [IPC: Message Queue](#ipc-message-queue)
- [IPC: Semaphore](#ipc-semaphore)
- [IPC: Shared Memory](#ipc-shared-memory)
- [서드파티 코드](#서드파티-코드)
