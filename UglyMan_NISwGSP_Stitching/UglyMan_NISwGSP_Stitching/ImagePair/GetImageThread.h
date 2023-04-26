#ifndef GET_IMAGE_THREAD_H
#define GET_IMAGE_THREAD_H

#include "opencv2/core/core.hpp"
#include <opencv2/videoio.hpp>

#include "../Util/CommonDef.h"

#include "../siftgpu/util/util_threading.h"

class CGetImageThread : public colmap::Thread
{
public:
    int Init(int nMaxQueueSize);
    int SetJobQueue(colmap::JobQueue<cv::Mat> *pJobQue);
    int StopThread();



private:
    void Run() override;
    int PushData(cv::Mat& img);
    int SaveImage2Disk(cv::Mat& img);

private:
    std::shared_ptr<spdlog::logger> m_pLogger;
    cv::VideoCapture m_videoCapture;
    int m_nImageId;
    int m_nSkipFrameNum; //skip n frame
    bool m_bScanning;    // is scanning in progress

    colmap::JobQueue<cv::Mat> *m_pJobQue;  //for producer-consumer paradigm, this is producer
    int m_nQueueMinSize;  // reload image from disk if queue size less than m_nQueueMinSize 
    int m_nQueueMaxSize;  // Max Size if img in the queue
    std::string m_strSavePath; //save images to this path
    std::queue<std::string> m_queSavedImages;
   
};

#endif //GET_IMAGE_THREAD_H