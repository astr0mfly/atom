#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>
#include <errno.h>
#include <string.h>

//strerror_s(char (&_Buffer)[_Size], int _ErrorMessage);

#ifndef NDEBUG
#define NDEBUG
#endif

#ifdef NDEBUG
#define logDebug(_M, ...)
#else
#define logDebug(_M, ...) fprintf(stderr, "DEBUG %s:%d: " _M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif


#ifdef __unix__

#define cleanErrno() (errno == 0 ? "None" : strerror(errno))

#define logErr(_M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " _M "\n", __FILE__, __LINE__, cleanErrno(), ##__VA_ARGS__)

#define logWarn(_M, ...) fprintf(stderr, "[WARN] (%s:%d: errno: %s) " _M "\n", __FILE__, __LINE__, cleanErrno(), ##__VA_ARGS__)

#elif _WIN32

#define LOG_BUFF_MAX_LEN 32

#define cleanErrno() (errno == 0 ? strcpy_s(logBuffer, sizeof(logBuffer), "None") : strerror_s(logBuffer, errno))

#define logError(_M, ...)     \
do {    \
    char logBuffer[LOG_BUFF_MAX_LEN];   \
    cleanErrno();       \
    fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " _M "\n", __FILE__, __LINE__, logBuffer, ##__VA_ARGS__);\
} while (0)

#define logWarn(_M, ...)     \
do {    \
    char logBuffer[LOG_BUFF_MAX_LEN];   \
    cleanErrno();       \
    fprintf(stderr, "[WARN] (%s:%d: errno: %s) " _M "\n", __FILE__, __LINE__, logBuffer, ##__VA_ARGS__);\
} while (0)

#endif

#define logInfo(_M, ...) fprintf(stderr, "[INFO]  (%s:%d) " _M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define logAlert(_M, ...) fprintf(stderr, "[ALERT]  (%s:%d) " _M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define logTrace(_M, ...) fprintf(stderr, "[TRACE]  (%s:%d) " _M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

#define check(A, _M, ...) if(!(A)) { logError(_M, ##__VA_ARGS__); errno=0; goto ERROR; }

#define sentinel(_M, ...)  { logError(_M, ##__VA_ARGS__); errno=0; goto ERROR; }

#define check_Me_M(A) check((A), "Out of _Me_Mory.")

#define checkDebug(A, _M, ...) if(!(A)) { logDebug(_M, ##__VA_ARGS__); errno=0; goto ERROR; }

#define ExitIf(b, _M) do { if (b) {logDebug(_M); exit(0);} } while (0)

#endif  //_DEBUG_H_