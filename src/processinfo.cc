//
// Created by Alone on 2022-9-21.
//

#include "elog/processinfo.h"
#if defined(_WIN32)
#include <windows.h>
#include <winsock.h>

#include <cstdio>
#elif defined(__linux__)
#include <sys/syscall.h>
#include <unistd.h>
#else
#include <unistd.h>

#include <thread>
#endif

using namespace elog;

elog::pid_t ProcessInfo::GetPid()
{
#if defined(_WIN32)
   thread_local auto pid = GetCurrentProcessId();
#else
   thread_local auto pid = getpid();
#endif
   return pid;
}

elog::tid_t ProcessInfo::GetTid()
{
#if defined(_WIN32)
   thread_local auto tid = GetCurrentThreadId();
#elif defined(__linux__)
   auto              tid = syscall(SYS_gettid);
#else
   elog::tid_t tid;

   auto id = std::this_thread::get_id();
   tid     = *reinterpret_cast<tid_t*>(&id);
#endif
   return tid;
}

#if defined(_WIN32)
void WSAStartUp()
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
      return;
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
      return;
   }
}
struct WSARegister
{
   WSARegister() { WSAStartUp(); }
   ~WSARegister() { WSACleanup(); }
};
static WSARegister s_wsa_register;
#else
#endif

const char* ProcessInfo::GetHostname()
{
   thread_local char buf[256]{};
   if (buf[0] == -1) { return buf + 1; }
   if (::gethostname(buf + 1, sizeof(buf) - 1) == 0)
   {
      buf[0] = -1;
      return buf + 1;
   }
   return "unknownhost";
}