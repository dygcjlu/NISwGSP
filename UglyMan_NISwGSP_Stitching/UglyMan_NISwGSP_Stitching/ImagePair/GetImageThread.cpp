
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include "../siftgpu/util/util_timer.h"
#include "GetImageThread.h"

CGetImageThread::CGetImageThread()
{
    m_pJobQue = nullptr;
    m_nImageId = 0;
    m_nNextFileIndex = 0;
    m_nSkipFrameNum = 0;
    m_bScanning = false;
    m_nQueueMaxSize = 20;  // 1920*1080*3=6MB per image.
    m_nQueueMinSize = 10;
    m_strSavePath = "./";
    m_nSrcImgType = -1;

    m_bIsIdle = true; 
    m_pLogger = GetLoggerHandle();



    SPDLOG_LOGGER_INFO(m_pLogger, "CGetImageThread::CGetImageThread...");
}


int CGetImageThread::Init(int nMaxQueueSize, int nSrcImgType, std::string strSrcFile, std::string strSavePath)
{
    m_nQueueMaxSize = nMaxQueueSize;//50;
    m_nQueueMinSize = m_nQueueMaxSize - 10; // 
    m_nQueueMinSize = m_nQueueMinSize > 0 ? m_nQueueMinSize : 0;
    m_strSavePath = strSavePath;

    if(m_strSavePath[m_strSavePath.size() - 1] != '/')
    {
        m_strSavePath = m_strSavePath + '/';
    }

    //std::string strFileName = strSrcFile;//"";
    m_nSrcImgType = nSrcImgType;

    if(SRC_TYPE_IMAGE_LIST == m_nSrcImgType)
    {
        //strSrcFile = /data/*.jpg
        cv::glob(strSrcFile, m_vecFileList, false);
        SPDLOG_LOGGER_INFO(m_pLogger, "Total Image Num:{}", m_vecFileList.size());
    
    }else if(SRC_TYPE_VIDEO_FILE == m_nSrcImgType)
    {
        m_videoCapture.open(strSrcFile);

        if (!m_videoCapture.isOpened()) 
        {
            //cerr << "ERROR! Unable to open camera\n";
            SPDLOG_LOGGER_ERROR(m_pLogger, "Failed to open video file:{}", strSrcFile);
            return -1;
        }

    }else if(SRC_TYPE_CAMERA == m_nSrcImgType)
    {
        m_videoCapture.open(strSrcFile);

        if (!m_videoCapture.isOpened()) 
        {
            //cerr << "ERROR! Unable to open camera\n";
            SPDLOG_LOGGER_ERROR(m_pLogger, "Failed to open camera:{}", strSrcFile);
            return -1;
        }

    }else{
        SPDLOG_LOGGER_ERROR(m_pLogger, "Unknown src image type:{}", nSrcImgType);
    }

    return 0;
}

int CGetImageThread::StopThread()
{
    m_pJobQue->Stop();
    Stop();

    return 0;
}

int CGetImageThread::SetJobQueue(colmap::JobQueue<cv::Mat> *pJobQue)
{
    m_pJobQue = pJobQue;
    return 0;
}

int CGetImageThread::SaveImage2Disk(cv::Mat& img)
{
    
    std::string strFileName = m_strSavePath + std::to_string(m_nImageId) + ".jpg";
    cv::imwrite(strFileName, img);
    m_queSavedImages.push(strFileName);

    return 0;
}

int CGetImageThread::PushData(cv::Mat& img)
{
    
    if(m_pJobQue->Size() < m_nQueueMaxSize)
    {
        if(m_queSavedImages.empty())
        {
            m_pJobQue->Push(img);
            SPDLOG_LOGGER_DEBUG(m_pLogger, "Insert one image to queue");
        }else if(m_pJobQue->Size() <= m_nQueueMinSize)
        {
            //m_queSavedImages not empty and  nQueueSize <= m_nQueueMinSize
            //reload image from disk

            colmap::Timer timer;
            timer.Start();
            
            int nCount = 0;
            while(true)
            {
                if(m_pJobQue->Size()>= m_nQueueMaxSize || m_queSavedImages.empty())
                {
                    break;
                }

                std::string strImageName = m_queSavedImages.front();
                m_queSavedImages.pop();
                cv::Mat image = cv::imread(strImageName);
                if(image.empty())
                {
                    SPDLOG_LOGGER_ERROR(m_pLogger, "Reload image from disk failed, file name:{}",strImageName);
                    break;
                }
                m_pJobQue->Push(image);
                nCount++;
            }
            //total tim elapse should less than 100ms
            double t = timer.ElapsedSeconds();
            SPDLOG_LOGGER_DEBUG(m_pLogger, "Load {} images, time elapsed:{}s", nCount, t);
        
        }else{
            SPDLOG_LOGGER_ERROR(m_pLogger, "PushData failed, job queue size:{}, max:{}, min:{}",
             m_pJobQue->Size(),m_nQueueMaxSize, m_nQueueMinSize);

        }

    }else{
        SaveImage2Disk(img);
        
    }


    return 0;
}

void CGetImageThread::StartScanning()
{
    m_bScanning = true;
}

void CGetImageThread::StopScanning()
{
    m_bScanning = false;
}

bool CGetImageThread::IsIdle()
{
    return m_bIsIdle;
}

bool CGetImageThread::GetNextFrame(cv::Mat& frame)
{

    switch(m_nSrcImgType)
    {
        case SRC_TYPE_IMAGE_LIST:
        {
            if(m_nNextFileIndex < m_vecFileList.size())
            {
                cv::String file = m_vecFileList[m_nNextFileIndex++];
                frame = cv::imread(file);

            }else{
                return false;
            }
            
            break;
        } 
        case SRC_TYPE_VIDEO_FILE:
        {
            m_videoCapture.read(frame);
            break;
        }
        case SRC_TYPE_CAMERA:
        {
            m_videoCapture.read(frame);
            break;
        }
        default:
        {
            SPDLOG_LOGGER_ERROR(m_pLogger, "Unkonwn src image type:{}", m_nSrcImgType);
            return false;
        }
    }

    if (frame.empty()) 
    {     
        return false;
    }
    return true;
}


void CGetImageThread::Run()
{
    SPDLOG_LOGGER_INFO(m_pLogger, "CGetImageThread run...");
    while(true)
    {
        m_bIsIdle = true;
        usleep(5*1000);//sleep 5ms
        
        if(IsStopped())
        {
            //quit this thread
            break;
        }
        cv::Mat frame;
        int nSkipFrameCount = 0;
        //m_nImageId = 0;

        
        while (m_bScanning)
        {
            if(m_bIsIdle)
            {
                m_bIsIdle = false;
            }

            if(!GetNextFrame(frame))
            {
                //SPDLOG_LOGGER_ERROR(m_pLogger, "Failed to get image");
                break;
            }
            
            //read image one by one
            //m_videoCapture.read(frame);
            // check if we succeeded
            
            m_nImageId++;

            //Skip n frame
            if(nSkipFrameCount < m_nSkipFrameNum)
            {
                nSkipFrameCount++;
                continue;
            }
            nSkipFrameCount = 0;

            //push image to queue or save to disk temporary
            PushData(frame);
        }
    }

    SPDLOG_LOGGER_WARN(m_pLogger, "CGetImageThread quit...");
}