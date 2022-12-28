#pragma once
#include "micros.h"
#if defined(_WIN32)
#include <io.h>
#include <windows.h>

#include <cstdio>
#include <ctime>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <sys/time.h>
#include <unistd.h>

#include <cstdio>
#include <cstring>
#include <ctime>
#endif

LBLOG_NAMESPACE_BEGIN
namespace platform {

#define F_OK 0 /* Check for file existence */
#define X_OK 1 /* Check for execute permission. */
#define W_OK 2 /* Check for write permission */
#define R_OK 4 /* Check for read permission */

#if defined(_WIN32)

// Code for Windows platform
using PidType = DWORD;
using TidType = DWORD;

inline auto WSAStartUp() -> bool
{
    WORD    wVersionRequested;
    WSADATA wsaData;
    int     err;

    /* Use the MAKEWORD(lowbyte, highbyte) macro declared in Windef.h */
    wVersionRequested = MAKEWORD(2, 2);

    err = WSAStartup(wVersionRequested, &wsaData);
    if (err != 0)
    {
        /* Tell the user that we could not find a usable */
        /* Winsock DLL.                                  */
        std::printf("WSAStartup failed with error: %d\n", err);
        return false;
    }

    /* Confirm that the WinSock DLL supports 2.2.*/
    /* Note that if the DLL supports versions greater    */
    /* than 2.2 in addition to 2.2, it will still return */
    /* 2.2 in wVersion since that is the version we      */
    /* requested.                                        */

    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2)
    {
        /* Tell the user that we could not find a usable */
        /* WinSock DLL.                                  */
        std::printf("Could not find a usable version of Winsock.dll\n");
        WSACleanup();
        return false;
    }
    return true;
}

struct cleanHelper
{
    ~cleanHelper() { WSACleanup(); }
};

inline auto GetPid() -> PidType { return GetCurrentProcessId(); }
inline auto GetTid() -> TidType { return GetCurrentThreadId(); }
inline auto GetHostname(char* name, size_t len) -> int
{
    if (!WSAStartUp()) return -1;
    cleanHelper h;
    return gethostname(name, static_cast<int>(len));
}
inline auto GetLocalTime_r(std::time_t* timer, std::tm* tm) -> std::tm*
{
    if (localtime_s(tm, timer) < 0)
    {
        return nullptr;
    }
    return tm;
}
inline auto GetGmTime_r(std::time_t* timer, std::tm* tm) -> std::tm*
{
    if (gmtime_s(tm, timer) < 0)
    {
        return nullptr;
    }
    return tm;
}
inline auto GetStrError_r(int err_code, char* buf, size_t len) -> char*
{
    if (strerror_s(buf, len, err_code) < 0)
    {
        return nullptr;
    }
    return buf;
}

inline auto CallAccess(const char* filename, int perm) -> int
{
    return _access(filename, perm);
}
inline auto CallUnlockedWrite(const void* ptr, size_t size, size_t n,
                              FILE* stream) -> size_t
{
    return fwrite(ptr, size, n, stream);
}
inline void CallSetBuffer(FILE* stream, char* buf, unsigned size)
{
    (void)std::setvbuf(stream, buf, _IOFBF, size);
}

inline auto CallFPutsUnlocked(const char* str, FILE* file) -> int
{
    return fputs(str, file);
}

#elif defined(__linux__)
// Code for Linux platform
using PidType = pid_t;
using TidType = pid_t;

inline auto GetPid() -> PidType { return getpid(); }
inline auto GetTid() -> TidType
{
    return static_cast<TidType>(syscall(SYS_gettid));
}
inline auto GetHostname(char* name, size_t len) -> int
{
    return ::gethostname(name, len);
}
inline auto GetLocalTime_r(std::time_t* timer, std::tm* tm) -> std::tm*
{
    return ::localtime_r(timer, tm);
}
inline auto GetGmTime_r(std::time_t* timer, std::tm* tm) -> std::tm*
{
    return ::gmtime_r(timer, tm);
}
inline auto GetStrError_r(int err_code, char* buf, size_t len) -> char*
{
    return ::strerror_r(err_code, buf, len);
}
inline auto CallAccess(const char* filename, int perm) -> int
{
    return ::access(filename, perm);
}
inline auto CallUnlockedWrite(const void* ptr, size_t size, size_t n,
                              FILE* stream) -> size_t
{
    return ::fwrite_unlocked(ptr, size, n, stream);
}
inline void CallSetBuffer(FILE* stream, char* buf, unsigned size)
{
    ::setbuffer(stream, buf, size);
}
inline auto CallFPutsUnlocked(const char* str, FILE* file) -> int
{
    return ::fputs_unlocked(str, file);
}
#else
#error "not supported compiler"
#endif
} // namespace platform
LBLOG_NAMESPACE_END