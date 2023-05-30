//------------------------------------------------------------------------
#ifndef __HJ_UDP_NODE_H
#define __HJ_UDP_NODE_H

#include <map>
#include "netCommon.h"
/*
typedef struct _HjNetFrameHeader
{
    unsigned frameFlag;//网络序HuaJ 0x4875614A ==>主机序JauH 0x4A617548
    unsigned count;
    timeval  timeStamp;
    unsigned short msgType;//EMMsgType
    unsigned short msgLen;
    //unsigned char * msg
}HjNetFrameHeader;
*/
#define USE_POSIX_THREAD 0 
#define PoseDoubleSize 13
class CUdpNode
{
public:
    CUdpNode();
    ~CUdpNode();

    // 初始化 监听端口
    bool Init(const char *pLocalIp, short nListenPort, uint64_t threadPeriod);
    bool Start();
    void Stop();
    // 设置目标发送ip和端口
    bool SetTransfer(bool bTransFlag, const char *pDstIp, short nDstPort);

    //接收网络Pose，正解到关节角，转换为编码器值, 写共享内存
    void NetToDeviceThreadProcess();
    ////读共享内存，转换为关节角，逆解为Pose，网络发出
    //void DeviceToNetThreadProcess();

private:
    bool m_bRun;      //线程运行状态
    bool m_bTrans;                                           // 是否发送数据
    unsigned int m_nSocket;                                       // 创建的socket
    struct sockaddr_in m_mySockAddr;                        // 本地地址记录
    struct sockaddr_in m_dstSockAddr;                       // 目标地址记录
    struct timeval m_lastTime;                              // 上次获取数据时间
    double m_cachePoseData[PoseDoubleSize];  //3x1 坐标点+  3x3方向+  1个夹钳
    uint64_t m_threadPeriod;
#if USE_POSIX_THREAD
    pthread_t m_thNet2Dev;
    pthread_t m_thDev2Net;
#else
    std::thread m_thNet2Dev;
    //std::thread m_thDev2Net;
#endif
    char m_sendBuf[1024];
    HjNetFrameHeader m_sendNetFrameHeader;
};


#endif  // __HJ_UDP_NODE_H
