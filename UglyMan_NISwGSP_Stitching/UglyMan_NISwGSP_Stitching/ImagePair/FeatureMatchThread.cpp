#include <opencv2/opencv.hpp>
#include "FeatureMatchThread.h"
#include "../siftgpu/siftgpu_manage.h"
#include "../Util/Transform.h"


int CFeatureMatchThread::m_nKeyFrameId = 0;
CFeatureMatchThread::CFeatureMatchThread()
{
    m_pOriginImgQue = nullptr;
    m_nFeaturePointSizeThreshold = 100;
    m_nMatchSizeThreshold = 20;
    m_nLoopDetectInterval = 2;

    m_dMinOverlapRatio = 0.1;
    m_dMaxOverlapRatio = 0.9;
    m_nLoopDetectStartIndex = 5;

    m_pLogger = GetLoggerHandle();
    m_strSavePath = './';



    SPDLOG_LOGGER_INFO(m_pLogger, "CFeatureMatchThread::CFeatureMatchThread...");

}


int CFeatureMatchThread::Init(std::string strSavePath)
{
    m_strSavePath = strSavePath;
    if(m_strSavePath[m_strSavePath.size() - 1] != '/')
    {
        m_strSavePath = m_strSavePath + '/';
    }

    SPDLOG_LOGGER_INFO(m_pLogger, "save path:{}", m_strSavePath);

    return 0;
}

int CFeatureMatchThread::StopThread()
{
    m_pOriginImgQue->Stop();
    Stop();

    return 0;
}

int CFeatureMatchThread::SetJobQueue(colmap::JobQueue<cv::Mat> *pJobQue)
{
    m_pOriginImgQue = pJobQue;
    return 0;
}

//image1 project to image2
double  CFeatureMatchThread::ComputeOverlapArea(ImageData* pImage1, colmap::FeatureDescriptors& descriptors1, const vector<Point2> & m1_fpts, 
                                                ImageData* pImage2)
{
    //need image2 project to image1?

    colmap::FeatureDescriptors & descriptors2 = pImage2->getFeatureDescriptorsSiftgpu();
    colmap::FeatureMatches matches;
    vector<pair<int, int> > initial_indices;

    CSiftgpuManage::getInstance().FeatureMatch(matches, descriptors1, descriptors2);
    if(matches.size() < m_nMatchSizeThreshold)
    {
        return 0;
    }

    for(int i = 0; i < matches.size(); i++)
    {
        initial_indices.emplace_back(matches[i].point2D_idx1, matches[i].point2D_idx2);
    }


    const vector<Point2> & m2_fpts = pImage2->getFeaturePoints();
    vector<Point2> X, Y;
    X.reserve(initial_indices.size());
    Y.reserve(initial_indices.size());
    for(int j = 0; j < initial_indices.size(); ++j) 
    {
        const pair<int, int> it = initial_indices[j];
        X.emplace_back(m1_fpts[it.first ]);
        Y.emplace_back(m2_fpts[it.second]);
    }
    cv::Mat H = cv::findHomography(X, Y, CV_RANSAC, 5);
        

    Point2f obj_corners[4] = { cv::Point(0,0),cv::Point(pImage1->img.cols, 0), cv::Point(pImage1->img.cols, 
    pImage1->img.rows), cv::Point(0, pImage1->img.rows) };
    //Point scene_corners2[4];
    vector<Point> pts;

    for (int i = 0; i < 4; i++)
    {
        double x = obj_corners[i].x;
        double y = obj_corners[i].y;
        Point2 p = applyTransform3x3(x, y, H);

        pts.push_back(cv::Point(cvRound(p.x), cvRound(p.y)));
    }
    for(int i = 0; i < pts.size(); ++i)
    {
        if(pts[i].x < 0)
        {
            pts[i].x = 0;
        }
        if(pts[i].y < 0)
        {
            pts[i].y = 0;
        }
        if(pts[i].x > (pImage2->img.cols - 1))
        {
            pts[i].x = pImage2->img.cols - 1;
        }
        if(pts[i].y > (pImage2->img.rows - 1))
        {
            pts[i].y = pImage2->img.rows - 1;
        }
    }
    

    Mat roi = Mat::zeros(pImage2->img.size(), CV_8UC1);
    vector<vector<Point>> contour;
    contour.push_back(pts);

    drawContours(roi, contour, 0, Scalar::all(255), -1);

#ifndef NDEBUG
    static int nOverlapNo = 1000;

    SaveImage2Disk(nOverlapNo++, roi);
#endif
    
    double area = cv::contourArea(contour[0]);
    double dRatio = area / (pImage2->img.rows * pImage2->img.cols);
    return dRatio;
    /*

   */


    return 0;

}

int CFeatureMatchThread::LoopFeatureMatch(ImageData* pImage,  bool& bIsKeyFrame)
{
    colmap::FeatureDescriptors & descriptors1 = pImage->getFeatureDescriptorsSiftgpu();
    const vector<Point2> & m1_fpts = pImage->getFeaturePoints();
    int nIteratorNo = 0;
    int nSkipKf = 0;
    double dOverlapRatioWithLastKF = 0;
    int nLastKFId = -1;
    double dMaxOverlapRatio = 0.;
    int nMaxOverlapIndex = -1;
    int nMaxOverlapId = -1;

    std::map<int, ImageData*>::reverse_iterator rit;
    vector<pair<int, int>> vecPairs;
    //bool bIsKeyFrame = false;
    bool bLoopDetect = false;
   
    for (rit=m_mapKeyFrame.rbegin(); rit!=m_mapKeyFrame.rend(); ++rit)
    {
        nIteratorNo++;
        
        if(nIteratorNo <= m_nLoopDetectStartIndex && nIteratorNo > 2)
        {
            continue;
        }

        if(nSkipKf < m_nLoopDetectInterval && nIteratorNo > m_nLoopDetectStartIndex)
        {
            nSkipKf++;
            continue;
        }
        nSkipKf = 0;

        double dOverlapRatio = ComputeOverlapArea(pImage, descriptors1, m1_fpts, rit->second);
        if(dOverlapRatio < m_dMinOverlapRatio || dOverlapRatio > m_dMaxOverlapRatio)
        {
            continue;
        }

        //add connection to list
        pImage->SetConnection(rit->first);
        rit->second->SetConnection(pImage->m_nID);


        if(1 == nIteratorNo)
        {     
            dOverlapRatioWithLastKF = dOverlapRatio;
            nLastKFId = rit->second->m_nID;
            if(m_mapKeyFrame.size() == 1) //only one key frame in map
            {
                bIsKeyFrame = true;
                //vecPairs.push_back( std::pair<pImage->m_nID, rit->second->m_nID>);
                vecPairs.emplace_back(std::make_pair(pImage->m_nID, rit->second->m_nID));
                SPDLOG_LOGGER_INFO(m_pLogger, "Add first pair<{},{}>", pImage->m_nID, rit->second->m_nID);
            }
            
        }else if(2 == nIteratorNo)
        {
            //若与前一张的重叠度大于与前前一张的重叠度(否则认为走回头路了)，则此图片做为候选关键帧
            if(dOverlapRatio > dOverlapRatioWithLastKF )
            {
                SPDLOG_LOGGER_INFO(m_pLogger, "dOverlapRatio{} > dOverlapRatioWithLastKF{},ignore this frame", dOverlapRatio , dOverlapRatioWithLastKF);
                break;
            }else
            {
                bIsKeyFrame = true;
                //vecPairs.push_back( std::pair<pImage->m_nID, rit->second->m_nID>);
                vecPairs.emplace_back(std::make_pair(pImage->m_nID, nLastKFId));
                SPDLOG_LOGGER_INFO(m_pLogger, "Add  pair<{},{}>", pImage->m_nID, nLastKFId);
            }
        }else
        {
            bLoopDetect = true;
            //只对候选关键帧做loop detect, 对m_nLoopDetectStartIndex帧之前的关键帧做loop detect
            if(dOverlapRatio > dMaxOverlapRatio)
            {
                bIsKeyFrame = true;
                dMaxOverlapRatio = dOverlapRatio;
                nMaxOverlapIndex = nIteratorNo;
                nMaxOverlapId = rit->first;
            }
        
        }

    }


    //对前一轮得到的最大从叠关键帧的前后帧进行计算，取最终的最大重叠帧做为配对帧
    if(bIsKeyFrame)
    {
        if(bLoopDetect && (dMaxOverlapRatio > m_dMinOverlapRatio))
        {
            nIteratorNo = 0;
            for (rit=m_mapKeyFrame.rbegin(); rit!=m_mapKeyFrame.rend(); ++rit)
            {
                nIteratorNo++;
                if(nIteratorNo == (nMaxOverlapIndex - 1) || nIteratorNo == (nMaxOverlapIndex + 1))
                {
                    double dOverlapRatio = ComputeOverlapArea(pImage, descriptors1, m1_fpts, rit->second);
                    if(dOverlapRatio > dMaxOverlapRatio)
                    {
                        dMaxOverlapRatio = dOverlapRatio;
                        nMaxOverlapIndex = nIteratorNo;
                        nMaxOverlapId = rit->first;
                    }
                }
            }
            //add this match pair 
            vecPairs.emplace_back(std::make_pair( pImage->m_nID, nMaxOverlapId));
            SPDLOG_LOGGER_INFO(m_pLogger, "Add loop pair<{},{}>", pImage->m_nID, nMaxOverlapId);
        }

        {
            std::unique_lock<std::mutex> lock(m_mutex);
            m_mapKeyFrame[pImage->m_nID] = pImage;   
            pImage->file_name = std::to_string(pImage->m_nID);
            m_vecPairs.insert(m_vecPairs.end(),vecPairs.begin(),vecPairs.end());
            SPDLOG_LOGGER_INFO(m_pLogger, "Add new key frame, id:{}, matched size:{}", pImage->m_nID, vecPairs.size());
        }
        
        

#ifndef NDEBUG
        SaveImage2Disk(pImage->m_nID, pImage->img);
#endif

    }else
    {

    }
    
    //this is a key frame
    //detect line

    return 0;
}

void CFeatureMatchThread::SaveImage2Disk(int nId, cv::Mat& img)
{
    std::string strFileName = m_strSavePath + std::to_string(nId) + "_kf.jpg";
    cv::imwrite(strFileName, img);
}

void CFeatureMatchThread::Run()
{
    SPDLOG_LOGGER_INFO(m_pLogger, "CFeatureMatchThread run....");
    while(true)
    {
        //usleep(5*1000);//sleep 5ms
        
        if(IsStopped())
        {
            //quit this thread
            break;
        }

        //get image
        colmap::JobQueue<cv::Mat>::Job j = m_pOriginImgQue->Pop();
        if(!j.IsValid())
        {
            continue;
        }

        SPDLOG_LOGGER_DEBUG(m_pLogger, "Got one image from queue");

        cv::Mat& img = j.Data();
        ImageData* pImage = new ImageData(img, m_nKeyFrameId++);

        //detect feature
        pImage->detectFeature();
        int nSize = pImage->getFeaturePointSize();
        if(nSize < m_nFeaturePointSizeThreshold)
        {
            delete pImage;
            pImage = nullptr;
            SPDLOG_LOGGER_DEBUG(m_pLogger, "nSize[{}] < m_nFeaturePointSizeThreshold[{}]", nSize, m_nFeaturePointSizeThreshold);
            continue;
        }

        //insert first key frame
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            if(m_mapKeyFrame.empty())
            {
                m_mapKeyFrame[pImage->m_nID] = pImage;  
                SPDLOG_LOGGER_INFO(m_pLogger, "Add new key frame, id:{}", pImage->m_nID);
                continue;
            }
        }
        

        //global project and loop detect
        bool bIsKeyFrame = false;
        LoopFeatureMatch(pImage, bIsKeyFrame);

        if(!bIsKeyFrame)
        {
            delete pImage;
            pImage = nullptr;
            continue;
        }
    }
    SPDLOG_LOGGER_WARN(m_pLogger, "CFeatureMatchThread quit....");
}

 
std::map<int, ImageData*> CFeatureMatchThread::GetKeyFrame()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_mapKeyFrame;
}

std::vector<pair<int, int>> CFeatureMatchThread::GetImagePairs()
{
    std::unique_lock<std::mutex> lock(m_mutex);
    return m_vecPairs;
}