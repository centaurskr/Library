///
/// @file    netlog.c
/// @brief   High-performance JSON logging library using UNIX domain datagram 
///          sockets.
///
/// Design goals:
///  - Zero mutex / zero blocking on main execution path
///  - One UNIX DGRAM socket per thread (TLS)
///  - Fire-and-forget logging (Vector unavailable => log dropped)
///  - JSON formatted logs for Vector ingestion
///
/// Typical flow:
///  Application -> netlog (JSON) -> UNIX DGRAM -> Vector
///
/// @date    2025. 12. 23. (화) 15:03:59 KST
/// @author  Cento

#define _GNU_SOURCE

#include <unistd.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <sys/un.h>

#include "netlog.h"

/**
 * @def VECTOR_SOCK_PATH
 * @brief Fixed UNIX domain socket path used by Vector on every host.
 *  vector.yaml의 
 * "app_log_source" section에  "path:"와 일치해야 함
 */
#define VECTOR_SOCK_PATH "/tmp/VECTOR.UNIX.DGRAM"

/**
 * @def MAX_LOG_BUF
 * @brief Maximum size of a single JSON log packet.
 */
#define MAX_LOG_BUF      5600

/**
 * @def MAX_MSG_BUF
 * @brief Maximum size of formatted log message field.
 */
#define MAX_MSG_BUF      5120

/* -------------------------------------------------------------------------- */
/*                         Process-wide static state                           */
/* -------------------------------------------------------------------------- */

/**
 * @brief Process name included in every log message.
 *
 * Set once by InitNetLogout().
 * Read-only afterwards.
 */
static char g_pname[64];

/**
 * @brief Process extra name (logical instance number).
 *
 * Used for sharding, grouping, and retention policy on log backend.
 */
static char  g_extra[32];

/**
 * @brief Thread-local UNIX domain socket.
 *
 * Each thread owns its own socket instance:
 *  - No mutex required
 *  - No contention
 *  - No interleaving of log messages
 */
__thread int tls_sock = -1;

/* -------------------------------------------------------------------------- */
/*                             Internal helpers                                */
/* -------------------------------------------------------------------------- */

/**
 * @brief Get current timestamp in nanoseconds since UNIX epoch.
 *
 * @return Nanoseconds since 1970-01-01 00:00:00 UTC
 */
static inline long long now_nsec(void)
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000000000LL + ts.tv_nsec;
}

/**
 * @brief Convert numeric log level to short string representation.
 *
 * Mapping:
 *  - LOG_DEBUG -> "D"
 *  - LOG_INFO  -> "I"
 *  - LOG_ERROR -> "E"
 *
 * @param level Log level enum value
 * @return Short string representing log level
 */
static inline const char *level_to_str(int level)
{
    switch (level) {
    case LOG_DEBUG: return "DEBUG";
    case LOG_INFO:  return "INFO";
    case LOG_ERROR: return "ERROR";
    default:        return "UNKNOWN";  /* Unknown */
    }
}

/**
 * @brief Open and initialize a UNIX domain datagram socket for current thread.
 *
 * Socket properties:
 *  - SOCK_DGRAM: preserves message boundary
 *  - O_NONBLOCK: never block main execution path
 *  - SOCK_CLOEXEC: safe across exec()
 *
 * @return Socket file descriptor on success, -1 on failure
 */
static int open_tls_socket(void)
{
    int s = socket(AF_UNIX, SOCK_DGRAM | SOCK_CLOEXEC, 0);
    if (s < 0)
        return -1;

    /* Ensure non-blocking behavior */
    int flags = fcntl(s, F_GETFL, 0);
    fcntl(s, F_SETFL, flags | O_NONBLOCK);

    tls_sock = s;
    return s;
}

/**
 * @brief Get thread-local socket, creating it lazily if needed.
 *
 * @return Valid socket fd or -1 on failure
 */
static inline int get_tls_socket(void)
{
    if (tls_sock != -1)
        return tls_sock;

    return open_tls_socket();
}

////////////////////////////////////////////////////////////////////////////////
// @brief 네트워크 로그 시스템 초기화 함수
//
// @details 
// 어플리케이션 시작 시 로그 시스템을 준비하는 함수입니다. 환경 변수 
// `LOGSOCKET`으로부터 Unix Domain Socket(UDS) 경로를 읽어와 수집기(Vector)와의
// 통신 채널을 설정합니다.
// 또한, 실행 중 반복적인 시스템 콜 오버헤드를 줄이기 위해 호스트네임, 
// 프로세스명 등 변하지 않는 시스템 정보를 이 단계에서 미리 캐싱합니다.
//
// @note 
// - **호출 시점**: 멀티스레드가 생성되기 전, `main()` 함수의 최상단에서 
// 단일 스레드 상태로 호출하는 것을 강력히 권장합니다. 
//
// @param pname     프로세스 식별 이름 (예: "auth_server", "db_proxy")
// @param extra     동일 프로세스의 다중 인스턴스 구분을 위한 추가 이름
//
// @return 초기화 성공 여부를 반환합니다.
// @retval  0  성공: 로그 시스템이 정상적으로 준비됨
// @retval -1  실패: pname이 없습니다.
//
// @par 초기화 시 캐싱되는 정보:
// - @b pname    : 사용자 지정 프로세스명
// - @b Extra    : 인스턴스 추가 이름
//
// @attention 
// 이 함수가 성공(0)을 반환하지 않은 상태에서 `_NetLogout()`를 호출할 경우, 
// 로그는 전송되지 않으며 함수는 즉시 리턴되어 메인 비즈니스 로직에 영향을 
// 주지 않습니다.
//
// @see _NetLogout(), 
////////////////////////////////////////////////////////////////////////////////
int InitNetLogout(const char *pname, char *extra)
{
    if (!pname)
        return -1;

    strncpy(g_pname, pname, sizeof(g_pname) - 1);
    g_pname[sizeof(g_pname) - 1] = '\0';
    /* NOTE: `sizeof(g_extra - 1)` is sizeof(char*) (pointer arithmetic on
     * the decayed array), not sizeof(g_extra) - 1 as intended, so this
     * copies at most 8 bytes of `extra` regardless of g_extra's 32-byte
     * size. g_extra is a zero-initialized static buffer, so this is not a
     * buffer overflow, just an unintended truncation to ~8 chars. */
    strncpy(g_extra , extra, sizeof(g_extra - 1));

    /*
     * Socket is intentionally NOT opened here.
     * It is created lazily per-thread to avoid:
     *  - unnecessary sockets
     *  - fork()/thread ordering issues
     */
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// @brief 네트워크 통합 로그 전송 함수 (Internal API)
// * @details 
// 매크로 NetLogout()에 의해 내부적으로 호출되는 핵심 로깅 함수입니다.
// 입력받은 인자들과 InitNetLogout()에서 캐싱된 시스템 정보를 결합하여 
// 8개의 필드를 가진 JSON 객체를 생성합니다.
// 생성된 로그는 Unix Domain Socket(Datagram)을 통해 로컬 Vector Agent로 
// 전송됩니다.
// * @note 
// - **Thread-safety**: 이 함수는 재진입 가능(Reentrant)하며, 모든 JSON 객체는 
//     스택 기반의 지역 변수로 처리되므로 Mutex 없이 멀티스레드에서 안전하게 
//     호출할 수 있습니다.
// -**  Thread-local UNIX domain socket 사용
// @param level    로그 레벨 (0: INFO, 1: DEBUG, 3: ERROR)
// @param file     로그가 발생한 소스 파일 경로 (__FILE__)
// @param func     로그가 발생한 함수명 (__FUNCTION__)
// @param line     로그가 발생한 라인 번호 (__LINE__)
// @param format   printf 스타일의 메시지 포맷 스트링
// @param ...      포맷 스트링에 매핑될 가변 인자 리스트
// * @par 최종 출력 JSON 필드 구성 (11개):
// 1. @b ts        : 나노초 단위 Unix 타임스탬프 (uint64)
// 2. @b level     : 로그 레벨 문자열 (INFO/DEBUG/...)
// 3. @b pname     : 프로세스 이름
// 4. @b index     : 프로세스 인스턴스 인덱스
// 5. @b source    : 소스 파일명
// 6. @b function  : 함수명
// 7. @b line      : 라인 번호
// 8. @b message   : 포맷팅된 최종 로그 메시지
// * @see InitNetLogout()
////////////////////////////////////////////////////////////////////////////////
void _NetLogout(int level,
                const char *file,
                const char *func,
                int line,
                const char *fmt, ...)
{
    /* Not initialized */
    if (strlen(g_pname) == 0)
        return;

    int sock = get_tls_socket();
    if (sock < 0)
        return;

    char msg[MAX_MSG_BUF];
    char buf[MAX_LOG_BUF];

    /* Format user message */
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(msg, sizeof(msg), fmt, ap);
    va_end(ap);

    long long ts = now_nsec();

    /* Build JSON payload */
    int len = snprintf(buf, sizeof(buf),
        "{"
        "\"ts\":%lld,"
        "\"level\":\"%s\","
        "\"pname\":\"%s\","
        "\"extra\":\"%s\","
        "\"source\":\"%s\","
        "\"function\":\"%s\","
        "\"line\":%d,"
        "\"message\":\"%s\""
        "}",
        ts,
        level_to_str(level),
        g_pname,
        g_extra,
        file,
        func,
        line,
        msg
    );

    if (len <= 0 || len >= (int)sizeof(buf))
        return;

    struct sockaddr_un addr;
    memset(&addr, 0, sizeof(addr));
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, VECTOR_SOCK_PATH, sizeof(addr.sun_path) - 1);

    /* Fire-and-forget send */
    int rtn = sendto(sock, buf, len, 0,
           (struct sockaddr *)&addr, sizeof(addr));
}
