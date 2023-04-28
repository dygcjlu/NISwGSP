#ifndef GET_IMAGE_THREAD_H
#define GET_IMAGE_THREAD_H
#include <atomic>
#include "opencv2/core/core.hpp"
#include <opencv2/videoio.hpp>

#include "../Util/CommonDef.h"

#include "../siftgpu/util/util_threading.h"


enum enumSrcImgType 
{ 
    SRC_TYPE_IMAGE_LIST = 0, 
    SRC_TYPE_VIDEO_FILE, 
    SRC_TYPE_CAMERA
};


class CGetImageThread : public colmap::Thread
{
public:
    CGetImageThread();
    int Init(int nMaxQueueSize, int nSrcImgType, std::string strSrcFile, std::string strSavePath);
    int StopThread();
    int SetJobQueue(colmap::JobQueue<cv::Mat> *pJobQue);
    void SetStartEndSecond(int nStart, int nEnd);
    

    void StartScanning();
    void StopScanning();
    bool IsIdle();

    colmap::JobQueue<cv::Mat> * GetJobQueuePoiner(){return m_pJobQue;}


private:
    void Run() override;
    int PushData(cv::Mat& img);
    int SaveImage2Disk(cv::Mat& img);
    bool GetNextFrame(cv::Mat& frame);
    bool HaveMoreImg();

private:
    std::shared_ptr<spdlog::logger> m_pLogger;
    cv::VideoCapture m_videoCapture;

    //for file list
    std::vector<cv::String> m_vecFileList;
    int m_nNextFileIndex;

    //for video file
    int m_nStartSecond;
    int m_nEndSecond;
    int m_nCurrentSecond;
    colmap::Timer m_timer;
    double m_dLastFrameTime;



    int m_nImageId;
    int m_nSkipFrameNum; //skip n frame
    int m_nSrcImgType;
    std::atomic_bool m_bScanning;    // is scanning in progress
    std::atomic_bool m_bIsIdle; //means already done sacnning, no more image 

    colmap::JobQueue<cv::Mat> *m_pJobQue;  //for producer-consumer paradigm, this is producer
    int m_nQueueMinSize;  // reload image from disk if queue size less than m_nQueueMinSize 
    int m_nQueueMaxSize;  // Max Size if img in the queue
    std::string m_strSavePath; //save images to this path
    std::queue<std::string> m_queSavedImages;
   
};

#endif //GET_IMAGE_THREAD_H