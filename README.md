# libsrc

Linux 시스템 프로그래밍용 C(및 일부 C++) 유틸리티 라이브러리 모음이다. 함수 1개당 파일 1개인 구성으로, 문자열/날짜 처리, 소켓, IPC(파이프/메시지큐/세마포어/공유메모리), 이벤트 루프, 로깅, ZeroMQ 래퍼, 경량 JSON 파서/빌더 등을 제공한다.

함수 프로토타입은 이 저장소가 아니라 `~/Project/include/{TbCapi.h,Tbzmqapi.h,old/tblibC.h}`에 선언되어 있다.

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

---

## 문자열 / 날짜 / 데이터 유틸리티

### BinSearch

`void *BinSearch(const void *key, int mode, void *base, int cnt, int size, int (*compare)());`

정렬된 배열에서 `mode`에 따라 이진 탐색을 수행한다. 표준 `bsearch`와 달리 EQ뿐 아니라 LT/LE/GE/GT 조건의 첫 매칭 위치도 찾을 수 있다.

**Parameters**
- `key` — 검색할 키
- `mode` — `BS_LT`, `BS_LE`, `BS_EQ`, `BS_GE`, `BS_GT` 중 하나
- `base` — 정렬된 배열의 시작 포인터
- `cnt` — 배열 원소 개수
- `size` — 원소 하나의 바이트 크기
- `compare` — `int compare(key, element)` 형태의 비교 함수(음수/0/양수 반환)

**Returns**
조건을 만족하는 첫 원소의 포인터. `BS_EQ`에서 일치 원소가 없거나 범위를 벗어나면 `NULL`.

**Example**
```c
typedef struct { int id; } Item;
int cmp(const void *k, const void *e) {
    return *(int*)k - ((Item*)e)->id;
}
Item arr[] = {{1},{3},{5},{7}};
int key = 4;
Item *p = BinSearch(&key, BS_GE, arr, 4, sizeof(Item), cmp);
// p == &arr[2] (id=5), 4 이상인 첫 원소
```

### IncDecNumMin

`unsigned long IncDecNumMin(unsigned long base, int incdec);`

기준 시각(yyyymmddhhmm)에서 `incdec`분만큼 증감한 시각을 구한다.

**Example**
```c
unsigned long t = IncDecNumMin(202401021230UL, 90); // 2024-01-02 12:30 + 90분
// t == 202401021400
```

### IncDecNumYyyymmdd

`int IncDecNumYyyymmdd(int base, int incdec);`

기준일(yyyymmdd)에서 `incdec`일만큼 증감한 날짜를 구한다.

**Example**
```c
int d = IncDecNumYyyymmdd(20240102, -3); // 3일 전
// d == 20231230
```

### NextSundayNumYyyymmdd / ThisSundayNumYyyymmdd / NextMondayNumYyyymmdd / ThisMondayNumYyyymmdd

```c
int NextSundayNumYyyymmdd(int base);
int ThisSundayNumYyyymmdd(int base);
int NextMondayNumYyyymmdd(int base);
int ThisMondayNumYyyymmdd(int base);
```

기준일이 속한(또는 다음) 주의 일요일/월요일 날짜(`yyyymmdd`)를 구한다.

**Example**
```c
int sun  = ThisSundayNumYyyymmdd(20240103); // 그 주의 일요일
int mon  = ThisMondayNumYyyymmdd(20240103); // 그 주의 월요일(주초)
int nsun = NextSundayNumYyyymmdd(20240103); // 다음 주의 일요일
int nmon = NextMondayNumYyyymmdd(20240103); // 다음 주의 월요일
```

### CleanLogfile

`int CleanLogfile(char *pt, char *f, int gdate);`

`pt` 디렉토리에서 `f` 패턴(`*`을 하나 포함할 수 있는 와일드카드)에 매치되는 파일 중, 마지막 수정일 기준으로 `gdate`일보다 오래된 파일을 삭제한다.

**Parameters**
- `pt` — 로그 디렉토리 경로
- `f` — 파일명 패턴(`"*.log"`, `"app*"`, `"*"` 등, `*`는 한 번만 사용)
- `gdate` — 보관할 일수(오늘 제외, 이보다 오래되면 삭제)

**Returns**
삭제한 파일 개수. 디렉토리를 열 수 없으면 `-1`.

**Example**
```c
int n = CleanLogfile("/var/log/myapp", "app_*.log", 7);
```

### IsEqDouble8 / IsZeroDouble8 / CompareDouble8

```c
int IsEqDouble8(DOUBLE a, DOUBLE b);
int IsZeroDouble8(DOUBLE aa);
int CompareDouble8(DOUBLE aa, DOUBLE bb);
```

`long double` 값을 소수점 이하 8자리 유효숫자 기준으로 비교한다(`EPSILON8`=5e-9 이내 차이는 같음으로 취급).

**Returns**
- `IsEqDouble8`/`IsZeroDouble8`: 1(참)/0(거짓)
- `CompareDouble8`: 0(같음)/1(a>b)/-1(a<b)

**Example**
```c
if (IsEqDouble8(1.00000001L, 1.00000002L)) { /* 같음으로 판정 */ }
int r = CompareDouble8(3.14L, 2.71L); // r == 1
```

### CutDoubleStr8

`void CutDoubleStr8(char *value);`

`"정수.소수"` 형태의 숫자 문자열을 소수점 이하 8자리까지만 남기고 in-place로 잘라낸다. `.`이 없으면 아무 동작도 하지 않는다.

**Example**
```c
char buf[32] = "3.123456789012";
CutDoubleStr8(buf);
// buf == "3.12345678"
```

### IsRoundUpDouble16

`int IsRoundUpDouble16(DOUBLE value);`

소수점 9~16번째 자리 중 0이 아닌 자리가 있는지 검사한다(반올림 필요 여부 판단용).

**Example**
```c
int need = IsRoundUpDouble16(1.0000000012345678L); // 9자리 이후에 값 있음 -> 1
```

### DoubleMode

`DOUBLE DoubleMode(DOUBLE a, DOUBLE b);`

`a % b`(나머지)를 뺄셈 반복으로 계산한다.

> ⚠️ `b <= 0`이거나 `a`가 매우 크면 while 루프가 매우 느리거나 무한 루프가 될 수 있다(단순 반복 뺄셈 구현).

**Example**
```c
DOUBLE r = DoubleMode(10.5L, 3.0L); // r == 1.5
```

### ConvertTimeStemp

`unsigned long ConvertTimeStemp(unsigned long ymd);`

`yyyymmddhhmmss` 형식의 숫자를 unix timestamp(초, local time 기준)로 변환한다.

**Example**
```c
unsigned long ts = ConvertTimeStemp(20240102123000UL);
```

### GetStrYyyymmdd / GetStrYymmdd

```c
void GetStrYyyymmdd(char *buff);
void GetStrYymmdd(char *ymd);
```

현재 시각을 각각 8자리(`yyyymmdd`)/6자리(`yymmdd`) 문자열로 얻는다. 결과 버퍼는 NUL 종료가 보장되지 않으므로 호출 전 0으로 초기화해두는 것을 권장한다.

**Example**
```c
char buf[16] = {0};
GetStrYyyymmdd(buf); // buf == "20240102"

char ymd[16] = {0};
GetStrYymmdd(ymd);   // ymd == "240102"
```

### GetNumYymmdd / GetNumYyyymmdd / GetNumYymmddhhmm / GetNumYyyymmddhhmm

```c
int GetNumYymmdd(void);
int GetNumYyyymmdd(void);
unsigned long GetNumYymmddhhmm(void);
unsigned long GetNumYyyymmddhhmm(void);
```

현재 시각을 각각 `yymmdd`/`yyyymmdd`/`yymmddhhmm`/`yyyymmddhhmm` 숫자로 얻는다.

**Example**
```c
int a = GetNumYymmdd();                 // 240102
int b = GetNumYyyymmdd();               // 20240102
unsigned long c = GetNumYymmddhhmm();   // 2401021530
unsigned long d = GetNumYyyymmddhhmm(); // 202401021530
```

### GetTimestamp / GetTimestampMs

```c
unsigned long GetTimestamp(void);
unsigned long GetTimestampMs(void);
```

현재 시각을 초/밀리초 단위 timestamp로 얻는다.

**Example**
```c
unsigned long sec = GetTimestamp();
unsigned long ms  = GetTimestampMs();
```

### GetStrYymmddhhmmsscc / GetNumYyyymmddhhmmsscc / GetNumYyyymmddhhmmssmmm

```c
void GetStrYymmddhhmmsscc(char *buff);
unsigned long GetNumYyyymmddhhmmsscc(void);
unsigned long GetNumYyyymmddhhmmssmmm(void);
```

현재 시각을 1/100초(cc, 2자리) 또는 밀리초(mmm, 3자리) 정밀도까지 포함한 문자열/숫자로 얻는다.

**Example**
```c
char buf[16];
GetStrYymmddhhmmsscc(buf); // 예: "24010215304523"

unsigned long a = GetNumYyyymmddhhmmsscc();  // 2024010215304523
unsigned long b = GetNumYyyymmddhhmmssmmm(); // 20240102153045234
```

### GetCurrTimeLong

`long GetCurrTimeLong(void);`

현재 시각을 초 단위 unix timestamp(`long`)로 얻는다.

**Example**
```c
long now = GetCurrTimeLong();
```

### GetFileSize

`int GetFileSize(char *fname);`

파일의 크기를 얻는다(`stat` 기반).

**Returns**
성공 시 파일 크기(바이트), 파일이 없거나 실패 시 `-1`.

**Example**
```c
int sz = GetFileSize("/etc/hosts");
if (sz < 0) { /* 파일 없음 */ }
```

### TbListInit / TbListInsert / TbListDelete / TbListDestory / TbListFetchByIndex / TbListFetchByKey

```c
void *TbListInit(void);
int   TbListInsert(void *lk, int key, void *data, int len);
int   TbListDelete(void *lk, int key);
void  TbListDestory(void *lk);
void *TbListFetchByIndex(void *lk, int index);
void *TbListFetchByKey(void *lk, int key);
```

`TB_LINKED_LIST`/`LINK_NODE` 기반의 단순 연결 리스트. `TbListInsert`는 `data`를 `len`바이트만큼 내부에 복사해 저장하므로 호출자가 소유권을 유지할 필요는 없다.

**Returns**
- `TbListInit`: 컨텍스트(사용 후 `TbListDestory` 필요), 실패 시 `NULL`
- `TbListInsert`: 1 성공, 0 실패(메모리 부족)
- `TbListDelete`: 1 성공, 0 해당 키 없음
- `TbListFetchByIndex`/`TbListFetchByKey`: 데이터 포인터(내부 버퍼, free 금지), 없으면 `NULL`

**Example**
```c
void *lk = TbListInit();
int val = 42;
TbListInsert(lk, 1, &val, sizeof(val));
void *p = TbListFetchByKey(lk, 1);
void *q = TbListFetchByIndex(lk, 0);
TbListDelete(lk, 1);
TbListDestory(lk);
```

### MakeMapFile / AttachMapFile

```c
char *MakeMapFile(char *fname, off_t size);
void  *AttachMapFile(char *filename, off_t size);
```

`size` 바이트짜리 파일을 새로 만들어(기존 파일이 있으면 삭제 후 재생성) memory-map하거나(`MakeMapFile`), 이미 존재하는 파일을 attach한다(`AttachMapFile`).

**Returns**
성공 시 매핑된 포인터, 실패 시 `NULL`.

**Example**
```c
char *p = MakeMapFile("/tmp/mydata.map", 4096);
if (p) { p[0] = 'A'; munmap(p, 4096); }

void *q = AttachMapFile("/tmp/mydata.map", 4096);
```

### MemCopy

`void MemCopy(char *dest, char *src, int len);`

`src`에서 `dest`로 `len`바이트를 1바이트씩 복사한다(`memcpy` 대체 구현).

> ⚠️ 내부 가드 코드(`if(i <= 0) return;`)가 반복 변수 `i`를 초기화하기 전에 읽어 undefined behavior다(로직은 그대로 두고 주석으로만 명시).

**Example**
```c
char dst[16];
MemCopy(dst, "hello", 5);
```

### Stringcmp

`int Stringcmp(char *a, char *b);`

두 NUL 종료 문자열을 비교한다(길이가 다르면 바로 다름 처리).

**Returns**
0: 같음, 그 외: 다름.

**Example**
```c
if (Stringcmp("abc", "abc") == 0) { /* 같음 */ }
```

### SetDoubleStr_Str / GetDoubleStr_Str / CompDoubleStr / AddDoubleStr / SubDoubleStr / MulDoubleStr / InitDoubleStr / IsZeroDoubleStr / CopyDoubleStr / RoundDoubleStr / DumpDoubleStr / DumpStr100

```c
int  SetDoubleStr_Str(DOUBLESTR *dstr, char *str);
void GetDoubleStr_Str(char *str, DOUBLESTR *dstr);
int  CompDoubleStr(DOUBLESTR *a, DOUBLESTR *b);
void AddDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void SubDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void MulDoubleStr(DOUBLESTR *r, DOUBLESTR *a, DOUBLESTR *b);
void InitDoubleStr(DOUBLESTR *dstr);
int  IsZeroDoubleStr(DOUBLESTR *dstr);
void CopyDoubleStr(DOUBLESTR *dst, DOUBLESTR *src);
void RoundDoubleStr(DOUBLESTR *dstr, int mode);
void DumpDoubleStr(DOUBLESTR *dstr);
void DumpStr100(char *str);
```

`TbData.h`(저장소 외부)의 `DOUBLESTR` 구조체(`sign`,`ilen`,`istr`,`flen`,`fstr`)를 기반으로 하는 임의 정밀도 10진 문자열 사칙연산(부동소수점 오차 없음) 라이브러리.

**Parameters (주요 함수)**
- `SetDoubleStr_Str`: `str`은 `"[+-]digits[.digits]"` 형식(공백 무시, 50자 미만) — 50자 이상이면 실패(-1)
- `RoundDoubleStr`의 `mode`: 1(9번째 자리 5이상 올림), 2(8자리 이하 버림), 3(9번째 자리가 0이 아니면 올림)

**Returns**
- `SetDoubleStr_Str`: 1 성공, -1 실패
- `CompDoubleStr`: 0 같음, 1 a>b, -1 a<b
- `IsZeroDoubleStr`: 1(0임)/0(0아님)
- 나머지 계산/변환 함수: void

**Example**
```c
DOUBLESTR a, b, r;
SetDoubleStr_Str(&a, "9.3");
SetDoubleStr_Str(&b, "3");
AddDoubleStr(&r, &a, &b);   // r == 12.3
SubDoubleStr(&r, &a, &b);   // r == 6.3
MulDoubleStr(&r, &a, &b);   // r == 27.9

char buf[64];
GetDoubleStr_Str(buf, &r);  // buf == "27.9"

RoundDoubleStr(&r, 1);      // 소수점 9자리 이후 반올림
DumpDoubleStr(&r);          // 디버깅용 stdout 출력
```

### StrTrim

`int StrTrim(char *dst, char *src, int len);`

`src`의 앞/뒤 공백(및 NUL)을 제거해 `dst`에 담는다.

**Returns**
trim된 문자열 길이(`len`이 0이면 0, `dst`는 건드리지 않음).

**Example**
```c
char out[32];
int n = StrTrim(out, "  hello  ", 9);
// out == "hello", n == 5
```

### WriteV

`int WriteV(int fd, char *data, int len);`

2바이트 길이 헤더(big-endian) + `data` 본문을 `writev()`로 한번에 전송한다.

**Parameters**
- `len` — 0~65535 범위여야 2바이트 헤더에 정확히 들어감

**Returns**
전송한 총 바이트 수, 실패 시 -1.

**Example**
```c
int n = WriteV(sockfd, "hello", 5);
// 상대는 2바이트 길이(0x0005) + "hello" 5바이트를 수신
```

### DStrStrMul / DStrStrAdd / DStrStrSub / DStrStrDiv

```c
void DStrStrMul(char *r, char *x, char *y, int prec, int rtype);
void DStrStrAdd(char *r, char *x, char *y, int prec, int rtype);
void DStrStrSub(char *r, char *x, char *y, int prec, int rtype);
void DStrStrDiv(char *r, char *x, char *y, int prec, int rtype);
```

MPFR(256비트 정밀도) 기반 10진 문자열 사칙연산.

**Parameters**
- `prec` — 소수점 이하 자리수
- `rtype` — 1: 올림, 0: 버림

> ⚠️ **버그**: `DStrStrDiv`는 이름과 달리 내부에서 `mpfr_div`가 아니라 `mpfr_mul`을 호출하여 실제로는 **곱셈**을 수행한다(`DStrStrMul`과 동일 동작). 나눗셈 용도로 사용하지 말 것.

**Example**
```c
char r[64];
DStrStrAdd(r, "1.005", "2.003", 2, 1); // r == "3.01" (올림)
DStrStrMul(r, "2.5", "4", 2, 0);       // r == "10.00"
```

---

## 네트워크 / 소켓 유틸리티

### GetMyHostCharAddress

`void GetMyHostCharAddress(char *addr);`

로컬 호스트의 IP 주소 문자열을 얻어온다(`gethostname`+`gethostbyname`+`inet_ntoa`).

**Parameters**
- `addr` — 결과를 저장할 버퍼(out, 64바이트 이상 권장)

**Example**
```c
char addr[64];
GetMyHostCharAddress(addr);
printf("my ip = %s\n", addr);
```

### GetPortNumber / GetPortNumberR

```c
int GetPortNumber(char *name, char *proto);
int GetPortNumberR(char *name, char *proto);
```

`/etc/services`에서 서비스 이름으로 포트 번호를 조회한다(`getservbyname`). `GetPortNumberR`은 `getservbyname_r` 기반 스레드 안전 버전(`_MAC_` 미정의 시에만 빌드).

**Returns**
성공 시 호스트 바이트 순서의 포트 번호, 실패 시 `-1`.

**Example**
```c
int port = GetPortNumber("http", "tcp");
int portR = GetPortNumberR("https", "tcp");
```

### GetWindowSize / SetWindowSize

```c
int GetWindowSize(int fd, int *x, int *y);
int SetWindowSize(int fd, int x, int y);
```

`TIOCGWINSZ`/`TIOCSWINSZ` ioctl로 터미널의 열/행 크기를 조회/설정한다.

**Returns**
성공 시 `1`, `fd`가 tty가 아니면 `-1`.

**Example**
```c
int cols, rows;
if (GetWindowSize(STDOUT_FILENO, &cols, &rows) > 0)
    printf("%dx%d\n", cols, rows);
SetWindowSize(pty_fd, 80, 24);
```

### OpenInetStreamClient / OpenInetStreamClientS

```c
int OpenInetStreamClient(char *host, int port);
int OpenInetStreamClientS(char *host, char *sname);
```

호스트명을 resolve하고 TCP 클라이언트 소켓을 연결한다(`SO_RCVBUF`/`SO_SNDBUF` 4096바이트, `SO_KEEPALIVE` 설정). `*S` 버전은 포트를 서비스 이름으로 조회한다.

**Returns**
성공 시 연결된 소켓 fd(≥0). `OpenInetStreamClient`: `-1`(호스트 조회 실패)/`-3`(socket·connect 실패). `OpenInetStreamClientS`: `-1`(호스트 조회 실패)/`-2`(서비스 조회 실패)/`-3`(socket 실패)/`-4`(connect 실패).

**Example**
```c
int fd = OpenInetStreamClient("example.com", 8080);
int fd2 = OpenInetStreamClientS("example.com", "http");
```

### OpenInetStreamServer

`int OpenInetStreamServer(int port);`

`INADDR_ANY`에 바인딩된 TCP 리스닝 소켓을 연다(`SO_REUSEADDR`/`SO_KEEPALIVE`, backlog 200).

**Returns**
성공 시 리스닝 소켓 fd. `-3001`(socket 실패), `-1`(bind 실패).

**Example**
```c
int lfd = OpenInetStreamServer(9000);
int cfd = WaitConnect(lfd, NULL);
```

### OpenUnixStreamClient / OpenUnixStreamServer

```c
int OpenUnixStreamClient(char *unistr_path);
int OpenUnixStreamServer(char *unistr_path);
```

지정된 경로의 Unix 도메인 소켓에 연결하거나(클라이언트), 리스닝 소켓을 연다(서버, bind 실패 시 경로 unlink 후 1회 재시도, backlog 10).

**Returns**
`OpenUnixStreamClient`: 성공 시 fd, `-1`(socket 실패)/`-2`(connect 실패). `OpenUnixStreamServer`: 성공 시 fd, `-1`(socket 실패)/`-2`(재시도 후에도 bind 실패).

**Example**
```c
int lfd = OpenUnixStreamServer("/tmp/my.sock");
int cfd = OpenUnixStreamClient("/tmp/my.sock");
```

### WaitConnect

`int WaitConnect(int fd, char *buff);`

리스닝 소켓에서 클라이언트 연결을 accept한다.

**Parameters**
- `buff` — 클라이언트 주소 문자열(`"xxx.xxx.xxx.xxx"`)을 받을 버퍼, `NULL`이면 무시

**Returns**
새 연결의 소켓 fd(accept 실패 시 -1, errno 설정).

**Example**
```c
char cliaddr[32];
int cfd = WaitConnect(lfd, cliaddr);
printf("client from %s\n", cliaddr);
```

### ReadFd / WriteFd

```c
int ReadFd(int fd, char *buf, size_t buflen);
int WriteFd(int fd, int sendfd, void *ptr, size_t nbytes);
```

Unix 도메인 소켓을 통해 SCM_RIGHTS ancillary data로 파일 디스크립터를 주고받는다(데이터 payload도 함께).

**Returns**
`ReadFd`: 성공 시 수신된 fd(≥0), `-errno`(recvmsg 실패)/`-2`(SCM_RIGHTS 없음). `WriteFd`: 성공 시 전송 바이트 수(`sendmsg` 결과), 실패 시 `-1`.

**Example**
```c
// 송신측
int fd_to_pass = open("/etc/hostname", O_RDONLY);
WriteFd(unix_sock, fd_to_pass, "ok", 2);

// 수신측
char payload[64];
int received_fd = ReadFd(unix_sock, payload, sizeof(payload));
```

### ReadSize / WriteSize

```c
int ReadSize(int fd, char *buff, int sz);
int WriteSize(int fd, char *buff, int sz); /* inline */
```

짧은 read/write를 반복해 정확히 `sz`바이트를 읽거나 쓴다.

**Returns**
`ReadSize`: 성공(모두 읽음) 시 `1`, 에러/EOF 시 `-1`. `WriteSize`: 성공 시 `1`, `write()`가 요청보다 많이 반환하면(비정상) `0`.

> ⚠️ `WriteSize`는 `write()`가 0 또는 음수(에러)를 반환하는 경우에 대한 가드가 없어 무한루프에 빠질 수 있다(기존 코드 그대로).

**Example**
```c
char buf[128];
if (ReadSize(fd, buf, 128) < 0) { /* error/EOF */ }
WriteSize(fd, buf, 128);
```

### ReadStream / ReadStream2 / WriteStream / WriteStream2

```c
int ReadStream(int fd, char *buff);
int ReadStream2(int fd, char *buff);
int WriteStream(int fd, char *buff, int sz);
int WriteStream2(int fd, char *buff, int sz);
```

길이 prefix + payload 프로토콜의 송수신. `*Stream`은 4바이트 길이 prefix, `*Stream2`는 2바이트 빅엔디안 길이 prefix(최대 65535)를 사용한다.

> ⚠️ `WriteStream`/`WriteStream2`는 내부 고정 스택 버퍼(`MAX_BUFF`=62580)에 bounds-check 없이 조립하므로, `sz + prefix길이`가 62580을 넘으면 스택 버퍼 오버플로가 발생한다(기존 코드 그대로, 호출 시 크기 제한을 반드시 지켜야 함).

**Returns**
`Read*`: 수신한 데이터 크기, 에러/EOF 시 `<=0`. `Write*`: 전송한 데이터 크기(prefix 제외), 실패 시 `-1`.

**Example**
```c
char buf[65536];
int n = ReadStream(fd, buf);   // 4바이트 길이 prefix 프로토콜
WriteStream(fd, "hello", 5);

int n2 = ReadStream2(fd, buf); // 2바이트 길이 prefix 프로토콜(최대 65535)
WriteStream2(fd, "hello", 5);
```

### SyncMapFile / UnmapMapFile

```c
int  SyncMapFile(void *point, size_t size);
void UnmapMapFile(void *point, size_t size);
```

`SyncMapFile`은 `msync(..., MS_ASYNC)`로 mmap 영역을 비동기 flush한다(매핑을 해제하지 않음 — 이름과 달리 **Unmap이 아님**). `UnmapMapFile`이 실제로 `munmap()`을 호출한다.

> ⚠️ `SyncMapFile`은 선언이 `int`지만 return문이 없어 반환값이 정의되지 않는다.

**Example**
```c
void *p = mmap(NULL, len, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
/* ... write to p ... */
SyncMapFile(p, len);   // flush만, 매핑 유지
UnmapMapFile(p, len);  // 실제 매핑 해제
```

---

## 이벤트 루프 / 로깅 / 프로세스 / JSON 유틸리티

이 저장소에는 서로 독립적인 **4가지** 이벤트 루프 구현이 있다: `event.c`(select 기반), `event_v2.c`(epoll 기반, C), `zmqevent.c`/`zmqevent_arg.c`(zmq_poller 기반 — ZeroMQ 섹션 참고), `tbevent.cc`(epoll 기반, C++). 용도에 맞는 하나만 골라 쓰면 된다.

### InitControlMessage / ControlMessage / GetControlMessageFd / DestroyControlMessage

```c
void *InitControlMessage(int port, char *group, int mynum);
int   ControlMessage(void *info, unsigned int to, unsigned int cmd, char *msg, int len);
int   GetControlMessageFd(void *info);
void  DestroyControlMessage(void *ctlinfo);
```

루프백(lo) 인터페이스 기반 멀티캐스트 Control Message 송수신 유틸리티.

**Parameters**
- `InitControlMessage`: `port`(멀티캐스트 포트), `group`(그룹 주소, 16바이트 이하), `mynum`(이 프로세스 식별 번호)
- `ControlMessage`: `to`(수신 대상 번호), `cmd`(Command ID), `msg`/`len`(본문, `len`이 `CTL_MSG_LEN`=80 초과 시 앞부분만 전송)

**Returns**
`InitControlMessage`: 성공 시 컨텍스트, 실패 시 `NULL`. `ControlMessage`: 항상 1. `GetControlMessageFd`: 수신 fd, `info`가 NULL이면 -1.

**Example**
```c
void *ctl = InitControlMessage(9000, "224.0.0.1", 1);
ControlMessage(ctl, PROCID_ALL, CMD_START, "GO", 2);
int fd = GetControlMessageFd(ctl); /* select()/epoll에 등록 */
DestroyControlMessage(ctl);
```

### GetEnvValue

`int GetEnvValue(char *fname, char *section, char *key, char *value);`

INI 스타일 설정 파일에서 `[section]` 아래의 `key` 값을 읽는다.

**Returns**
1: 성공, 0: 못 찾음, -1: 파일 열기 실패, -3: 라인 파싱 실패.

**Example**
```c
char value[256];
if (GetEnvValue("/etc/app.conf", "server", "port", value) == 1)
    printf("port=%s\n", value);
```

### AppEventInitSelect / AppAddEventAutoIdSelect / AppDelEventSelect / AppEventLoopShutdownSelect / AppChangeEventTimeoutSelect / AppChangeDefaultHandlerSelect / AppEventLoopSelect

```c
void *AppEventInitSelect(void);
int   AppAddEventAutoIdSelect(void *ent, int fd, int tout, int answer, int (*handler)());
int   AppDelEventSelect(void *ent, int id);
void  AppEventLoopShutdownSelect(void *ent);
void  AppChangeEventTimeoutSelect(void *ent, long sec, long usec);
void  AppChangeDefaultHandlerSelect(void *ent, int (*handler)());
void  AppEventLoopSelect(void *ent);
```

`select()` 기반 이벤트 루프(`event.c`).

**Parameters (AppAddEventAutoIdSelect)**
- `tout` — 타임아웃(1/100초 단위)
- `answer` — 타임아웃 보고 필요 여부
- `handler` — `handler(event, fd, event_id)`

**Returns**
`AppEventInitSelect`: 컨텍스트 또는 `NULL`. `AppAddEventAutoIdSelect`: event id 또는 -1. `AppDelEventSelect`: 1 성공/0 id없음.

**Example**
```c
int MyHandler(void *ent, int fd, int id) { /* ... */ return 0; }

void *ev = AppEventInitSelect();
int id = AppAddEventAutoIdSelect(ev, sockfd, 500, 1, MyHandler);
AppChangeDefaultHandlerSelect(ev, MyDefaultHandler);
AppEventLoopSelect(ev); /* AppEventLoopShutdownSelect() 호출 전까지 블록 */
```

### Event_CreateNew / Event_AddEvent / Event_GetEventArg / Event_SetEventArg / Event_FreeEventArg / Event_DeleteEvent / Event_SetDefaultHandler / Event_SetDefaultHandlerTimer / Event_SetDefaultHandlerState / Event_SetHandlerState / Event_SetEventTimeout / Event_Shutdown / Event_StartLoop

```c
void  *Event_CreateNew(void);
int    Event_AddEvent(void *info, int fd, int (*handler)(void *, int, int, void *), void *arg, int tout, int state);
void  *Event_GetEventArg(void *info, int fd);
int    Event_SetEventArg(void *info, int fd, void *arg);
int    Event_FreeEventArg(void *info, int fd);
int    Event_DeleteEvent(void *info, int fd);
void   Event_SetDefaultHandler(void *info, int (*handler)(), void *arg, int tout, int state);
void   Event_StartLoop(void *info);
```

`epoll` 기반 이벤트 루프(`event_v2.c`).

**Parameters (Event_AddEvent)**
- `handler` — `handler(ctx, fd, ewhat, arg)`
- `arg` — 사용자 데이터(해제는 호출자 책임, 또는 `Event_FreeEventArg`로 위임 가능)
- `tout` — 타임아웃(초), 0이면 리포트 안 함
- `state` — 0: PAUSE, 1: ACTIVATE

**Returns**
`Event_CreateNew`: 컨텍스트 또는 `NULL`(epoll_create 실패). `Event_AddEvent`: 1 성공/-1 calloc 실패/-2 epoll_ctl 실패. `Event_GetEventArg`: arg 포인터 또는 `NULL`. `Event_SetEventArg`/`Event_FreeEventArg`/`Event_DeleteEvent`: 1 성공/0 fd없음.

**Example**
```c
int MyHandler(void *ctx, int fd, int ewhat, void *arg) { /* ... */ return 0; }

void *ctx = Event_CreateNew();
Event_AddEvent(ctx, sockfd, MyHandler, NULL, 30, 1);
Event_SetDefaultHandler(ctx, MyDefHandler, NULL, 5, 1); /* 5초마다 */
Event_StartLoop(ctx); /* 블로킹 */
```

### IsRunning

`int IsRunning(char *name);`

`/tmp/<name>` 파일에 대한 `fcntl()` 독점 쓰기 락으로 중복 실행 여부를 검사한다.

**Returns**
0: 실행중 아님(락 획득 성공), 1: 이미 실행중, -1: 파일 open 실패, -2: 기타 fcntl 에러.

**Example**
```c
if (IsRunning("myapp") == 1) { fprintf(stderr, "already running\n"); exit(1); }
```

### InitNetLogout / NetLogout

```c
int InitNetLogout(const char *pname, char *extra);
#define NetLogout(level, fmt, ...) \
    _NetLogout(level, __FILE__, __FUNCTION__, __LINE__, fmt, ##__VA_ARGS__)
```

UNIX domain datagram 소켓으로 JSON 로그를 Vector 등 수집기에 전송하는 뮤텍스 없는 로거. `_NetLogout`을 직접 호출하지 말고 `NetLogout()` 매크로를 사용한다.

**Parameters**
- `pname` — 프로세스 식별 이름
- `extra` — 인스턴스 구분용 추가 이름(구현상 약 8바이트까지만 복사됨)
- `level` — `LOG_DEBUG`/`LOG_INFO`/`LOG_ERROR`

**Returns**
`InitNetLogout`: 0 성공, -1 pname 없음.

**Example**
```c
InitNetLogout("order-engine", "1");
NetLogout(LOG_INFO, "engine started");
NetLogout(LOG_ERROR, "failed rc=%d", rc);
```

### InitLogout / Logout / CloseLogout / ChangeDateLogout

```c
#define InitLogout(arc, arv, lname, valid)  /* LOGOUT_TYPE==0: 파일, ==2: 멀티캐스트 */
#define Logout(mode, fmt, ...)
#define CloseLogout()
#define ChangeDateLogout()
```

빌드 플래그 `-DLOGOUT_TYPE=N`에 따라 백엔드가 바뀌는 로깅 매크로.
- `LOGOUT_TYPE=0`(`st_logout.c`): `<lname>_MMDD.LOG` 로컬 파일에 append. `LOG_HOME` 환경변수로 디렉토리 지정(없으면 `/tmp`), 보관일수(`valid`) 경과분은 `CleanLogfile()`로 정리.
- `LOGOUT_TYPE=2`(`mt_logout.c`): `LOG_GROUP` 환경변수의 멀티캐스트 그룹으로 UDP 전송. `LOG_HOME`도 필요.

> ℹ️ `mt_logout.c`는 헤더(`TbLogout.h`)의 extern 선언(`_MtCloseLogout0mq`, `_MtChangeDateLogout0mq`)과 실제 구현/매크로 이름(`_MtCloseLogout`, `_MtChangeDateLogout`)이 다르다 — 매크로(`CloseLogout()`/`ChangeDateLogout()`)를 통해서만 사용할 것.

**Example**
```c
// 빌드시 -DLOGOUT_TYPE=0(파일) 또는 -DLOGOUT_TYPE=2(멀티캐스트)
InitLogout(argc, argv, "myapp", 7); /* 7일 보관 */
Logout('I', "started, pid=%d", getpid());
ChangeDateLogout(); /* 자정 등 날짜 변경 시 호출 */
CloseLogout();
```

### DecimalPack / DecimalUnPack

```c
int DecimalPack(unsigned char *dest, unsigned char *src, int sz);
int DecimalUnPack(unsigned char *dest, unsigned char *src, int sz);
```

`'0'-'9', '.', ',', '+', '-'`로만 구성된 숫자 문자열을 니블(4bit) 단위로 압축/해제한다(packed BCD 유사).

**Returns**
`dest`에 쓰여진 바이트 수(Pack) / 문자 수(UnPack).

**Example**
```c
unsigned char packed[16];
int n = DecimalPack(packed, (unsigned char *)"1234.56", 7);
unsigned char plain[16];
DecimalUnPack(plain, packed, n);
```

### CreateTimerfd / ResetTimerfd

```c
int CreateTimerfd(int sec);
int ResetTimerfd(int fd, int sec);
```

`timerfd_create()`로 select/epoll에 등록 가능한 타이머 fd를 만들고(`sec`초 주기), 필요시 주기를 재설정한다.

**Returns**
`CreateTimerfd`: 성공 시 fd, 실패 시 -1. `ResetTimerfd`: ⚠️ return문이 없어 반환값이 정의되지 않음(반환값을 사용하지 말 것).

**Example**
```c
int fd = CreateTimerfd(5); /* 5초마다 만료 */
uint64_t exp;
read(fd, &exp, sizeof(exp));
```

### OpenMtPublish / OpenMtSubscribe

```c
int OpenMtPublish(struct sockaddr_in *addr, const char *dev, const char *group, int port, int ttl);
int OpenMtSubscribe(const char *dev, const char *group, int port);
```

IPv4 멀티캐스트 발행/구독 소켓을 연다.

**Returns**
성공 시 소켓 fd, 실패 시 음수(-1~-4, 단계별 에러 코드).

**Example**
```c
struct sockaddr_in addr;
int pubfd = OpenMtPublish(&addr, "eth0", "224.0.0.1", 9000, 1);
sendto(pubfd, "hello", 5, 0, (struct sockaddr *)&addr, sizeof(addr));

int subfd = OpenMtSubscribe("eth0", "224.0.0.1", 9000);
char buf[256];
recv(subfd, buf, sizeof(buf), 0);
```

### JsonParser / JsonParserAdd / JsonParserGet / JsonParserFree

```c
void *JsonParser(char *json);
int   JsonParserAdd(void *ctx, char *str);
char *JsonParserGet(void *ctx, char *key);
void  JsonParserFree(void *ctx);
```

평면(단일 레벨) JSON 객체를 파싱하는 초경량 파서(`tbjson.c`). 중첩 객체/배열, 이스케이프된 따옴표는 지원하지 않으며 `strtok()`을 사용하므로 스레드 세이프하지 않고 입력 버퍼를 변형시킨다.

**Example**
```c
char json[] = "{\"name\":\"foo\",\"age\":30}";
void *p = JsonParser(json);
if (p) {
    printf("name=%s\n", JsonParserGet(p, "name"));
    JsonParserFree(p);
}
```

### JsonInit / JsonStart / JsonArrayStart / JsonAdd / JsonAddInt / JsonAddLong / JsonAddDouble / JsonAddLDouble / JsonEnd / JsonArrayEnd / JsonGetStr / JsonGetLen / JsonFree

```c
void  *JsonInit(void);
void   JsonStart(void *jp);
void   JsonArrayStart(void *jp, char *key);
void   JsonAdd(void *jp, char *key, char *val);
void   JsonAddInt(void *jp, char *key, int val);
void   JsonAddLong(void *jp, char *key, long val);
void   JsonAddDouble(void *jp, char *key, double val);
void   JsonAddLDouble(void *jp, char *key, long double val);
void   JsonEnd(void *jp, int type);      /* 1=continue, 0=end */
void   JsonArrayEnd(void *jp, int type);
char  *JsonGetStr(void *jp);
int    JsonGetLen(void *jp);
void   JsonFree(void *jp);
```

JSON 문자열을 점진적으로 조립하는 빌더(`tbjson.c`). 완성된 문자열은 NUL 종료가 보장되지 않으므로 항상 `JsonGetLen()`으로 얻은 길이만큼만 사용해야 한다.

**Example**
```c
void *j = JsonInit();
JsonStart(j);
JsonAdd(j, "name", "foo");
JsonAddInt(j, "age", 30);
JsonArrayStart(j, "tags");
JsonAdd(j, "", "a"); /* 배열 원소는 key를 빈 문자열로 */
JsonEnd(j, 0);
printf("%.*s\n", JsonGetLen(j), JsonGetStr(j));
JsonFree(j);
```

### TB_InitEvent / TB_AddEvent / TB_DeleteEvent / TB_RunEventLoop / TB_ShutdownEvent / TB_ChangeEventTimeout / TB_ChangeDefaultHandler

```cpp
void *TB_InitEvent(void);
int   TB_AddEvent(void *ent, int fd, int tout, bool tfg, void *data, int dtlen, Handler handler);
void  TB_DeleteEvent(void *ent, int fd);
void  TB_RunEventLoop(void *ent);
void  TB_ShutdownEvent(void *ent);
void  TB_ChangeEventTimeout(void *ent, int msec);
int   TB_ChangeDefaultHandler(void *ent, void *dt, int dtlen, DefHandler handler);
```

`tbevent.cc` — epoll 기반 C++ 이벤트 루프(저장소의 유일한 C++ 소스).

> ⚠️ `TB_DeleteEvent()`는 등록되지 않은 fd로 호출하면 크래시할 수 있다. 반드시 `TB_AddEvent()`로 등록한 fd만 넘길 것.

**Example**
```cpp
void *ev = TB_InitEvent();
TB_AddEvent(ev, sockfd, 30, true, NULL, 0, MyHandler);
TB_RunEventLoop(ev); /* blocking */
```

---

## ZeroMQ 래퍼

libzmq(ZeroMQ)를 감싸는 얇은 C 래퍼 함수 모음. 헤더는 `~/Project/include/Tbzmqapi.h`.

### SendControlMsg0mq / ReceiveControlMsg0mq / MakeControlMsgSocket0mq / CloseControlMsg0mq

```c
int   SendControlMsg0mq(int to, int cmd, char *msg);
int   ReceiveControlMsg0mq(void *socket, void *hd, char *buff, int size);
void *MakeControlMsgSocket0mq(void *ctx, int idx, char *pdev, char *sdev);
void  CloseControlMsg0mq(void *socket);
```

PUB/SUB 기반 프로세스 간 제어 메시지 송수신. `MakeControlMsgSocket0mq`가 송신용 PUB(내부 static 보관)과 수신용 SUB(`"CONTROL"` 토픽 구독)를 함께 만든다.

**Returns**
`MakeControlMsgSocket0mq`: 성공 시 SUB 소켓, 실패 시 `NULL`(반환 직후 slow-joiner 완화를 위해 1초 sleep). `SendControlMsg0mq`: 성공 시 전송 byte 수, PUB 미초기화 시 0, topic 전송 실패 시 -1. `ReceiveControlMsg0mq`: 성공 시 메시지 byte 수, topic 수신 실패 시 -1.

**Example**
```c
void *ctx = zmq_ctx_new();
void *sub = MakeControlMsgSocket0mq(ctx, 1, "tcp://*:6000", "tcp://localhost:6001");
SendControlMsg0mq(2, 100, "PAUSE");

char hd[32], msg[256];
int n = ReceiveControlMsg0mq(sub, hd, msg, sizeof(msg));

CloseControlMsg0mq(sub);
zmq_ctx_destroy(ctx);
```

### AppEventInit / AppAddEventAutoIdFd / AppAddEventAutoId0mq / AppDelEvent / AppEventLoopShutdown / AppChangeEventTimeout / AppChangeDefaultHandler / AppEventLoop

```c
void *AppEventInit(void);
int   AppAddEventAutoIdFd(void *ent, int fd, int tout, int answer, int (*handler)());
int   AppAddEventAutoId0mq(void *ent, void *fd, int tout, int answer, int (*handler)());
int   AppDelEvent(void *ent, int id);
void  AppEventLoopShutdown(void *ent);
void  AppChangeEventTimeout(void *ent, int sec);
void  AppChangeDefaultHandler(void *ent, int (*handler)());
void  AppEventLoop(void *ent);
```

`zmq_poller` 기반 이벤트 루프(`zmqevent.c`). 일반 fd와 zmq 소켓을 함께 등록해 감시할 수 있다.

> ℹ️ `AppEventLoop()`가 종료되면 내부적으로 `ent`를 free하므로, 루프 종료 후 `ent`를 재사용하면 안 된다.

**Example**
```c
int zmq_handler(void *ent, void *zsock, int id){ /* ... */ return 0; }

void *ev = AppEventInit();
AppAddEventAutoId0mq(ev, sub_socket, 0, 0, zmq_handler);
AppEventLoop(ev); /* AppEventLoopShutdown() 호출 전까지 블록 */
```

### _AppEventInit / _AppAddEventAutoIdFd / _AppAddEventAutoId0mq / _AppDelEvent / _AppEventLoopShutdown / _AppChangeEventTimeout / _AppChangeDefaultHandler / _AppEventLoop / AppEventGetUserData / AppEventSetUserData / AppEventFreeUserData

```c
void *_AppEventInit(void *udata);
void  _AppChangeEventTimeout(void *ent, int sec, long usec); /* usec 미사용, sec가 사실상 ms 단위 */
void *AppEventGetUserData(void *ent);
void  AppEventSetUserData(void *ent, void *udata);
void  AppEventFreeUserData(void *ent);
```

`zmqevent_arg.c` — `zmqevent.c`와 API 형태는 같지만 EVENT 컨텍스트에 임의의 user-data 포인터를 하나 더 들고 다닐 수 있는 버전.

> ⚠️ `zmqevent.c`와 `zmqevent_arg.c`는 동일한 전역 심볼(`MemSet`)을 각각 정의하므로 **같은 바이너리에 동시에 링크할 수 없다**. 둘 중 하나만 선택해서 링크해야 한다.

**Example**
```c
void *ev = _AppEventInit(my_ctx_ptr);
AppEventSetUserData(ev, my_state);
void *state = AppEventGetUserData(ev); // == my_state
```

### GetFdFrom0mqSocket

`int GetFdFrom0mqSocket(void *socket);`

`zmq_getsockopt(socket, ZMQ_FD, ...)`로 zmq 소켓의 underlying fd를 가져온다(select/poll/epoll에 zmq 소켓을 섞어 넣을 때 사용).

**Returns**
성공 시 fd, 실패 시 -1.

**Example**
```c
int fd = GetFdFrom0mqSocket(sub_socket);
```

### MakeMonitorSocket0mq

`void *MakeMonitorSocket0mq(void *ctx, void *sock);`

임의 이름의 inproc endpoint에 `zmq_socket_monitor(sock, ..., ZMQ_EVENT_ALL)`을 걸고, 그 endpoint에 connect하는 PAIR 소켓을 반환한다. CONNECTED/DISCONNECTED 등 소켓 상태 이벤트를 수신할 수 있다.

**Example**
```c
void *mon = MakeMonitorSocket0mq(ctx, pub_socket);
// mon에서 zmq_msg_recv로 이벤트 프레임을 읽어 상태 변화를 감시
```

### IsRunning0mq

`int IsRunning0mq(void *context, int number);`

`tcp://*:<number>`에 PUB 소켓으로 bind를 시도해 이미 같은 포트를 쓰는 프로세스(중복 실행)가 있는지 검사한다.

**Returns**
1: 이미 실행 중(bind 실패), 0: 중복 아님(bind 성공), -1: 소켓 생성 실패.

**Example**
```c
if (IsRunning0mq(ctx, 15000)) { fprintf(stderr, "already running\n"); exit(1); }
```

### GetProxyEnv

`int GetProxyEnv(char *file, int *id, char *front, char *back, char *ctl, char *cap);`

`KEY=VALUE` 형식의 환경설정 파일을 읽어 ID/FRONT/BACK/CONTROL/CAPTURE 값을 파싱한다(`#`로 시작하는 줄은 무시). ID/FRONT/BACK은 필수.

**Returns**
1: 성공, -1: 파일 open 실패, -2: 필수 키 누락/중복/형식 오류.

**Example**
```c
int id; char front[128]={0}, back[128]={0}, ctl[128]={0}, cap[128]={0};
if (GetProxyEnv("/etc/myproxy.env", &id, front, back, ctl, cap) == 1)
    printf("id=%d front=%s back=%s\n", id, front, back);
```

### _XInitLogout0mq / _XLogout0mq / _XCloseLogout0mq / _XChangeDateLogout0mq

```c
int  _XInitLogout0mq(void *ctx, int arc, char **arv, char *lname, int valid);
void _XLogout0mq(char mode, const char *file, const char *func, const int line, const char *fmt, ...);
void _XCloseLogout0mq(void);
void _XChangeDateLogout0mq(void);
```

`zmqlogout.c` — 프로세스 전역(단일 static) 로거. `LOG_PUBDEV`(zmq PUB endpoint), `LOG_HOME`(로그 디렉토리) 환경변수를 사용한다.

**Returns**
`_XInitLogout0mq`: 1 성공, -1000(lname NULL)/-1002(LOG_PUBDEV 미설정)/-2001(소켓 open 실패)/-3001(LOG_HOME 미설정).

**Example**
```c
void *ctx = zmq_ctx_new();
_XInitLogout0mq(ctx, argc, argv, "myapp", 30);
_XLogout0mq('I', __FILE__, __func__, __LINE__, "started, pid=%d", getpid());
_XChangeDateLogout0mq(); /* 자정 등 날짜 변경 시 */
_XCloseLogout0mq();
```

### _TInitLogout0mq / InitLogoutThread0mq / _TLogout0mq / _TCloseLogout0mq / _TChangeDateLogout0mq

```c
void *_TInitLogout0mq(void *ctx, int arc, char **arv, char *lname, int valid);
void *InitLogoutThread0mq(void *old);
void  _TLogout0mq(void *ep, char mode, const char *file, const char *func, const int line, const char *fmt, ...);
void  _TCloseLogout0mq(void *ep);
void  _TChangeDateLogout0mq(void *evp);
```

`zmqlogout_thr.c` — `zmqlogout.c`와 동일 프로토콜이지만 상태를 static이 아닌 `LOGENV *` 핸들로 스레드마다 따로 들고 다니는 thread-safe 버전.

**Returns**
`_TInitLogout0mq`/`InitLogoutThread0mq`: 성공 시 LOGENV 핸들, 실패 시 `NULL`.

**Example**
```c
void *mainlog = _TInitLogout0mq(ctx, argc, argv, "myapp", 30);
_TLogout0mq(mainlog, 'I', __FILE__, __func__, __LINE__, "main thread up");

void *worker(void *arg) {
    void *mylog = InitLogoutThread0mq(arg); /* arg == mainlog */
    _TLogout0mq(mylog, 'I', __FILE__, __func__, __LINE__, "worker up");
    _TCloseLogout0mq(mylog);
    return NULL;
}
```

### ReceiveMessage0mq / SendMessage0mq

```c
char *ReceiveMessage0mq(void *socket, int *rlen);
int   SendMessage0mq(void *socket, char *data, int size);
```

zmq_msg_t 기반 단일 프레임 송수신.

**Returns**
`ReceiveMessage0mq`: malloc된 데이터 포인터(**호출자가 반드시 `free()`해야 함**). `SendMessage0mq`: 성공 시 전송 byte 수, 실패 시 -1.

**Example**
```c
int len;
char *data = ReceiveMessage0mq(pull_socket, &len);
/* 사용 후 */
free(data);

SendMessage0mq(push_socket, "hello", 5);
```

### OpenPubSocket0mq / OpenPullSocket0mq / OpenRepSocket0mq / OpenReqSocket0mq / OpenSubSocket0mq

```c
void *OpenPubSocket0mq(void *ctx, char *pdev);
void *OpenPullSocket0mq(void *ctx, char *pdev);
void *OpenRepSocket0mq(void *ctx, char *pdev);
void *OpenReqSocket0mq(void *ctx, char *pdev);
void *OpenSubSocket0mq(void *ctx, char *sdev);
```

패턴별 zmq 소켓 생성(`zmq_socket()` → `zmq_connect()`). PUB는 slow-joiner 문제 완화를 위해 connect 후 1ms 대기한다. REP/REQ도 bind가 아니라 connect(브로커/프록시의 backend에 물리는 용도).

> ℹ️ `OpenSubSocket0mq`는 `ZMQ_SUBSCRIBE`를 설정하지 않으므로, 메시지를 받으려면 반환된 소켓에 `SetSubTopic0mq()`를 별도로 호출해야 한다.

**Example**
```c
void *pub = OpenPubSocket0mq(ctx, "tcp://*:6000");
void *sub = OpenSubSocket0mq(ctx, "tcp://localhost:6000");
SetSubTopic0mq(sub, ""); // 전체 구독
SendMessage0mq(pub, "hi", 2);
```

### SetSubTopic0mq / SetUnsubTopic0mq

```c
int SetSubTopic0mq(void *socket, char *topic);
int SetUnsubTopic0mq(void *socket, char *topic);
```

`ZMQ_SUBSCRIBE`/`ZMQ_UNSUBSCRIBE`로 topic 필터를 추가/제거한다. 빈 문자열이면 전체 메시지를 구독한다.

**Returns**
성공 1, 실패 0.

**Example**
```c
SetSubTopic0mq(sub, "PRICE");
SetUnsubTopic0mq(sub, "PRICE");
```

### SendTopicMessage0mq / ReceiveTopicMessage0mq / SendTopic0mq / ReceiveTopic0mq

```c
int   SendTopicMessage0mq(void *socket, char *topic, char *data, int size);
char *ReceiveTopicMessage0mq(void *socket, char *topic, int *rlen);
int   SendTopic0mq(void *socket, char *topic, char *data, int len);
int   ReceiveTopic0mq(void *socket, char *topic, char *buffer, int size);
```

topic 프레임 + data 프레임의 multipart 메시지 송수신. `*Message0mq` 계열은 zmq_msg_t + malloc 기반(반환된 data는 **호출자가 free 필요**), `SendTopic0mq`/`ReceiveTopic0mq`는 고정 버퍼 기반으로 더 가볍다.

> ⚠️ `ReceiveTopic0mq`/`ReceiveTopicMessage0mq`를 쓰려면 사전에 `SetSubTopic0mq()`로 구독이 설정되어 있어야 하며, `topic` 출력 버퍼는 NUL 종료되지 않으므로 충분히 크게 준비해야 한다.

**Returns**
`SendTopicMessage0mq`: 성공 시 data 프레임 byte 수, 실패 시 -1. `SendTopic0mq`/`ReceiveTopic0mq`: topic 프레임 실패 시 -1, 그 외엔 data 프레임의 `zmq_send`/`zmq_recv` 결과 그대로.

**Example**
```c
// zmq_msg_t 기반
SendTopicMessage0mq(pub, "PRICE", "100.5", 5);
char topic[64] = {0};
int len;
char *data = ReceiveTopicMessage0mq(sub, topic, &len);
free(data);

// 고정 버퍼 기반 (사전에 SetSubTopic0mq 필요)
SendTopic0mq(pub, "PRICE", "100.5", 5);
char topic2[512] = {0}, buf[1024] = {0};
int n = ReceiveTopic0mq(sub, topic2, buf, sizeof(buf));
```

---

## IPC: Named Pipe

`ipc/pipe/` — Named Pipe(FIFO) 래퍼. 관찰된 흐름(`ipc/pipe/sample/`): 서버가 `Serverpipe()`로 FIFO 노드를 만들어 읽기 fd를 얻고 `Readdatafromnpipe()` 루프로 수신, 클라이언트는 `Clientpipe()`로 열어 `Senddata2npipe()`로 전송 후 `Closenamedpipe()`로 닫고, 서버 종료 시 `Deletenamedpipe()`로 노드를 제거한다. `Senddata2npipe`/`Readdatafromnpipe`는 4바이트 길이 헤더 + 데이터의 자체 프로토콜을 사용한다.

### Makenamedpipe

`int Makenamedpipe(char *path, int mode);`

`mknod()`로 FIFO 노드를 만들고 권한을 0666으로 설정한 뒤 지정된 `mode`로 연다.

**Returns**
성공 시 파일 디스크립터, 실패 시 -1.

**Example**
```c
int fd = Makenamedpipe("/tmp/my.pipe", O_RDONLY);
```

### Opennamedpipe

`int Opennamedpipe(unsigned char *path, int mode);`

이미 존재하는 named pipe 노드를 지정된 모드로 연다(노드 생성/권한 설정은 하지 않음).

**Returns**
성공 시 파일 디스크립터, 실패 시 -1.

**Example**
```c
int fd = Opennamedpipe((unsigned char *)"/tmp/my.pipe", O_WRONLY);
```

### Closenamedpipe / Deletenamedpipe

```c
int Closenamedpipe(int pipe);
int Deletenamedpipe(int pipe, char *pname);
```

pipe fd를 닫거나(`Closenamedpipe`), 닫은 뒤 `unlink()`로 노드까지 제거한다(`Deletenamedpipe`).

**Returns**
성공 1, 실패 -1.

**Example**
```c
Closenamedpipe(fd);
Deletenamedpipe(fd2, "/tmp/my.pipe");
```

### Clientpipe

`int Clientpipe(char *pname);`

클라이언트 관점에서 named pipe를 `O_WRONLY|O_NDELAY`로 연다(내부적으로 `Opennamedpipe` 호출).

**Example**
```c
int id = Clientpipe("./TEST.PIPE");
Senddata2npipe(id, (unsigned char *)"hello", 5);
Closenamedpipe(id);
```

### Serverpipe / Serverpipe_t

```c
int Serverpipe(char *pname, int *wpipe, int delay);
int Serverpipe_t(char *pname, int *wpipe, int delay);
```

서버 관점에서 named pipe를 생성한다: 기존 경로를 unlink하고 새로 만든 뒤 읽기용 fd를 반환하고, 쓰기 전용 fd는 `*wpipe`에 돌려준다. `Serverpipe`는 `delay`가 참이면 `fcntl`로 블로킹 모드 전환까지 수행하고, `Serverpipe_t`는 그 전환 로직 없이 `Makenamedpipe`를 통해 노드를 만든다.

**Parameters**
- `wpipe` — [out] 쓰기 전용으로 연 fd
- `delay` — 0이 아니면 블로킹 모드

**Returns**
성공 시 읽기용 파일 디스크립터, 실패 시 -1.

**Example**
```c
int wpipe;
int mainid = Serverpipe("./TEST.PIPE", &wpipe, 1);
unsigned char buff[255];
while (1) {
    Readdatafromnpipe(mainid, buff);
    printf("BUFF[%s]\n", buff);
}
```

### Readdatafromnpipe / Senddata2npipe

```c
int Readdatafromnpipe(int fd, unsigned char *buff);
int Senddata2npipe(int fd, unsigned char *data, int size);
```

4바이트 길이 헤더 + 데이터 프로토콜의 송수신.

> ⚠️ `Senddata2npipe`는 헤더+데이터를 4096바이트 고정 스택 버퍼에 조립하므로 `size`는 4092바이트(4096 - sizeof(int))를 넘으면 안 된다(초과 시 버퍼 오버플로).

**Returns**
`Readdatafromnpipe`: 성공 시 읽은 바이트 수(≥0), 실패 시 실패한 `read()`의 반환값. `Senddata2npipe`: 성공 시 기록한 바이트 수(size), 실패 시 실패한 `write()`의 반환값.

**Example**
```c
unsigned char buff[4096];
int n = Readdatafromnpipe(fd, buff);

const char *msg = "hello";
Senddata2npipe(id, (unsigned char *)msg, strlen(msg));
```

---

## IPC: Message Queue

`ipc/queue/` — System V 메시지 큐 래퍼. 관찰된 흐름(`ipc/queue/sample/`): 서버가 `MakeMsgqueue(key, size)`로 큐를 만들고 `ReadMsgqueue()` 루프로 수신하다가 `CloseMsgqueue()`로 파괴, 클라이언트는 `OpenMsgqueue(key)`로 같은 키의 큐를 연 뒤 `WriteMsgqueue()`로 전송한다.

### MakeMsgqueue / ChangeQueueSize

```c
int MakeMsgqueue(key_t key, unsigned short size);
int ChangeQueueSize(int id, unsigned short size);
```

`key`로 System V 메시지 큐를 생성한다.

> ⚠️ `key`에 이미 큐가 존재하면 **먼저 파괴**(`msgctl IPC_RMID`)하고 새로 만든다 — 기존 큐에 남아있던 메시지는 소실된다.

**Returns**
`MakeMsgqueue`: 성공 시 큐 ID, 실패 시 -1. `ChangeQueueSize`: `msgctl(IPC_STAT)` 실패 시 0, 그 외 1.

**Example**
```c
int qid = MakeMsgqueue(0x0ff00000, 3072);
```

### OpenMsgqueue

`int OpenMsgqueue(key_t key);`

클라이언트 관점에서 기존 큐의 ID를 얻는다(`msgget(key, 0)` — 생성하지 않음).

**Returns**
성공 시 큐 ID, 실패 시 -1.

**Example**
```c
int qid = OpenMsgqueue(0x0ff00000);
```

### ReadMsgqueue / WriteMsgqueue

```c
int ReadMsgqueue(int id, int type, int *rtype, int mode, char *buff);
int WriteMsgqueue(int id, int type, int size, int mode, char *buff);
```

`msgrcv()`/`msgsnd()` 래퍼. `mode`는 1이면 블로킹, 0이면 `IPC_NOWAIT`.

> ⚠️ `WriteMsgqueue`의 `size`는 내부 고정 버퍼(4096바이트) 크기를 넘으면 안 된다(경계 검사 없음).

**Returns**
`ReadMsgqueue`: 큐가 삭제되었거나 에러 시 음수, 정상 수신 시 읽은 바이트 수, `mode=0`이고 수신할 데이터가 없으면 0. `WriteMsgqueue`: 성공 시 1, 실패 시 `-errno`.

**Example**
```c
char buff[4096];
int rtype;
int n = ReadMsgqueue(qid, 0, &rtype, 1, buff);

const char *msg = "hello";
WriteMsgqueue(qid, 0, strlen(msg), 1, (char *)msg);
```

### CloseMsgqueue

`int CloseMsgqueue(int id);`

`msgctl(id, IPC_RMID, ...)`로 큐를 파괴한다.

**Returns**
성공 0, 실패 -1.

**Example**
```c
CloseMsgqueue(qid);
```

---

## IPC: Semaphore

`ipc/sema/` — System V 세마포어 래퍼. 관찰된 흐름(`ipc/sema/sample/semtest.c`): `InitSemaphore(key)`로 세마포어 셋을 만들거나 붙은 뒤, `SemaphoreOperation(sid, 1)`로 잠그고 `SemaphoreOperation(sid, 0)`으로 푼다. `SEM_UNDO` 플래그 덕분에 프로세스가 잠금을 쥔 채 죽어도 커널이 값을 되돌린다.

### InitSemaphore

`int InitSemaphore(key_t key);`

주어진 key로 멤버 1개짜리 세마포어 셋을 생성(`IPC_CREAT|IPC_EXCL`)하거나, 이미 존재하면(`EEXIST`) 해당 셋에 붙는다. 어느 경우든 마지막에 값을 1로 설정한다.

> ⚠️ 이미 다른 프로세스가 쓰고 있던 기존 세마포어에 붙는 경우에도 값이 1로 리셋된다.

**Returns**
성공 시 세마포어 셋 id(0 이상), 실패 시 `errno`에 -1을 곱한 음수 값.

**Example**
```c
#define TEST_SEM 0x85000000
int sid = InitSemaphore(TEST_SEM);
```

### SemaphoreOperation

`int SemaphoreOperation(int id, int op);`

세마포어 셋의 0번 멤버를 잠그거나(P) 푼다(V). `SEM_UNDO` 플래그로 호출한다.

**Parameters**
- `op` — `1`이면 잠금(블록 가능), 그 외 값이면 잠금 해제

**Returns**
성공 시 1, 실패 시 `semop()`의 `errno`에 -1을 곱한 음수 값.

**Example**
```c
int sid = InitSemaphore(TEST_SEM);
for (int i = 0; i < 10; i++) {
    SemaphoreOperation(sid, 1);   /* lock */
    printf("[%d] critical section\n", i);
    sleep(1);
    SemaphoreOperation(sid, 0);   /* unlock */
}
```

---

## IPC: Shared Memory

`ipc/shm/` — System V 공유메모리 래퍼. 일반적 흐름: `Makeshm()`(서버, 최초 1회) 또는 `Getshmid()`(기존 세그먼트를 찾는 쪽) → `Attachshm()`으로 프로세스 주소공간에 붙임 → 사용 후 `Detachshm()` → 완전히 끝났으면 `Removeshm()`으로 커널에서 제거.

### Getshmid

`int Getshmid(key_t shmkey);`

주어진 key로 이미 존재하는 공유메모리 세그먼트의 id를 조회한다(새로 생성하지 않음).

**Returns**
성공 시 양수 shmid, 세그먼트가 없거나 실패하면 -1.

**Example**
```c
int shmid = Getshmid(0x85000001);
```

### Makeshm

`int Makeshm(key_t shmkey, long nbyte);`

주어진 key로 `nbyte` 크기의 공유메모리 세그먼트를 새로 만든다.

> ⚠️ 같은 key로 이미 존재하는 세그먼트가 있으면 `Removeshm()`으로 먼저 삭제한 뒤 새로 생성한다 — 기존 데이터는 사라진다.

**Returns**
성공 시 양수 shmid. `shmkey == IPC_PRIVATE`면 -1, 기존 세그먼트 삭제 실패 시 -2, `shmget()` 실패 시 -3.

**Example**
```c
int shmid = Makeshm(0x85000001, 4096);
```

### Attachshm / Detachshm

```c
char *Attachshm(int shmid, char *shmaddr, int flag);
int   Detachshm(char *vaddr);
```

공유메모리 세그먼트를 프로세스 주소공간에 붙이거나(`shmat()`) 뗀다(`shmdt()`).

**Returns**
`Attachshm`: 성공 시 붙은 세그먼트의 시작 주소, 실패 시 `(char *)-1`(주의: 기존 주석엔 "NULL"로 적혀 있었으나 실제로는 `-1`이므로 `== (char *)-1`로 비교해야 함). `Detachshm`: 성공 1, 실패 -1.

**Example**
```c
char *seg = Attachshm(shmid, NULL, 0);
if (seg == (char *)-1) { perror("shmat"); exit(1); }
strcpy(seg, "hello");
Detachshm(seg);
```

### Removeshm

`int Removeshm(key_t shmkey);`

주어진 key의 공유메모리 세그먼트를 커널에서 완전히 제거한다(`shmctl(..., IPC_RMID, ...)`).

**Returns**
성공 1, 세그먼트를 찾지 못했거나 실패 시 -1.

**Example**
```c
Removeshm(0x85000001);
```

---

## 서드파티 코드

`yyjson.c`는 벤더링된 서드파티 JSON 파서/직렬화 라이브러리(MIT License, [ibireme/yyjson](https://github.com/ibireme/yyjson))다. 자체 주석을 건드리지 않았으며 개별 함수 문서화 대상에서 제외했다. 공개 API는 `yyjson_*` 접두사를 가지며 상세 사용법은 업스트림 문서를 참고할 것.
