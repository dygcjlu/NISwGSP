#include "netCommon.h"

// 网络库初始化
bool g_bWSAStartup = false;

//////////////////////////////////////////////////////////////////////////
// 初始化网络模块
int loadNetModule()
{
#if defined(WIN32) || defined(WIN64)
    if (!g_bWSAStartup)
    {
        WORD wVersion = MAKEWORD(2, 2);
        WSADATA wsaData;
        int nErr = WSAStartup(wVersion, &wsaData);
        if (nErr != 0)
        {
            return ReturnCode_Error;
        }
        g_bWSAStartup = true;
    }
#endif
    return ReturnCode_Success;
}

//////////////////////////////////////////////////////////////////////////
// 卸载网络模块
int unloadNetModule()
{
#if defined(WIN32) || defined(WIN64)
    if (g_bWSAStartup)
    {
        int nErr = WSACleanup();
        if (nErr != 0)
        {
            return ReturnCode_Error;
        }
        g_bWSAStartup = false;
    }
#endif
    return ReturnCode_Success;
}

//////////////////////////////////////////////////////////////////////////
// 将socket设置为非阻塞模式
INT32 setNonBlock(SOCKET socket)
{
#if defined(WIN32) || defined(WIN64)
    unsigned long flag = 1;
    return ioctlsocket(socket, FIONBIO, (unsigned long*)&flag);
#else
    int flags = fcntl(socket, F_GETFL, 0);
    return fcntl(socket, F_SETFL, flags | O_NONBLOCK);
#endif
}

//////////////////////////////////////////////////////////////////////////
// 关闭socket
INT32 closeSocket(SOCKET socket)
{
#if defined(WIN32) || defined(WIN64)
    return ::closesocket(socket);
#else
    return close(socket);
#endif
}

//////////////////////////////////////////////////////////////////////////
// 获取最近一次的错误码
INT32 getLaseError()
{
#if defined(WIN32) || defined(WIN64)
    /*
    * WSAGetLastError 和 GetLastError 都可以使用注意不要覆盖系统函数
    */
    return WSAGetLastError(); 
#else
    return errno;
#endif
}

#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp)
{
    time_t clock;
    struct tm tm;
    SYSTEMTIME wtm;

    GetLocalTime(&wtm);
    tm.tm_year = wtm.wYear - 1900;
    tm.tm_mon = wtm.wMonth - 1;
    tm.tm_mday = wtm.wDay;
    tm.tm_hour = wtm.wHour;
    tm.tm_min = wtm.wMinute;
    tm.tm_sec = wtm.wSecond;
    tm.tm_isdst = -1;

    clock = mktime(&tm);
    tp->tv_sec = clock;
    tp->tv_usec = wtm.wMilliseconds * 1000;
    return (0);
}
#endif
