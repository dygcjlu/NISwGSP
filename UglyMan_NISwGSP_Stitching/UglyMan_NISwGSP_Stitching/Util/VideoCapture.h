#ifndef YOLACT_VIDEO_CAPTURE_H
#define YOLACT_VIDEO_CAPTURE_H

#include <mutex>
#include "CommonDef.h"

typedef void * PVOID;
typedef unsigned long ULONG;

class CVideoCapture
{
public:
    //static CVideoCapture *GetInstance();
    static CVideoCapture& getInstance()
    {
        static CVideoCapture inst;
        return inst;
    }
    
    ~CVideoCapture();
    CVideoCapture(CVideoCapture &other) = delete;
    void operator=(const CVideoCapture &) = delete;

public:
    //Initialize video input 
    int CreateDevice(std::string strDeviceName, int nInputType, int nImgW, int nImgH);
    //Release video device 
    int DestoryDevice();

    int PasteRGBData(unsigned char* pSrcData, int nDataLen);
    int CopyRGBData(unsigned char* pDataBuffer, int nBufferLen, int* pDataLen);

    //Set video format info
    int SetVideoFormat(ULONG nVideoWidth, ULONG nVideoHeight, ULONG dVideoFrameRate);

    PVOID GetFramePool(){return m_pFramePool;}
    void SetFramePool(PVOID pFramePool){m_pFramePool = pFramePool;}

    ULONG GetVideoWidth(){return m_nVideoWidth;}
    ULONG GetVideoHeight(){return m_nVideoHeight;}
    ULONG GetTargetImgWidth(){return m_nTargetImgWidth;}
    ULONG GetTargetImgHeight(){return m_nTargetImgHeight;}
    unsigned char* GetRGBPointer(){return m_pRGBData;}

private:
    CVideoCapture();
    static CVideoCapture* m_pInstance;

private:
    //logger
    std::shared_ptr<spdlog::logger> file_logger;

    int m_nInputType;
    //video input devie handle
    PVOID m_pDevice;
 
    //Original width of YUV
    ULONG m_nVideoWidth;
    //Original height of YUV
    ULONG m_nVideoHeight;
    double m_dVideoFrameRate;

    //RGB image width
    ULONG m_nTargetImgWidth;
    //RGB image height
    ULONG m_nTargetImgHeight;
    unsigned char* m_pRGBData;
    int m_nRGBDataLen;
    std::mutex m_MutexRGBData;

    PVOID m_pFramePool;
};



#endif