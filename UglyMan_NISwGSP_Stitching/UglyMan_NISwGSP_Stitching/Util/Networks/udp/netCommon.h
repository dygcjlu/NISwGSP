#ifndef __HJ_COMMON_H
#define __HJ_COMMON_H

#include <time.h>
#include "common.h"

#if defined(WIN32) || defined(WIN64)
#include <WinSock2.h> 
#include <WS2tcpip.h>
#include <ws2ipdef.h>
#else
#include <sys/socket.h>
#include <sys/select.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/time.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <ifaddrs.h>
#include <errno.h>
#include <string.h>
#endif

enum GpReturnCode
{
    ReturnCode_Error = -1,
    ReturnCode_Success = 0,
};

#if defined(WIN32) || defined(WIN64)
// 网络库初始化,windows需要手动加载/卸载网络模块
extern bool g_bWSAStartup;
#pragma warning(disable:4996)
#pragma comment(lib, "ws2_32.lib")
#else

// 在linux平台对Socket模块做WinSock2风格的别名，适配WinSock2风格的源码
typedef unsigned int        SOCKET;
typedef sockaddr_in         SOCKADDR_IN;
typedef sockaddr            SOCKADDR;

#ifndef INVALID_SOCKET
#define INVALID_SOCKET  (SOCKET)(-1)
#endif

#ifndef SOCKET_ERROR
#define SOCKET_ERROR            (-1)
#endif

#endif

#ifndef SOCKET_S_ADDR
#if defined(WIN32) || defined(WIN64)
#define SOCKET_S_ADDR(s) s.sin_addr.S_un.S_addr
#else
#define SOCKET_S_ADDR(s) s.sin_addr.s_addr
#endif
#endif // !S_ADDR_SYS

#ifndef SOCKET_LEN_T
#if defined(WIN32) || defined(WIN64)
#define SOCKET_LEN_T INT32
#else
#define SOCKET_LEN_T socklen_t
#endif
#endif // !SOCKET_LEN_T

//////////////////////////////////////////////////////////////////////////
//
/*
* 跨平台-网络相关处理
*/

// 初始化网络模块
int loadNetModule();

// 卸载网络模块
int unloadNetModule();

// 将socket设置为非阻塞模式
INT32 setNonBlock(SOCKET socket);

// 关闭socket
INT32 closeSocket(SOCKET socket);

// 设置套接字超时时间
//INT32 gpSetSocketTimeout(SOCKET socket,INT32 timeout);

// linux需要适配WSA风格的错误码
#if defined(WIN32) || defined(WIN64)
#elif defined(__linux__) || defined(__APPLE__)
#define WSAETIMEDOUT    ETIMEDOUT
#define WSAEINTR        EINTR
#define WSAEWOULDBLOCK  EWOULDBLOCK
#endif


#ifdef WIN32
int gettimeofday(struct timeval *tp, void *tzp);
#endif

// 获取最近一次的错误码
INT32 getLaseError();

#endif // !__HJ_COMMON_H
