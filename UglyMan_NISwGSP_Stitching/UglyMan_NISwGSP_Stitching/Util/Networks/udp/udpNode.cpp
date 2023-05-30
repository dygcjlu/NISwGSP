//-----------------------------------------------------------------------
#include "udpNode.h"

//////////////////////////////////////////////////////////////////////////
//
CUdpNode::CUdpNode()
{
    m_bTrans = true;
    m_nSocket = 0;
    memset(&m_mySockAddr, 0, sizeof(m_mySockAddr));
    memset(&m_dstSockAddr, 0, sizeof(m_dstSockAddr));
    memset(m_cachePoseData, 0, sizeof(m_cachePoseData));
    memset(&m_lastTime, 0, sizeof(m_lastTime));
    m_sendNetFrameHeader.frameFlag = 0x4875614A;
    m_sendNetFrameHeader.count = 0;
}

CUdpNode::~CUdpNode()
{
    Stop();
#if USE_POSIX_THREAD
    pthread_join(m_thNet2Dev,0);
    pthread_join(m_thDev2Net,0);
#else
    if(m_thNet2Dev.joinable())
    {
        m_thNet2Dev.join();
    }
    // if(m_thDev2Net.joinable())
    // {
    //     m_thDev2Net.join();
    // }
#endif
    // 关闭socket
    closeSocket(m_nSocket);
    // 卸载网络模块
    unloadNetModule();
}

// 初始化 监听端口
bool CUdpNode::Init(const char* pLocalIp, short nListenPort, uint64_t threadPeriod)
{
    //m_pRobotDevice = pRobotDev;
    m_threadPeriod = threadPeriod;
    // 加载网络模块
    loadNetModule();

    // 保存监听地址
    m_mySockAddr.sin_family = AF_INET;
    m_mySockAddr.sin_port = htons(nListenPort);
    if (strlen(pLocalIp) == 0) // 本地ip为空
    {
        SOCKET_S_ADDR(m_mySockAddr) = INADDR_ANY;
    }
    else
    {
        SOCKET_S_ADDR(m_mySockAddr) = inet_addr(pLocalIp);
    }

    // 创建udp socket
    m_nSocket = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);

    // 设置成非阻塞模式
    int ret = setNonBlock(m_nSocket);
    if (ret == SOCKET_ERROR)   // 设置失败
    {
        int err = getLaseError();
        printf("setsockopt() set FIONBIO failed[%d]. err=[%d]", ret, err);
        closeSocket(m_nSocket);
        return false;
    }

    //设置端口复用
    int32_t on = 1;
    if(setsockopt(m_nSocket, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on)) < 0)
    {
        //LOG_ERROR("Set port reuse error when init UDP client socket!\n");
        return false;
    }
    // 绑定本地端口
    ret = bind(m_nSocket, (SOCKADDR*)&m_mySockAddr, sizeof(m_mySockAddr));
    if (SOCKET_ERROR == ret)
    {
        int err = getLaseError();
        printf("bind is failed. err=[%d]", err);
        return false;
    }

    return true;
}

/*
开启两个线程：
NetToDevice
DeviceToNet
本线程：
从TcpNode中，根据控制消息，控制另外两个线程的状态
*/
void *NetToDeviceThreadFunc(void *arg)
{
    CUdpNode *pThis=static_cast<CUdpNode*>(arg);
    if(pThis!=nullptr)
    {
        pThis->NetToDeviceThreadProcess();
    }
    return nullptr;
}
// void *DeviceToNetThreadFunc(void *arg)
// {
//     CUdpNode *pThis=static_cast<CUdpNode*>(arg);
//     if(pThis!=nullptr)
//     {
//         pThis->DeviceToNetThreadProcess();
//     }
//     return nullptr;
// }

bool CUdpNode::Start()
{
    m_bRun = true;
#if USE_POSIX_THREAD
#if 0
    pthread_attr_t ThreadAttributes;
    int err = pthread_attr_init(&ThreadAttributes);
    if ( err )
    {
        printf("pthread attr_init() failed for thread '%s' with err=%d\n", TaskName, err );
        return -10;
    }
#ifdef __COBALT__
    err = pthread_attr_setinheritsched( &ThreadAttributes, PTHREAD_EXPLICIT_SCHED );
    if ( err )
    {
        printf("pthread set explicit sched failed for thread '%s' with err=%d\n", TaskName, err );
        return -11;
    }
#endif
    err = pthread_attr_setdetachstate(&ThreadAttributes, PTHREAD_CREATE_DETACHED /*PTHREAD_CREATE_JOINABLE*/);
    if ( err )
    {
        printf("pthread set detach state failed for thread '%s' with err=%d\n", TaskName, err );
        return -12;
    }
#if defined(__XENO__) || defined(__COBALT__)
    err = pthread_attr_setschedpolicy(&ThreadAttributes, SCHED_FIFO);
    if ( err )
    {
        printf("pthread set scheduling policy failed for thread '%s' with err=%d\n", TaskName, err );
        return -13;
    }
    struct sched_param paramA = { .sched_priority = Priority };
    err = pthread_attr_setschedparam(&ThreadAttributes, &paramA);
    if ( err )
    {
        printf("pthread set priority failed for thread '%s' with err=%d\n", TaskName, err );
        return -14;
    }
#endif
#endif
    pthread_create(&m_thNet2Dev, NULL, &NetToDeviceThreadFunc, this);
    pthread_create(&m_thDev2Net, NULL, &DeviceToNetThreadFunc, this);
#else
    m_thNet2Dev = std::thread(NetToDeviceThreadFunc,this);
    //m_thDev2Net = std::thread(DeviceToNetThreadFunc,this);
#endif
    printf("CUdpNode Start Finished\n");
    return true;
}

void CUdpNode::Stop()
{
    m_bTrans = false;
    m_bRun = false;
    printf("CUdpNode Stop Finished\n");
}

// 设置目标发送ip和端口
bool CUdpNode::SetTransfer(bool bTransFlag, const char *pDstIp, short nDstPort)
{
    if(nDstPort)
    {
        m_bTrans = false;
        m_dstSockAddr.sin_family = AF_INET;
        m_dstSockAddr.sin_port = htons(nDstPort);
        SOCKET_S_ADDR(m_dstSockAddr) = inet_addr(pDstIp);
    }
    m_bTrans = bTransFlag;
    return true;
}

// 接收网络Pose，进行设置
void CUdpNode::NetToDeviceThreadProcess()
{
    int nLen = sizeof(SOCKADDR_IN);
    char szBuffMsg[2048] = {0};

    unsigned maxFd=m_nSocket+1;
    fd_set rfd;
    timeval timeout;
    unsigned nextSeq=0;

    // RTIME costRts=0;
    // RTIME minCostRts=0xFFFFFFF;
    // RTIME maxCostRts=0;
    // RTIME avgCostRts=0;
    while (m_bRun)
    {
        if(!m_bTrans)
        {
            usleep(50000);//50ms
            continue;
        }
        // 超时时间设置为1妙  需要放到while内部，每次会被清理
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;

        FD_ZERO(&rfd); // 使用之前要清空
        FD_SET(m_nSocket, &rfd); // 把socket放入要测试的描述符集中
        int nRet = select(maxFd, &rfd, NULL, NULL, &timeout);
        if (nRet == SOCKET_ERROR) // socket 错误
        {
            printf("select 1\n");
            break;
        }
        else if (nRet == 0) // 超时
        {
            continue;
        }
        else // 检测到有套接字可读
        {
            //RTIME startRts=rt_timer_read();
            //if (FD_ISSET(m_nSocket, &rfd)) // 确认可读 -- only one
            {
                //printf("select 4\n");
                //memset(szBuffMsg, 0, 2048);
                nRet = recvfrom(m_nSocket, szBuffMsg, sizeof(szBuffMsg), 0, (SOCKADDR*)&m_dstSockAddr, (socklen_t*)&nLen);
                if (nRet == INVALID_SOCKET)
                {
                    printf("select 5\n");
                    continue;
                }
                else
                {
                    unsigned remainLen=nRet;
                    char *pData=szBuffMsg;
                    while(remainLen>sizeof(HjNetFrameHeader))
                    {
                        HjNetFrameHeader netFrameHeader;
                        int MsgPoseDataLen = 13 * sizeof(double);
                        memcpy(&netFrameHeader,pData,sizeof(HjNetFrameHeader));
                        if(netFrameHeader.frameFlag == 0x4A617548 &&
                            netFrameHeader.msgType == EMMsgAllPoseData &&
                            netFrameHeader.msgLen == MsgPoseDataLen)
                        {
                            //1x3坐标  3x3方向  1个夹钳角度  double
                           
                            memcpy(m_cachePoseData,pData+sizeof(HjNetFrameHeader),MsgPoseDataLen);
                            if(nextSeq!=netFrameHeader.count)
                            {
                                printf("Error seq not equal %u VS %u\n",nextSeq,netFrameHeader.count);
                                nextSeq=netFrameHeader.count+1;
                            }
                            else
                            {
                                nextSeq++;
                            }
                            // if(netFrameHeader.count%1000==-1)
                            // {
                            //     timeval nowTv;
                            //     getNowTv(nowTv);
                            //     static timeval lastTs=nowTv;
                            //     printf("NetToDevice count:%u recvTs:%ld.%06ld nowTs:%ld.%06ld diffNow:%ld ms, last costRts:%llu (%llu %llu %llu)\n",
                            //            netFrameHeader.count,netFrameHeader.timeStamp.tv_sec,netFrameHeader.timeStamp.tv_usec,nowTv.tv_sec,nowTv.tv_usec,
                            //            (nowTv.tv_sec-lastTs.tv_sec)*1000+(nowTv.tv_usec-lastTs.tv_usec)/1000,costRts,minCostRts,maxCostRts,avgCostRts);
                            //     lastTs=nowTv;
                            //     if(m_cachePoseData[0]>0.000001||m_cachePoseData[0]<-0.000001)
                            //     {
                            //         for(int i=0;i<13;i++)
                            //         {
                            //             printf("%9.6f ",m_cachePoseData[i]);
                            //             if(i%3==2)
                            //             {
                            //                 printf("\n");
                            //             }
                            //         }
                            //         printf("\n");
                            //     }
                            // }
                            //m_pRobotDevice->SetAllPose(m_cachePoseData);
                            //姿态数据后面再确定

                            remainLen = remainLen - (sizeof(HjNetFrameHeader)+MsgPoseDataLen);
                            pData = szBuffMsg + (nRet - remainLen);
                            // RTIME endRts=rt_timer_read();
                            // costRts = endRts-startRts;

                            // if(avgCostRts!=0)
                            // {
                            //     avgCostRts = (avgCostRts + costRts)/2;
                            // }
                            // else
                            // {
                            //     avgCostRts = costRts;
                            // }
                            // if(costRts<minCostRts)
                            // {
                            //     minCostRts = costRts;
                            // }
                            // if(costRts>maxCostRts)
                            // {
                            //     printf("NetToDevice maxCostRts change from %llu to %llu\n",maxCostRts,costRts);
                            //     maxCostRts = costRts;
                            // }
                        }
                        else
                        {
                            printf("msgError msgLen = %u\n", netFrameHeader.msgLen);
                            break;
                        }
                    }

                    if(remainLen)
                    {
                        //TOUPDATE 理论上需要缓存剩余字节，目前不处理
                        printf("msgError readLen=%d, remainLen=%u\n", nRet,remainLen);
                    }
                }
            }
        }
    }
}

#if 0
//获得Pose，网络发出
void CUdpNode::DeviceToNetThreadProcess()
{
    double poseArr[PoseDoubleSize];
    char sendBuf[256];

    HjNetFrameHeader netFrameHeader;
    netFrameHeader.frameFlag = 0x4A617548;
    netFrameHeader.count = 0;
    netFrameHeader.msgType = EMMsgAllPoseData;
    netFrameHeader.msgLen = MsgPoseDataLen;
    timespec wakeupTime;
    clock_gettime(CLOCK_MONOTONIC,&wakeupTime);

    while (m_bRun)
    {
        TIMESPEC_ADDNS(wakeupTime,m_threadPeriod);//可能一段时间后需要调用clock_gettime修正时间
        clock_nanosleep(CLOCK_MONOTONIC,TIMER_ABSTIME,&wakeupTime,nullptr);

        if(!m_bTrans)
        {
            continue;
        }

        m_pRobotDevice->GetAllPose(poseArr);

        netFrameHeader.count++;
        getNowTv(netFrameHeader.timeStamp);
        if(netFrameHeader.count%4000==-1)
        {
            static timeval lastTs;
            printf("DeviceToNet count:%u now:%ld.%06ld diff:%ld ms\n",netFrameHeader.count,netFrameHeader.timeStamp.tv_sec,
                   netFrameHeader.timeStamp.tv_usec,(netFrameHeader.timeStamp.tv_sec-lastTs.tv_sec)*1000+(netFrameHeader.timeStamp.tv_usec-lastTs.tv_usec)/1000);
            lastTs = netFrameHeader.timeStamp;
            if(poseArr[0]>0.000001||poseArr[0]<-0.000001)
            {
                for(int i=0;i<13;i++)
                {
                    printf("%9.6f ",poseArr[i]);
                    if(i%3==2)
                    {
                        printf("\n");
                    }
                }
                printf("\n");
            }
        }
        memcpy(sendBuf,&netFrameHeader,sizeof(HjNetFrameHeader));
        memcpy(sendBuf+sizeof(HjNetFrameHeader),poseArr,MsgPoseDataLen);
        int nRet = sendto(m_nSocket, sendBuf, sizeof(HjNetFrameHeader)+MsgPoseDataLen,
                          0, (SOCKADDR*)&m_dstSockAddr, sizeof(m_dstSockAddr));
        if (nRet == -1)
        {
            printf("sendto errno = %d\n", getLaseError());
        }

    }
}
#endif