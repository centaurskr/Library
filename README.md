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

### 문자열 / 날짜 / 데이터 유틸리티

#### BinSearch
```c
void *BinSearch(const void *key, int mode, void *base, 
                int cnt, int size, int (*compare)());
```

정렬된 배열에서 이진 탐색을 수행합니다. 표준 `bsearch`와 달리 LT/LE/GE/GT 조건도 지원합니다.

**Parameters**
- `key` - 검색할 키
- `mode` - `BS_LT`, `BS_LE`, `BS_EQ`, `BS_GE`, `BS_GT`
- `base` - 정렬된 배열 포인터
- `cnt` - 배열 원소 수
- `size` - 원소 크기
- `compare` - 비교 함수 (음수/0/양수 반환)

**Returns**
- 조건을 만족하는 첫 원소의 포인터
- `BS_EQ`에서 미발견 시 `NULL`

**Example**
```c
typedef struct { int id; } Item;

int cmp(const void *k, const void *e) {
    return *(int *)k - ((Item *)e)->id;
}

Item arr[] = {{1}, {3}, {5}, {7}};
int key = 4;
Item *p = BinSearch(&key, BS_GE, arr, 4, sizeof(Item), cmp);
// p == &arr[2] (id=5, 4 이상인 첫 원소)
```

---

#### GetTimestamp / GetTimestampMs
```c
unsigned long GetTimestamp(void);        // 초 단위
unsigned long GetTimestampMs(void);      // 밀리초 단위
```

현재 시간을 unix timestamp로 반환합니다.

**Returns**
- 초 또는 밀리초 단위 timestamp

**Example**
```c
unsigned long sec = GetTimestamp();
unsigned long ms = GetTimestampMs();
printf("sec=%lu, ms=%lu\n", sec, ms);
```

---

#### GetNumYyyymmdd / GetNumYymmdd
```c
int GetNumYyyymmdd(void);    // yyyymmdd 형식
int GetNumYymmdd(void);      // yymmdd 형식
unsigned long GetNumYyyymmddhhmm(void);  // yyyymmddhhmm
```

현재 날짜/시간을 숫자 형식으로 반환합니다.

**Returns**
- 날짜/시간 숫자 (예: 20240102, 240102, 202401021530)

**Example**
```c
int date = GetNumYyyymmdd();       // 20240102
unsigned long datetime = GetNumYyyymmddhhmm();  // 202401021530
```

---

#### GetStrYyyymmdd / GetStrYymmdd
```c
void GetStrYyyymmdd(char *buff);   // yyyymmdd 형식 문자열
void GetStrYymmdd(char *buff);     // yymmdd 형식 문자열
```

현재 날짜를 문자열로 반환합니다. **NUL 종료가 보장되지 않으므로 호출 전 버퍼를 0으로 초기화해야 합니다.**

**Example**
```c
char buf[16] = {0};
GetStrYyyymmdd(buf);
// buf == "20240102"
```

---

#### IncDecNumYyyymmdd / IncDecNumMin
```c
int IncDecNumYyyymmdd(int base, int incdec);        // 날짜 계산
unsigned long IncDecNumMin(unsigned long base, int incdec);  // 시간 계산
```

기준 날짜/시간에서 일수 또는 분을 더하거나 뺍니다.

**Parameters**
- `base` - 기준 날짜/시간
- `incdec` - 증감량 (양수: +, 음수: -)

**Returns**
- 계산된 날짜/시간

**Example**
```c
int date = IncDecNumYyyymmdd(20240102, -3);  // 3일 전
unsigned long time = IncDecNumMin(202401021230UL, 90);  // 90분 후
```

---

#### TbListInit / TbListInsert / TbListDelete / TbListFetchByKey
```c
void *TbListInit(void);
int TbListInsert(void *lk, int key, void *data, int len);
int TbListDelete(void *lk, int key);
void *TbListFetchByKey(void *lk, int key);
void TbListDestory(void *lk);
```

단순 연결 리스트를 관리합니다. 내부적으로 데이터를 복사해 저장하므로 호출자가 메모리 관리를 걱정할 필요가 없습니다.

**Parameters**
- `lk` - 리스트 컨텍스트
- `key` - 데이터 식별 키
- `data` - 저장할 데이터 포인터
- `len` - 데이터 길이

**Returns**
- `TbListInit` - 컨텍스트 (실패 시 `NULL`)
- `TbListInsert` - 1(성공) 또는 0(실패)
- `TbListDelete` - 1(성공) 또는 0(미발견)
- `TbListFetchByKey` - 데이터 포인터 (미발견 시 `NULL`)

**Example**
```c
void *list = TbListInit();
int val = 42;
TbListInsert(list, 1, &val, sizeof(val));
int *p = (int *)TbListFetchByKey(list, 1);  // p == 42
TbListDelete(list, 1);
TbListDestory(list);
```

---

#### MakeMapFile / AttachMapFile
```c
char *MakeMapFile(char *fname, off_t size);
void *AttachMapFile(char *filename, off_t size);
```

파일 기반 메모리 맵을 생성하거나 attach합니다. `MakeMapFile`은 기존 파일을 삭제하고 새로 생성합니다.

**Parameters**
- `fname` - 파일 경로
- `size` - 크기

**Returns**
- 매핑된 메모리 포인터 (실패 시 `NULL`)

**Example**
```c
char *p = MakeMapFile("/tmp/data.map", 4096);
if (p) {
    p[0] = 'A';
    munmap(p, 4096);
}
```

---

#### StrTrim
```c
int StrTrim(char *dst, char *src, int len);
```

문자열 앞뒤의 공백을 제거한 결과를 반환합니다.

**Parameters**
- `dst` - 결과 버퍼
- `src` - 입력 문자열
- `len` - 입력 길이

**Returns**
- trim된 문자열의 길이

**Example**
```c
char out[32];
int n = StrTrim(out, "  hello  ", 9);
// out == "hello", n == 5
```

---

#### SetDoubleStr_Str / AddDoubleStr / SubDoubleStr / MulDoubleStr
```c
int SetDoubleStr_Str(DOUBLESTR *dstr, char *str);
void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
```

`DOUBLESTR` 구조체를 사용하여 문자열 기반 고정밀 십진 연산을 수행합니다. 부동소수점 오차가 발생하지 않습니다.

**Parameters**
- `dstr` / `a`, `b` - DOUBLESTR 포인터
- `str` - 문자열 (형식: `[+-]digits[.digits]`)

**Returns**
- `SetDoubleStr_Str` - 1(성공) 또는 -1(실패)

**Example**
```c
DOUBLESTR a, b, r;
SetDoubleStr_Str(&a, "9.3");
SetDoubleStr_Str(&b, "3");
AddDoubleStr(&r, &a, &b);  // r = 12.3 (정확)
char buf[64];
GetDoubleStr_Str(buf, &r);
printf("%s\n", buf);  // "12.3"
```

---

### 네트워크 / 소켓 유틸리티

#### OpenInetStreamClient / OpenInetStreamClientS
```c
int OpenInetStreamClient(char *host, int port);
int OpenInetStreamClientS(char *host, char *sname);
```

TCP 클라이언트 소켓을 생성하고 연결합니다. `*S` 버전은 포트를 서비스 이름으로 지정합니다.

**Parameters**
- `host` - 호스트명 또는 IP
- `port` - 포트 번호
- `sname` - 서비스 이름 (예: "http", "https")

**Returns**
- 연결된 소켓 fd (실패 시 음수)

**Example**
```c
int fd = OpenInetStreamClient("example.com", 8080);
if (fd < 0) {
    perror("connection failed");
    return -1;
}
// 소켓 사용...
close(fd);
```

---

#### OpenInetStreamServer
```c
int OpenInetStreamServer(int port);
```

TCP 리스닝 소켓을 생성합니다. `INADDR_ANY`에 바인딩되며, `SO_REUSEADDR` 및 `SO_KEEPALIVE` 옵션이 설정됩니다.

**Parameters**
- `port` - 포트 번호

**Returns**
- 리스닝 소켓 fd (실패 시 음수)

**Example**
```c
int lfd = OpenInetStreamServer(9000);
if (lfd < 0) {
    perror("bind failed");
    return -1;
}
int cfd = WaitConnect(lfd, NULL);
```

---

#### OpenUnixStreamClient / OpenUnixStreamServer
```c
int OpenUnixStreamClient(char *unistr_path);
int OpenUnixStreamServer(char *unistr_path);
```

Unix Domain Socket을 생성합니다. 로컬 프로세스 간 통신에 사용됩니다.

**Parameters**
- `unistr_path` - 소켓 파일 경로 (예: "/tmp/my.sock")

**Returns**
- 소켓 fd (실패 시 음수)

**Example**
```c
int lfd = OpenUnixStreamServer("/tmp/my.sock");
int cfd = OpenUnixStreamClient("/tmp/my.sock");
```

---

#### WaitConnect
```c
int WaitConnect(int fd, char *buff);
```

리스닝 소켓에서 클라이언트 연결을 accept합니다.

**Parameters**
- `fd` - 리스닝 소켓
- `buff` - 클라이언트 주소 문자열을 받을 버퍼 (또는 `NULL`)

**Returns**
- 새로운 연결 소켓 fd (accept 실패 시 -1)

**Example**
```c
char addr[32];
int cfd = WaitConnect(lfd, addr);
if (cfd < 0) {
    perror("accept failed");
    return -1;
}
printf("client from %s\n", addr);
```

---

#### ReadFd / WriteFd
```c
int ReadFd(int fd, char *buf, size_t buflen);
int WriteFd(int fd, int sendfd, void *ptr, size_t nbytes);
```

Unix domain socket을 통해 파일 디스크립터를 전송/수신합니다. `SCM_RIGHTS` ancillary data를 사용합니다.

**Parameters**
- `fd` - Unix domain socket
- `buf` / `ptr` - 데이터 버퍼
- `sendfd` - 전송할 파일 디스크립터
- `buflen` / `nbytes` - 버퍼/데이터 크기

**Returns**
- `ReadFd` - 수신된 fd (실패 시 음수)
- `WriteFd` - 전송 바이트 수 (실패 시 -1)

**Example**
```c
// 송신측
int fd_to_pass = open("/etc/hostname", O_RDONLY);
WriteFd(unix_sock, fd_to_pass, "ok", 2);

// 수신측
char payload[64];
int received_fd = ReadFd(unix_sock, payload, sizeof(payload));
```

---

#### ReadStream / ReadStream2 / WriteStream / WriteStream2
```c
int ReadStream(int fd, char *buff);
int ReadStream2(int fd, char *buff);
int WriteStream(int fd, char *buff, int sz);
int WriteStream2(int fd, char *buff, int sz);
```

길이 prefix 기반 프로토콜로 데이터를 송수신합니다.
- `*Stream` - 4바이트 길이 prefix
- `*Stream2` - 2바이트 빅엔디안 길이 prefix (최대 65535)

**Parameters**
- `fd` - 소켓
- `buff` - 데이터 버퍼
- `sz` - 데이터 크기

**Returns**
- 송수신한 바이트 수 (실패 시 ≤ 0)

**Example**
```c
char buf[65536];
int n = ReadStream(fd, buf);
if (n > 0) {
    printf("received %d bytes\n", n);
}
WriteStream(fd, "hello", 5);
```

⚠️ **주의**: `WriteStream`은 내부 버퍼 크기가 62KB로 제한되어 있습니다.

---

### 이벤트 루프 / 로깅 / 프로세스

#### IsRunning
```c
int IsRunning(char *name);
```

프로세스 중복 실행 여부를 확인합니다. `/tmp/<name>` 파일에 대한 exclusive lock을 사용합니다.

**Parameters**
- `name` - 프로세스 식별 이름

**Returns**
- `0` - 실행 중 아님 (락 획득 성공)
- `1` - 이미 실행 중
- `-1` - 파일 열기 실패
- `-2` - fcntl 에러

**Example**
```c
if (IsRunning("myapp") == 1) {
    fprintf(stderr, "already running\n");
    exit(1);
}
```

---

#### InitLogout / Logout / CloseLogout / ChangeDateLogout
```c
#define InitLogout(arc, arv, lname, valid)
#define Logout(mode, fmt, ...)
#define CloseLogout()
#define ChangeDateLogout()
```

파일 기반 또는 멀티캐스트 기반 로깅을 수행합니다. 빌드 플래그 `LOGOUT_TYPE`에 따라 동작이 결정됩니다.
- `LOGOUT_TYPE=0` - 로컬 파일에 append
- `LOGOUT_TYPE=2` - 멀티캐스트로 UDP 전송

**Example**
```c
InitLogout(argc, argv, "myapp", 7);  // 7일 보관
Logout('I', "started, pid=%d", getpid());
ChangeDateLogout();  // 자정에 호출
CloseLogout();
```

---

#### InitNetLogout / NetLogout
```c
int InitNetLogout(const char *pname, char *extra);
#define NetLogout(level, fmt, ...)
```

UNIX domain datagram socket 기반 JSON 로거를 초기화합니다. 뮤텍스 없이 안전합니다.

**Parameters**
- `pname` - 프로세스 이름
- `extra` - 추가 구분 정보 (최대 약 8바이트)
- `level` - `LOG_DEBUG`, `LOG_INFO`, `LOG_ERROR`

**Returns**
- `InitNetLogout` - 0(성공) 또는 -1(실패)

**Example**
```c
InitNetLogout("order-engine", "1");
NetLogout(LOG_INFO, "engine started");
NetLogout(LOG_ERROR, "failed rc=%d", rc);
```

---

#### AppEventInitSelect / AppAddEventAutoIdSelect / AppEventLoopSelect
```c
void *AppEventInitSelect(void);
int AppAddEventAutoIdSelect(void *ent, int fd, int tout, 
                            int answer, int (*handler)());
void AppEventLoopSelect(void *ent);
void AppEventLoopShutdownSelect(void *ent);
```

select 기반 이벤트 루프를 구현합니다. 단순하고 이식성이 좋습니다.

**Parameters**
- `ent` - 이벤트 컨텍스트
- `fd` - 감시할 파일 디스크립터
- `tout` - 타임아웃 (1/100초 단위)
- `answer` - 타임아웃 보고 여부
- `handler` - 콜백 함수 `int (*handler)(void *ent, int fd, int id)`

**Returns**
- `AppEventInitSelect` - 컨텍스트
- `AppAddEventAutoIdSelect` - event id 또는 -1

**Example**
```c
int handler(void *ent, int fd, int id) {
    char buf[256];
    ssize_t n = read(fd, buf, sizeof(buf));
    return 0;
}

void *ev = AppEventInitSelect();
AppAddEventAutoIdSelect(ev, sockfd, 500, 1, handler);
AppEventLoopSelect(ev);  // 블로킹
```

---

#### Event_CreateNew / Event_AddEvent / Event_StartLoop
```c
void *Event_CreateNew(void);
int Event_AddEvent(void *info, int fd, int (*handler)(), 
                   void *arg, int tout, int state);
void Event_StartLoop(void *info);
void Event_DeleteEvent(void *info, int fd);
```

epoll 기반 이벤트 루프를 구현합니다. Linux에서 고성능입니다.

**Parameters**
- `info` - 이벤트 컨텍스트
- `fd` - 감시할 파일 디스크립터
- `handler` - 콜백 함수 `int (*handler)(void *ctx, int fd, int ewhat, void *arg)`
- `arg` - 사용자 데이터 포인터
- `tout` - 타임아웃 (초)
- `state` - 0(PAUSE) 또는 1(ACTIVATE)

**Returns**
- `Event_CreateNew` - 컨텍스트
- `Event_AddEvent` - 1(성공) 또는 음수(실패)

**Example**
```c
int handler(void *ctx, int fd, int ewhat, void *arg) {
    char buf[256];
    read(fd, buf, sizeof(buf));
    return 0;
}

void *ctx = Event_CreateNew();
Event_AddEvent(ctx, sockfd, handler, NULL, 30, 1);
Event_StartLoop(ctx);  // 블로킹
```

---

### JSON 처리

#### JsonParser / JsonParserGet / JsonParserFree
```c
void *JsonParser(char *json);
char *JsonParserGet(void *ctx, char *key);
void JsonParserFree(void *ctx);
```

평면 JSON 객체를 파싱합니다. 중첩 구조나 배열은 지원하지 않습니다.

**Parameters**
- `json` - JSON 문자열
- `ctx` - 파서 컨텍스트
- `key` - 조회할 키

**Returns**
- `JsonParserGet` - 값 문자열 (미발견 시 `NULL`)

**Example**
```c
char json[] = "{\"name\":\"foo\",\"age\":30}";
void *p = JsonParser(json);
if (p) {
    printf("name=%s\n", JsonParserGet(p, "name"));
    printf("age=%s\n", JsonParserGet(p, "age"));
    JsonParserFree(p);
}
```

---

#### JsonInit / JsonStart / JsonAdd / JsonGetStr / JsonFree
```c
void *JsonInit(void);
void JsonStart(void *jp);
void JsonAdd(void *jp, char *key, char *val);
void JsonAddInt(void *jp, char *key, int val);
char *JsonGetStr(void *jp);
int JsonGetLen(void *jp);
void JsonFree(void *jp);
```

JSON 문자열을 단계적으로 조립합니다. 완성된 문자열의 NUL 종료가 보장되지 않으므로 `JsonGetLen()`으로 얻은 길이를 반드시 사용해야 합니다.

**Example**
```c
void *j = JsonInit();
JsonStart(j);
JsonAdd(j, "name", "foo");
JsonAddInt(j, "age", 30);
JsonAdd(j, "status", "active");

int len = JsonGetLen(j);
char *str = JsonGetStr(j);
printf("%.*s\n", len, str);  // {"name":"foo","age":30,"status":"active"}
JsonFree(j);
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
| `DStrStrDiv` | 실제로는 곱셈 수행 (버그) | `DStrStrMul` 사용 또는 수정 필요 |
| `WriteStream` | 버퍼 크기 제한 (62KB) | 큰 데이터는 분할 전송 |
| `WriteSize` | 무한루프 위험 | 에러 처리 강화 후 사용 |
| `MemCopy` | UB(Undefined Behavior) 가능성 | `memcpy` 함수 권장 |
| `JsonParser` | 중첩 구조/배열 미지원 | 평면 JSON만 사용 |
| `TB_DeleteEvent` | 미등록 fd에 대해 크래시 | 등록된 fd만 전달 |
| `GetStrYyyymmdd` | NUL 종료 미보장 | 호출 전 버퍼 0 초기화 |
| `ResetTimerfd` | 반환값 미정의 | 반환값 사용 금지 |

---

## FAQ

**Q: 어떤 이벤트 루프를 선택해야 하나?**

- 단순 로컬 서버 → `event.c` (select)
- Linux 고성능 서버 → `event_v2.c` (epoll)
- ZeroMQ 분산 시스템 → `zmqevent.c`
- C++ 프로젝트 → `tbevent.cc`

**Q: 어떤 IPC를 선택해야 하나?**

- 단순 FIFO 통신 → Named Pipe
- 큐 기반 메시징 → Message Queue
- 동기화 제어 → Semaphore
- 고속 데이터 공유 → Shared Memory

**Q: 고정밀 계산은?**

`DOUBLESTR` 기반 함수 사용:
```c
SetDoubleStr_Str(&a, "9.3");
AddDoubleStr(&r, &a, &b);  // 정확한 계산 보장
```

**Q: JSON 중첩 구조는?**

지원하지 않습니다. 평면 JSON만 사용 가능합니다.

**Q: 스레드 안전성은?**

- `*R` 접미사 함수 (예: `GetPortNumberR`) → thread-safe
- 일반 함수 → 뮤텍스 추가 필요
- `NetLogout` → 뮤텍스 없이 thread-safe (async socket 사용)

**Q: 버퍼 크기는 호출자가 보장?**

네. 문자열 함수 대부분은 버퍼 크기를 호출자가 관리해야 합니다.
```c
char buf[16] = {0};  // 호출 전 0으로 초기화
GetStrYyyymmdd(buf);
```

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
