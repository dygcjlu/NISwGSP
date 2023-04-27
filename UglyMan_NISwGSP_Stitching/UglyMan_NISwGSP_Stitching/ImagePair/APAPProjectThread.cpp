#include <opencv2/opencv.hpp>
#include "../siftgpu/siftgpu_manage.h"
#include "../Stitching/APAP_Stitching.h"
#include "APAPProjectThread.h"

CAPAPProjectThread::CAPAPProjectThread()
{
    m_nIndexEncodeBase=100000;
    m_pLogger = GetLoggerHandle();

    SPDLOG_LOGGER_INFO(m_pLogger, "CAPAPProjectThread::CAPAPProjectThread...");

}

int CAPAPProjectThread::Init()
{

    return 0;
}

int CAPAPProjectThread::StopThread()
{
    Stop();
    return 0;
}

int CAPAPProjectThread::SetFeatureMatchObject(CFeatureMatchThread* pFeatureMatchThread)
{
    m_pFeatureMatchThread = pFeatureMatchThread;
    if(nullptr == m_pFeatureMatchThread)
    {
        SPDLOG_LOGGER_ERROR(m_pLogger, "nullptr == m_pFeatureMatchThread");
        return -1;
    }
    return 0;
}


vector<pair<int, int> > CAPAPProjectThread::DealWithRANSAC( const vector<Point2> & _X,
                                                                       const vector<Point2> & _Y,
                                                                       const vector<pair<int, int> > & _initial_indices)
{
    const int HOMOGRAPHY_MODEL_MIN_POINTS = 4;
    const int GLOBAL_MAX_ITERATION = log(1 - OPENCV_DEFAULT_CONFIDENCE) / log(1 - pow(GLOBAL_TRUE_PROBABILITY, HOMOGRAPHY_MODEL_MIN_POINTS));
    
   
    vector<char> final_mask(_initial_indices.size(), 0);
    cv::Mat H = findHomography(_X, _Y, CV_RANSAC, 5, final_mask, GLOBAL_MAX_ITERATION);
    //std::cout<<"H:"<<H<<std::endl;
     
    
    vector<Point2> tmp_X = _X, tmp_Y = _Y;
    
    vector<int> mask_indices(_initial_indices.size(), 0);
    for(int i = 0; i < mask_indices.size(); ++i) {
        mask_indices[i] = i;
    }
    
    while(tmp_X.size() >= HOMOGRAPHY_MODEL_MIN_POINTS) 
    {
        const int LOCAL_MAX_ITERATION = log(1 - OPENCV_DEFAULT_CONFIDENCE) / log(1 - pow(LOCAL_TRUE_PROBABILITY, HOMOGRAPHY_MODEL_MIN_POINTS));
        vector<Point2> next_X, next_Y;
        vector<char> mask(tmp_X.size(), 0);
        H = findHomography(tmp_X, tmp_Y, CV_RANSAC, 3, mask, LOCAL_MAX_ITERATION);
        //std::cout<<"h:"<<H<<std::endl;

        int inliers_count = 0;
        for(int i = 0; i < mask.size(); ++i) {
            if(mask[i]) { ++inliers_count; }
        }
        if(inliers_count < 40) {
            break;
        }
        for(int i = 0, shift = -1; i < mask.size(); ++i) {
            if(mask[i]) {
                final_mask[mask_indices[i]] = 1;
            } else {
                next_X.emplace_back(tmp_X[i]);
                next_Y.emplace_back(tmp_Y[i]);
                mask_indices[++shift] = mask_indices[i];
            }
        }
#ifndef NDEBUG
        //cout << "Local true Probabiltiy = " << next_X.size() / (float)tmp_X.size() << endl;
#endif
        tmp_X = next_X;
        tmp_Y = next_Y;
    }
    vector<pair<int, int> > result;
    for(int i = 0; i < final_mask.size(); ++i) {
        if(final_mask[i]) {
            result.emplace_back(_initial_indices[i]);
        }
    }
#ifndef NDEBUG
    cout << "Global true Probabiltiy = " << result.size() / (float)_initial_indices.size() << endl;
#endif
    return result;
}

int  CAPAPProjectThread::GetFeatureMatches(ImageData* pImage1, ImageData* pImage2, vector<vector<Point2>>& vecFeatureMatches)
{
    colmap::FeatureDescriptors & descriptors1 = pImage1->getFeatureDescriptorsSiftgpu();
    const vector<Point2> & m1_fpts = pImage1->getFeaturePoints();

    colmap::FeatureDescriptors & descriptors2 = pImage2->getFeatureDescriptorsSiftgpu();
    const vector<Point2> & m2_fpts = pImage2->getFeaturePoints();
    colmap::FeatureMatches matches;
    vector<pair<int, int> > initial_indices;

    CSiftgpuManage::getInstance().FeatureMatch(matches, descriptors1, descriptors2);
    if(matches.size() < 40)
    {
        return 0;
    }

    for(int i = 0; i < matches.size(); i++)
    {
        initial_indices.emplace_back(matches[i].point2D_idx1, matches[i].point2D_idx2);
    }

    vector<Point2> X, Y;
    X.reserve(initial_indices.size());
    Y.reserve(initial_indices.size());
    for(int j = 0; j < initial_indices.size(); ++j) 
    {
        const pair<int, int> it = initial_indices[j];
        X.emplace_back(m1_fpts[it.first ]);
        Y.emplace_back(m2_fpts[it.second]);
    }
    
    vector<pair<int, int> > result;
    result = DealWithRANSAC( X, Y, initial_indices);

    //////////////////
    
    //vector<Point2> vecFeatureMatches2;

    for(int j = 0; j < result.size(); ++j) 
    {
        vecFeatureMatches[0].emplace_back(m1_fpts[result[j].first ]);
        vecFeatureMatches[1].emplace_back(m2_fpts[result[j].second]);
    }

}

int CAPAPProjectThread::ImageLineProject(ImageData* pImage, vector<Point2>& vecFeatureMatches1, 
                                         vector<Point2>& vecFeatureMatches2, vector<Point2>& vecProjectedLine)
{
    const vector<LineData> & lines = pImage->getLines();
    //pImage2->getLines(); //preprocess, no need now
    vector<Point2> points, project_points;
    points.reserve(lines.size() * EDGE_VERTEX_SIZE);
    for(int i = 0; i < lines.size(); ++i) 
    {   
        for(int j = 0; j < EDGE_VERTEX_SIZE; ++j) 
        {
            points.emplace_back(lines[i].data[j]);
        }
        
    }
    vector<Mat> not_be_used;
    //TimeCalculator timer2;
      
    //timer2.start();
    //vector<vector<Point2>> vecProjectedLine;
    //vecProjectedLine.resize(2);
    APAP_Stitching::apap_project(vecFeatureMatches1, vecFeatureMatches2, points, vecProjectedLine, not_be_used);


    return 0;
}


int CAPAPProjectThread::PreComputeAPAPProject(ImageData* pImage1, ImageData* pImage2)
{
    vector<vector<Point2>> vecFeatureMatches;
    vecFeatureMatches.resize(2);
    GetFeatureMatches(pImage1, pImage2, vecFeatureMatches);
    int nSrcIndex = pImage1->m_nID;
    int nDstIndex = pImage2->m_nID;
    
    //std::map<int, std::map<int, std::vector<cv::Mat>>> m_APAPHomographies;
    //std::map<int, std::map<int, std::vector<Point2>>> m_matchPoints;
    vector<vector<cv::Mat>> vecAPAPHomographies;
    vecAPAPHomographies.resize(2);

    vector<vector<Point2>> vecMatchPoints;
    vecMatchPoints.resize(2);

    APAP_Stitching::apap_project(vecFeatureMatches[0],
                                     vecFeatureMatches[1],
                                     pImage1->mesh_2d->getVertices(), vecMatchPoints[0], vecAPAPHomographies[0]);

    APAP_Stitching::apap_project(vecFeatureMatches[1],
                                     vecFeatureMatches[0],
                                     pImage2->mesh_2d->getVertices(), vecMatchPoints[1], vecAPAPHomographies[1]);

    const int PAIR_SIZE = 2;
    const vector<Point2> * out_dst[PAIR_SIZE] = { &vecMatchPoints[0], &vecMatchPoints[1] };
    //std::map<int, std::map<int, std::vector<bool>>> m_overlapMask;
    vector<vector<bool>> vecOverlapMask;
    vecOverlapMask.resize(2);
    vecOverlapMask[0].resize(vecAPAPHomographies[0].size(), false);
    vecOverlapMask[1].resize(vecAPAPHomographies[1].size(), false);

    vector<ImageData*> vecImg;
    vecImg.push_back(pImage1);
    vecImg.push_back(pImage2);

    detail::MatchesInfo matchInfo;

    //vector<vector<bool>> vecImagesFeatureMask;
    //vecImagesFeatureMask.resize(2);

    //vector<detail::ImageFeatures> vecImagesFeatures;
    //vecImagesFeatures.resize(2);
    if(!m_mapImagesFeatures.count(nSrcIndex))
    {
        m_mapImagesFeatures[nSrcIndex] = detail::ImageFeatures();
        const vector<Point2> & vertices = pImage1->mesh_2d->getVertices();
        if(!m_mapImagesFeaturesMask.count(nSrcIndex))
        {
            m_mapImagesFeaturesMask[nSrcIndex] =  vector<bool>();
        }

        m_mapImagesFeaturesMask[nSrcIndex].resize(vertices.size(), false);
        for(int j = 0; j < vertices.size(); ++j) 
        {
            m_mapImagesFeatures[nSrcIndex].keypoints.emplace_back(vertices[j], 0);
        }
    }

    if(!m_mapImagesFeatures.count(nDstIndex))
    {
        m_mapImagesFeatures[nDstIndex] = detail::ImageFeatures();
        const vector<Point2> & vertices = pImage2->mesh_2d->getVertices();
        if(!m_mapImagesFeaturesMask.count(nDstIndex))
        {
            m_mapImagesFeaturesMask[nDstIndex] =  vector<bool>();
        }

        m_mapImagesFeaturesMask[nDstIndex].resize(vertices.size(), false);
        for(int j = 0; j < vertices.size(); ++j) 
        {
            m_mapImagesFeatures[nDstIndex].keypoints.emplace_back(vertices[j], 0);
        }
    }

/*
    for(int i = 0; i < vecImg.size(); i++)
    {
        const vector<Point2> & vertices = vecImg[i]->mesh_2d->getVertices();
        vecImagesFeatureMask[i].resize(vertices.size(), false);
        for(int j = 0; j < vertices.size(); ++j) 
        {
            vecImagesFeatures[i].keypoints.emplace_back(vertices[j], 0);
        }
    }*/

    vector<DMatch> & D_matches = matchInfo.matches;
    
    for(int j = 0; j < PAIR_SIZE; ++j) 
    {
        for(int k = 0; k < out_dst[j]->size(); ++k) 
        {
                if((*out_dst[j])[k].x >= 0 && (*out_dst[j])[k].y >= 0 &&
                   (*out_dst[j])[k].x <= vecImg[!j]->img.cols &&
                   (*out_dst[j])[k].y <= vecImg[!j]->img.rows) 
                {
                    //D_matches的第一个值为M1的点，第二个值为m2的点，保存了两种匹配情况，
                    // 第一种为m1投影到m2，若在m2图像内，则保存m1源点与投影后在m2上的点，两个点为匹配点
                    // 第二种为M2投影到m1，若在m1图像内，则保存投影后的M1点，以及M2上的源点，两个点为匹配点对
                    if(j) {
                        //apap_overlap_mask[m2][m1][k] = true;
                        vecOverlapMask[j][k] = true;
                        D_matches.emplace_back(m_mapImagesFeatures[nSrcIndex].keypoints.size(), k, 0);//m2的第K个点投影到m1的坐标，存储在m1的keypoint中，即，m2的第k个顶点与投影到m1的顶点坐标
                        //images_features_mask[m2][k] = true;
                        //vecImagesFeatureMask[j][k] = true;
                        m_mapImagesFeaturesMask[nDstIndex][k] = true;
                        m_mapImagesFeatures[nSrcIndex].keypoints.emplace_back((*out_dst[j])[k], 0);
                    } else {
                        vecOverlapMask[j][k] = true;
                        //apap_overlap_mask[m1][m2][k] = true; //m1的第K个网格顶点投影到m2图像，如果在M2图像内，则值为true
                        D_matches.emplace_back(k, m_mapImagesFeatures[nDstIndex].keypoints.size(), 0);//保存顶点配对关系，m1的第k个顶点与m2图像keypoint列表的第n个点配对，即，m1的第k个顶点投影到m2后的顶点坐标
                        //images_features_mask[m1][k] = true;//m1的第K个网格点，有与它匹配的网格点
                        //vecImagesFeatureMask[j][k] = true;
                        m_mapImagesFeaturesMask[nSrcIndex][k] = true;
                        m_mapImagesFeatures[nDstIndex].keypoints.emplace_back((*out_dst[j])[k], 0);
                    }
                    //存储当前图像网格顶点坐标以及其他图像顶点投影到当前图像后的顶点坐标
                }
        }
    }

    matchInfo.confidence  = 2.; /*** need > 1.f ***/
    matchInfo.src_img_idx = nSrcIndex;
    matchInfo.dst_img_idx = nDstIndex;
    matchInfo.inliers_mask.resize(D_matches.size(), 1);
    matchInfo.num_inliers = (int)D_matches.size();
    matchInfo.H = vecAPAPHomographies[0].front(); /*** for OpenCV findMaxSpanningTree funtion ***/

    ///////project line
    vector<vector<Point2>> vecProjectedLine;
    vecProjectedLine.resize(2);
    ImageLineProject(pImage1, vecFeatureMatches[0], vecFeatureMatches[1], vecProjectedLine[0]);
    ImageLineProject(pImage2, vecFeatureMatches[1], vecFeatureMatches[0], vecProjectedLine[1]);

    //////////save result to member variable
    int nForwardPairIndex = 0;
    int nBackwradPairIndex = 0;
    GetEncodeIndex(nSrcIndex, nDstIndex, &nForwardPairIndex);
    GetEncodeIndex(nDstIndex, nSrcIndex, &nBackwradPairIndex);

    m_mapAPAPHomographies[nForwardPairIndex] = vecAPAPHomographies[0]; // src 到 dst的单应矩阵列表
    m_mapAPAPHomographies[nBackwradPairIndex] = vecAPAPHomographies[1];// dst 到 src的单应矩阵列表

    m_mapMatchPoints[nForwardPairIndex] = vecMatchPoints[0]; //src 网格顶点 到 dst上后在dst的坐标
    m_mapMatchPoints[nBackwradPairIndex] = vecMatchPoints[1]; //相反

    m_mapOverlapMask[nForwardPairIndex] = vecOverlapMask[0]; //src的第K个网格顶点投影到dst图像，如果在M2图像内，则值为true
    m_mapOverlapMask[nBackwradPairIndex] = vecOverlapMask[1];//相反

    m_mapFeatureMatches[nForwardPairIndex] = vecFeatureMatches[0];//src与dst匹配点的第j个特征匹配点的坐标，src上的坐标
    m_mapFeatureMatches[nBackwradPairIndex] = vecFeatureMatches[1];

    m_mapPairwiseMatches[nForwardPairIndex] = matchInfo; //保存匹配图相对的匹配信息,其中，D_matches的第一个值为src的点，第二个值为dst的点

    //???不能用赋值，需要循环添加
    //m_mapImagesFeatures[nSrcIndex] = vecImagesFeatures[0]; ////存储当前图像网格顶点坐标以及其他图像顶点投影到当前图像后的顶点坐标,
    //m_mapImagesFeatures[nDstIndex] = vecImagesFeatures[1];

    // m_mapImagesFeaturesMask[nSrcIndex] = vecImagesFeatureMask[0]; //src的第K个网格点，其他图像中有与它匹配的网格点,相当于各图像的网格顶点是否与其他图像有重叠
    //m_mapImagesFeaturesMask[nDstIndex] = vecImagesFeatureMask[1];

    m_mapImagesLinesProjects[nForwardPairIndex] = vecProjectedLine[0];
    m_mapImagesLinesProjects[nBackwradPairIndex] = vecProjectedLine[1];

    return 0;
}


void CAPAPProjectThread::Run()
{
    m_bIsIdle = false;
    SPDLOG_LOGGER_INFO(m_pLogger, "CAPAPProjectThread run....");
    while(true)
    {
        usleep(10*1000);//sleep 10ms
        
        if(IsStopped())
        {
            //quit this thread
            SPDLOG_LOGGER_WARN(m_pLogger, "quit this thread soon!");
            break;
        }

        //get matched pairs
        std::map<int, ImageData*> mapKeyFrame = m_pFeatureMatchThread->GetKeyFrame();
        std::vector<pair<int, int>> vecPairs = m_pFeatureMatchThread->GetImagePairs();

        //process pairs
        bool bHaveNewPair = false;
        for(auto& onePair : vecPairs)
        {
            int nPairId;// = onePair.first * 100000 +  onePair.second;
            GetEncodeIndex(onePair.first, onePair.second, &nPairId);
            if(m_setDonePairList.count(nPairId))
            {
                //Already processed before
                continue;
            }
            bHaveNewPair = true;

            PreComputeAPAPProject(mapKeyFrame[onePair.first], mapKeyFrame[onePair.second]);

            m_setDonePairList.insert(nPairId);
            SPDLOG_LOGGER_INFO(m_pLogger, "process pair<{},{}> finished.", onePair.first, onePair.second);
        }

        if(bHaveNewPair)
        {
            m_bIsIdle = false;
        }else{
            m_bIsIdle = true;
        }
    }

    SPDLOG_LOGGER_WARN(m_pLogger, "CAPAPProjectThread quit....");


}