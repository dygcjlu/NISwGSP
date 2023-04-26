#ifndef FEATURE_MATCH_THREAD_H
#define FEATURE_MATCH_THREAD_H

#include "opencv2/core/core.hpp"
#include "../siftgpu/util/util_threading.h"
#include "../Util/CommonDef.h"
#include "../Feature/ImageData.h"

class CFeatureMatchThread : public colmap::Thread
{
public:
    CFeatureMatchThread();
    int Init();
    int StopThread();
    int SetJobQueue(colmap::JobQueue<cv::Mat> *pJobQue);

    std::map<int, ImageData*> GetKeyFrame();
    std::vector<pair<int, int>> GetImagePairs();


private:
    void Run() override;
    int LoopFeatureMatch(ImageData* pImage,  bool bIsKeyFrame);
    double ComputeOverlapArea(ImageData* pImage1, colmap::FeatureDescriptors& descriptors1, const vector<Point2> & m1_fpts, 
                                                ImageData* pImage2);
    void SaveImage2Disk(int nId, cv::Mat& img);


private:
    static int m_nKeyFrameId;
    int m_nFeaturePointSizeThreshold;
    int m_nMatchSizeThreshold;
    int m_nLoopDetectInterval;
    double m_dMinOverlapRatio;
    double m_dMaxOverlapRatio;
    int m_nLoopDetectStartIndex;
    std::string m_strSavePath;

    std::shared_ptr<spdlog::logger> m_pLogger;
    colmap::JobQueue<cv::Mat> *m_pOriginImgQue;

    std::mutex m_mutex;
    std::map<int, ImageData*> m_mapKeyFrame; 
    std::vector<pair<int, int>> m_vecPairs; //matched images, first should be larger than second
    //std::map<int, std::map< >>

   
    

};

#endif //FEATURE_MATCH_THREAD_H