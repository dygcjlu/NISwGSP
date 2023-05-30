#ifndef COMMON_DEFINE_H
#define COMMON_DEFINE_H

#include <thread>
#include <string.h>

#ifndef NSEC_PER_SEC
#define FREQUENCY 1000
#define NSEC_PER_SEC (1000000000L)
#define PERIOD_NS (NSEC_PER_SEC / FREQUENCY)
#endif

#define TIMESPEC_ADDNS(A,B) \
    (A).tv_nsec+=B;\
    if((A).tv_nsec>=NSEC_PER_SEC){\
        (A).tv_sec+=1;(A).tv_nsec-=NSEC_PER_SEC;}

#ifndef SAFE_DELETE(p)
#define SAFE_DELETE(p) if(p) {delete p; p = nullptr;};
#endif

enum EMServerType
{
    EMServerUnknown = 0,
    EMServerMain = 1,
    EMServerGraphic,
    EMServerMasterLeft,
    EMServerMasterRight,
    EMServerSlaveLeft,
    EMServerSlaveRight,
    EMServerSlaveCamera
};

enum EMMasterType
{
    EMMasterUnknown = 0,
    EMMasterOmega = 1,
    EMMasterTouch,
    EMMasterHuaJ
};

enum EMMsgType
{
    EMMsgUnknown = 0,
    EMMsgAllPoseData = 1 //4x4矩阵
};

typedef struct _HjNetFrameHeader
{
    unsigned frameFlag;//网络序HuaJ 0x4875614A ==>主机序JauH 0x4A617548
    unsigned count;
    timeval  timeStamp;
    unsigned short msgType;//EMMsgType
    unsigned short msgLen;
    //unsigned char * msg
}HjNetFrameHeader;


typedef signed char         INT8, *PINT8;
typedef signed short        INT16, *PINT16;
typedef signed int          INT32, *PINT32;
typedef signed long long    INT64, *PINT64;
typedef unsigned char       UINT8, *PUINT8;
typedef unsigned short      UINT16, *PUINT16;
typedef unsigned int        UINT32, *PUINT32;
typedef unsigned long long  UINT64, *PUINT64;
typedef unsigned char       BYTE;
typedef long                LONG;
typedef unsigned long       ULONG;
typedef short               SHORT;

#ifndef M_PI
#define M_PI		3.14159265359
#endif


//定义串口波特率
#define  SBR_0	     0
#define  SBR_50	     50
#define  SBR_75	     75
#define  SBR_110	 110
#define  SBR_134	 134
#define  SBR_150	 150
#define  SBR_200	 200
#define  SBR_300	 300
#define  SBR_600	 600
#define  SBR_1200	 1200
#define  SBR_1800	 1800
#define  SBR_2400	 2400
#define  SBR_4800	 4800
#define  SBR_9600	 9600
#define  SBR_19200	 19200
#define  SBR_38400	 38400

#define  SBR_OTHER   0010000
#define  SBR_57600   57600
#define  SBR_115200  115200
#define  SBR_230400  230400
#define  SBR_460800  460800
#define  SBR_500000  500000
#define  SBR_576000  576000
#define  SBR_921600  921600
#define  SBR_1000000 1000000
#define  SBR_1152000 1152000
#define  SBR_1500000 1500000
#define  SBR_2000000 2000000
#define  SBR_2500000 2500000
#define  SBR_3000000 3000000
#define  SBR_3500000 3500000
#define  SBR_4000000 4000000

#define SEV_RXCHAR           0x0001  // Any Character received
#define SEV_RXFLAG           0x0002  // Received certain character
#define SEV_TXEMPTY          0x0004  // Transmitt Queue Empty
#define SEV_CTS              0x0008  // CTS changed state
#define SEV_DSR              0x0010  // DSR changed state
#define SEV_RLSD             0x0020  // RLSD changed state
#define SEV_BREAK            0x0040  // BREAK received
#define SEV_ERR              0x0080  // Line status error occurred
#define SEV_RING             0x0100  // Ring signal detected
#define SEV_PERR             0x0200  // Printer error occured
#define SEV_RX80FULL         0x0400  // Receive buffer is 80 percent full
#define SEV_EVENT1           0x0800  // Provider specific event 1
#define SEV_EVENT2           0x1000  // Provider specific event 2

#define HJ_CRC_LENGTH        256

#define NFC_DATA_SIZE 20
#define OP_ACK_SIZE 20
#define OP_ACK_PARAM_SIZE 20

#define OP_RECV_SIZE 20
#define OP_ARG_RECV_SIZE 20

typedef struct  SerialPortParam_Stru
{
    char        m_PortName[16];           //端口名
    INT32       m_BaudRate;               //波特率
    INT32       m_DataBit;                //数据位的个数,默认值为8个数据位
    char        m_Parity;                 //奇偶校验
    INT32       m_StopBit;                //停止位
    INT64       m_CommEvents;             //通信时间

    SerialPortParam_Stru()
    {
        memset(m_PortName, 0, sizeof(m_PortName));
        m_BaudRate = 9600;
        m_DataBit = 8;
        m_Parity = 'N';
        m_StopBit = 1;
        m_CommEvents = SEV_RXCHAR;
    }

    SerialPortParam_Stru(const SerialPortParam_Stru &other)
    {
        memcpy(m_PortName, other.m_PortName, sizeof(m_PortName));
        m_BaudRate = other.m_BaudRate;
        m_DataBit = other.m_DataBit;
        m_Parity = other.m_Parity;
        m_StopBit = other.m_StopBit;
        m_CommEvents = other.m_CommEvents;
    }

    SerialPortParam_Stru &operator= (const SerialPortParam_Stru &other)
    {
        if (this == &other)
        {
            return *this;
        }

        memcpy(m_PortName, other.m_PortName, sizeof(m_PortName));
        m_BaudRate = other.m_BaudRate;
        m_DataBit = other.m_DataBit;
        m_Parity = other.m_Parity;
        m_StopBit = other.m_StopBit;
        m_CommEvents = other.m_CommEvents;

        return *this;
    }
}SerialPortParam;

typedef struct stuMastHandRecvData
{
    int key1;       //主手传感器 0:靠进, 非0：未靠进
    int key2;       //主手离合开关 0:打开, 非0：未打开
    int code_cic;   //霍尔传感器输出未使用
    int code_out;   //霍尔传感器输出结果
    double angle;   //夹钳角度
}MastHandRecvData;


struct rgbsData
{
    int style;
    int freq;
};

typedef struct stuCameraSendData
{
    int cmd;
    int fan;
    struct rgbsData rgbs[2];
    char nfc[NFC_DATA_SIZE];
}CameraSendData;

typedef struct stuCameraRecvData
{
    int cmd;
    int hall1;
    int hall2;
    int key1;
    int key2;
    char nfc[NFC_DATA_SIZE];
}CameraRecvData;

struct beepData
{
    int en;
    int cnt;
};

typedef struct stuPatientSendData
{
    int cmd;
    int fan;
    struct beepData beep;
    char opt_ack[OP_ACK_SIZE];
    char opt_ack_param[OP_ACK_PARAM_SIZE];
}PatientSendData;

struct keyData
{
    int dray;
    int start;
};

struct forceData
{
    int x;
    int y;
    int z;
    int r;
};

typedef struct stuPatientRecvData
{
    int cmd;
    int emg;
    struct keyData key;
    struct forceData force;
    char opt[OP_ACK_SIZE];
    char opt_param[OP_ACK_PARAM_SIZE];
}PatientRecvData;

typedef struct stuDoctorSendData
{
    int cmd;
    char opt_ack[OP_ACK_SIZE];
    char opt_ack_param[OP_ACK_PARAM_SIZE];
}DoctorSendData;

typedef struct stuDoctorRecvData
{
    int cmd;
    int err;
    int emg;
    int eng[6];
    int step[6];
    int sensor[2];
    char opt[OP_RECV_SIZE];
    char opt_arg[OP_ARG_RECV_SIZE];
}DoctorRecvData;


void getNowTv(timeval& tv);
#endif // COMMON_DEFINE_H
