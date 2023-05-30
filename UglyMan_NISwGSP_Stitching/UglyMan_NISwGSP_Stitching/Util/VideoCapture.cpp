#include <iostream>
#include <string>
#include "stdio.h"
#include "qcap.h"
#include "qcap.windef.h"
#include "qcap.linux.h"
//#include "utils.h"

#include "VideoCapture.h"


CVideoCapture* CVideoCapture::m_pInstance= nullptr;
/*
CVideoCapture *CVideoCapture::GetInstance()
{
    if(m_pInstance==nullptr){
        m_pInstance = new CVideoCapture();
    }
    
    return m_pInstance;
}*/

CVideoCapture::CVideoCapture()
{
    //file_logger = spdlog::get(strLogFile);
    file_logger = GetLoggerHandle();
    if(file_logger == nullptr)
    {
        std::cout<< "CVideoCapture get logger handle failed!" <<std::endl;
    }
    m_nInputType = 0;
    m_pDevice = nullptr;
	m_nVideoWidth = 0;
	m_nVideoHeight = 0;
	m_dVideoFrameRate = 0.0;
	m_pFramePool = nullptr;
    m_pRGBData = nullptr;
    SPDLOG_LOGGER_INFO(file_logger , "CVideoCapture::CVideoCapture");
}

CVideoCapture::~CVideoCapture()
{
    DestoryDevice();
    if(m_pRGBData != nullptr)
    {
        delete[] m_pRGBData;
    }

    SPDLOG_LOGGER_INFO(file_logger , "CVideoCapture::~CVideoCapture");
}

static QRETURN on_process_signal_removed( PVOID pDevice, ULONG nVideoInput, ULONG nAudioInput, PVOID pUserData )
{
    //printf("signal removed\n\n");
    //auto file_logger = spdlog::get(strLogFile);
    auto file_logger = GetLoggerHandle();
    if(file_logger == nullptr)
    {
        std::cout<< "CVideoCapture get logger handle failed!" <<std::endl;
    }

    SPDLOG_LOGGER_INFO(file_logger , "video signal removed");
    return QCAP_RT_OK;
}


static QRETURN on_process_format_changed( PVOID pDevice, ULONG nVideoInput, ULONG nAudioInput, 
                                          ULONG nVideoWidth, ULONG nVideoHeight, BOOL bVideoIsInterleaved, 
                                          double dVideoFrameRate, ULONG nAudioChannels, ULONG nAudioBitsPerSample, 
                                           ULONG nAudioSampleFrequency, PVOID pUserData )
{
    //auto file_logger = spdlog::get(strLogFile);
    auto file_logger = GetLoggerHandle();
    if(file_logger == nullptr)
    {
        std::cout<< "on_process_format_changed get logger handle failed!" <<std::endl;
    }
    SPDLOG_LOGGER_INFO(file_logger , "on_process_format_changed, nVideoInput:{},nVideoWidth:{}, nVideoHeight:{},dVideoFrameRate:{}", 
    nVideoInput,nVideoWidth, nVideoHeight,dVideoFrameRate);

    CVideoCapture::getInstance().SetVideoFormat(nVideoWidth, nVideoHeight, dVideoFrameRate);
   
    
	if( CVideoCapture::getInstance().GetFramePool() == nullptr )
    {
        PVOID tmpFramePool;
        QCAP_CREATE_VIDEO_FRAME_POOL(QCAP_COLORSPACE_TYPE_RGB24, nVideoWidth, nVideoWidth, 1, &tmpFramePool );
        CVideoCapture::getInstance().SetFramePool(tmpFramePool);
    }
    

    return QCAP_RT_OK;
}

static QRETURN on_process_video_preview( PVOID pDevice, double dSampleTime, BYTE * pFrameBuffer, ULONG nFrameBufferLen, PVOID pUserData )
{
   
    if( CVideoCapture::getInstance().GetFramePool() == nullptr )
    {
        //auto file_logger = spdlog::get(strLogFile);
        auto file_logger = GetLoggerHandle();
        if(file_logger == nullptr)
        {
            std::cout<< "on_process_video_preview get logger handle failed!" <<std::endl;
        }
        SPDLOG_LOGGER_ERROR(file_logger , "m_pFramePool() in NULL!");

        return QCAP_RT_OK;
    }

    BYTE* pDstFrameBuffer = nullptr;
    ULONG nDstFrameBufferLen = 0;
    PVOID pRCBuffer = nullptr;
    qcap_av_frame_t* pAVFrame = nullptr;

    ULONG nVideoWidth = CVideoCapture::getInstance().GetVideoWidth();
    ULONG nVideoHeight = CVideoCapture::getInstance().GetVideoHeight();
    ULONG nTargetImgWidth = CVideoCapture::getInstance().GetTargetImgWidth();
    ULONG nTargetImgHeight = CVideoCapture::getInstance().GetTargetImgHeight();

    QCAP_GET_FRAME_BUFFER(CVideoCapture::getInstance().GetFramePool(), &pDstFrameBuffer, &nDstFrameBufferLen);

    QCAP_COLORSPACE_YUY2_TO_RGB24(pFrameBuffer, nVideoWidth, nVideoHeight, nFrameBufferLen, pDstFrameBuffer, 
                                  nTargetImgWidth, nTargetImgHeight, nDstFrameBufferLen, 0, 0 );
                                  
    pRCBuffer = QCAP_BUFFER_GET_RCBUFFER(pDstFrameBuffer, nDstFrameBufferLen);
    pAVFrame = (qcap_av_frame_t*)QCAP_RCBUFFER_LOCK_DATA(pRCBuffer);

    //FILE* fp0 = fopen("frame.RGB", "a+");
    //printf("!!!!!! frame pool: %p  ,pitch:%d \n", pAVFrame->pData[0], pAVFrame->nPitch[0]);
    //fwrite(pAVFrame->pData[0], 1, pAVFrame->nPitch[0]* m_nVideoHeight, fp0);
    //fclose(fp0);
    static long long lastFrameTime =  getCurrentTimeMs();
    {
        CVideoCapture::getInstance().PasteRGBData(pAVFrame->pData[0], pAVFrame->nPitch[0]* nTargetImgHeight);
        static int nFrameCount = 0;
        if(nFrameCount++ % 1000 == 0)
        {
            long long curTime = getCurrentTimeMs();
            lastFrameTime = curTime;

            std::cout<< curTime - lastFrameTime<< "ms, pAVFrame width:"<<pAVFrame->nWidth << ",pAVFrame height:"<<pAVFrame->nHeight <<std::endl;
        }
    }
    
    QCAP_RCBUFFER_UNLOCK_DATA(pRCBuffer);

    return QCAP_RT_OK;
}



int CVideoCapture::CreateDevice(std::string strDeviceName, int nInputType, int nImgW, int nImgH)
{
    //std::string strDeviceName = "UB3300 USB"
    m_nInputType = nInputType;
    m_nTargetImgWidth = nImgW;
    m_nTargetImgHeight = nImgH;
    m_nRGBDataLen = m_nTargetImgHeight * m_nTargetImgWidth * 3;

	if(QCAP_RS_SUCCESSFUL != QCAP_CREATE((char*)strDeviceName.c_str(), 0, 0, &m_pDevice, 1, 0))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_CREATE failed!");
        return -1;
    }

    if(QCAP_RS_SUCCESSFUL != QCAP_REGISTER_SIGNAL_REMOVED_CALLBACK( m_pDevice, on_process_signal_removed, (PVOID)this ))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_REGISTER_SIGNAL_REMOVED_CALLBACK failed!");
        return -1;
    }

    if(QCAP_RS_SUCCESSFUL != QCAP_REGISTER_FORMAT_CHANGED_CALLBACK( m_pDevice, on_process_format_changed, (PVOID)this))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_REGISTER_FORMAT_CHANGED_CALLBACK failed!");
        return -1;
    }
	
    if(QCAP_RS_SUCCESSFUL != QCAP_REGISTER_VIDEO_PREVIEW_CALLBACK( m_pDevice, on_process_video_preview, (PVOID)this ))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_REGISTER_VIDEO_PREVIEW_CALLBACK failed!");
        return -1;
    }

    //int nInputType = QCAP_INPUT_TYPE_HDMI;
    if(QCAP_RS_SUCCESSFUL != QCAP_SET_VIDEO_INPUT(m_pDevice, nInputType))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_SET_VIDEO_INPUT failed!");
        return -1;
    }

    if(QCAP_RS_SUCCESSFUL != QCAP_RUN(m_pDevice))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_RUN failed!");
        return -1;
    }

    if(m_pRGBData == nullptr)
    {
        m_pRGBData = new unsigned char[m_nRGBDataLen];
    }

    return 0;
}

int CVideoCapture::DestoryDevice()
{
    if(QCAP_RS_SUCCESSFUL != QCAP_STOP(m_pDevice))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_STOP failed!");
        return -1;
    }
	
    if(QCAP_RS_SUCCESSFUL != QCAP_DESTROY(m_pDevice))
    {
        SPDLOG_LOGGER_ERROR(file_logger , "Call QCAP_DESTROY failed!");
        return -1;
    }

	m_pDevice = nullptr;
    return 0;
}

int CVideoCapture::SetVideoFormat(ULONG nVideoWidth, ULONG nVideoHeight, ULONG dVideoFrameRate)
{
    m_nVideoWidth = nVideoWidth;
    m_nVideoHeight = nVideoHeight;
    m_dVideoFrameRate = dVideoFrameRate;


    //m_nTargetImgWidth = m_nVideoWidth;
    //m_nTargetImgHeight = m_nVideoHeight;

    return 0;
}

 int CVideoCapture::PasteRGBData(unsigned char* pSrcData, int nDataLen)
 {
    if(nullptr == pSrcData || nullptr == m_pRGBData)
    {
        SPDLOG_LOGGER_ERROR(file_logger , "nullptr == pSrcData || nullptr == m_pRGBData");
        return -1;
    }

    if(nDataLen != m_nRGBDataLen)
    {
        SPDLOG_LOGGER_ERROR(file_logger , "nDataLen[{}] is not equal to m_nRGBDataLen[{}]!", nDataLen, m_nRGBDataLen);
        return -1;
    }

    {
        std::unique_lock<std::mutex> dataGuard(m_MutexRGBData);
        memcpy(m_pRGBData, pSrcData, nDataLen);
    }

    return 0;
 }

 int CVideoCapture::CopyRGBData(unsigned char* pDataBuffer, int nBufferLen, int* pDataLen)
 {
    if(nullptr == pDataBuffer || nullptr == m_pRGBData)
    {
        SPDLOG_LOGGER_ERROR(file_logger , "nullptr == pDataBuffer || nullptr == m_pRGBData");
        return -1;
    }


    if(nBufferLen < m_nRGBDataLen)
    {
        SPDLOG_LOGGER_ERROR(file_logger , "RGB buffer length[{}] less then RGB data lenth[{}]!", nBufferLen, m_nRGBDataLen);
        return -1;
    }

    {
        std::unique_lock<std::mutex> dataGuard(m_MutexRGBData);
        memcpy(pDataBuffer, m_pRGBData, m_nRGBDataLen);
        *pDataLen = m_nRGBDataLen;
    }

    return 0;
 }