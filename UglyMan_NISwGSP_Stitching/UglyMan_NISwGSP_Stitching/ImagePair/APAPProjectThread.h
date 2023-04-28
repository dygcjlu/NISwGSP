#ifndef APAP_PROJECT_THREAD_H
#define APAP_PROJECT_THREAD_H

#include "opencv2/core/core.hpp"
#include "../siftgpu/util/util_threading.h"
#include "../Util/CommonDef.h"
#include "../Feature/ImageData.h"
#include "FeatureMatchThread.h"

class CAPAPProjectThread : public colmap::Thread
{
public:
    CAPAPProjectThread();
    int Init();
    int StopThread();
    int SetFeatureMatchObject(CFeatureMatchThread* pFeatureMatchThread);
    void GetEncodeIndex(int nIndex1, int nIndex2, int* nEncodedIndex)
    {
        *nEncodedIndex = m_nIndexEncodeBase * nIndex1 + nIndex2; //100000*index1 + index2
    }

    void GetDecodeIndex(int nEncodedIndex, int* pnIndex1, int* pnIndex2)
    {
        *pnIndex1 = nEncodedIndex / m_nIndexEncodeBase;

        *pnIndex2 = nEncodedIndex % m_nIndexEncodeBase;
    }

    bool IsIdle(){return m_bIsIdle;}

public:
    std::map<int, std::vector<cv::Mat>>& GetAPAPHomographies()
    {
        return m_mapAPAPHomographies;
    }

    std::map<int, std::vector<Point2>>& GetMatchPoints()
    {
        return m_mapMatchPoints;
    }

    std::map<int, std::vector<bool>>& GetOverlapMask()
    {
        return m_mapOverlapMask;
    }

    std::map<int, std::vector<Point2>>& GetFeatureMatches()
    {
        return m_mapFeatureMatches;
    }

    std::map<int, detail::MatchesInfo>& GetPairwiseMatches()
    {
        return m_mapPairwiseMatches;
    }

    std::map<int, detail::ImageFeatures>& GetImagesFeatures()
    {
        return m_mapImagesFeatures;
    }

    std::map<int, std::vector<bool>>& GetImagesFeaturesMask()
    {
        return m_mapImagesFeaturesMask;
    }

    std::map<int, std::vector<Point2>>& GetImagesLinesProjects()
    {
        return m_mapImagesLinesProjects;
    }


private:
    void Run() override;

    vector<pair<int, int> > DealWithRANSAC( const vector<Point2> & _X,
                                            const vector<Point2> & _Y,
                                            const vector<pair<int, int> > & _initial_indices);

    int  GetFeatureMatches(ImageData* pImage1, ImageData* pImage2, vector<vector<Point2>>& vecFeatureMatches);

    int PreComputeAPAPProject(ImageData* pImage1, ImageData* pImage2);

    int ImageLineProject(ImageData* pImage, vector<Point2>& vecFeatureMatches1, 
                                         vector<Point2>& vecFeatureMatches2, vector<Point2>& vecProjectedLine);

    


private:
    std::atomic_bool m_bIsIdle;
    int m_nMatchSizeThreshold;
    int m_nIndexEncodeBase; //encode index1 and index1 to one index
    std::shared_ptr<spdlog::logger> m_pLogger; //log handle
    CFeatureMatchThread* m_pFeatureMatchThread; //
    std::set<int> m_setDonePairList; //save pair that had done precompute

    //mutable vector<vector<vector<Mat> > >    apap_homographies; //apap_homographies[m1][m2] m1投影到m2的单应矩阵列表
    //std::map<int, std::map<int, std::vector<cv::Mat>>> m_APAPHomographies;
    std::map<int, std::vector<cv::Mat>> m_mapAPAPHomographies;

    //mutable vector<vector<vector<Point2> > > apap_matching_points;//apap_matching_points[m1][m2] m1上所有网格顶点投影到m2后的坐标
    //std::map<int, std::map<int, std::vector<Point2>>> m_matchPoints;
    std::map<int, std::vector<Point2>> m_mapMatchPoints;

    //mutable vector<vector<vector<bool> > >   apap_overlap_mask; //apap_overlap_mask[m1][m2][k] = true; //m1的第K个网格顶点投影到m2图像，如果在M2图像内，则值为true
    //std::map<int, std::map<int, std::vector<bool>>> m_overlapMask;
    std::map<int, std::vector<bool>> m_mapOverlapMask;

    //mutable vector<vector<vector<Point2> > > feature_matches; /* [m1][m2][j], img1 j_th matches */ //m1与m2的第j个特征匹配点的坐标，m1上的坐标
    //std::map<int, std::map<int, std::vector<Point2>>> m_featureMatches;
    std::map<int, std::vector<Point2>> m_mapFeatureMatches;

    //mutable vector<detail::MatchesInfo>   pairwise_matches; 
    //std::map<int, std::map<int, detail::MatchesInfo>> m_pairwiseMatches;
    std::map<int, detail::MatchesInfo> m_mapPairwiseMatches; //保存匹配图相对的匹配信息,其中，D_matches的第一个值为M1的点，第二个值为m2的点

    //mutable vector<detail::ImageFeatures> images_features;
    std::map<int, detail::ImageFeatures> m_mapImagesFeatures; ////存储当前图像网格顶点坐标以及其他图像顶点投影到当前图像后的顶点坐标, (当前图像中与其他图像有匹配关系的匹配点坐标，即其他图像的网格顶点通过apap投影到当前图像的点的坐标)

    //mutable vector<vector<bool> > images_features_mask;  //images_features_mask[m1][k] = true; m1的第K个网格点，其他图像中有与它匹配的网格点,相当于各图像的网格顶点是否与其他图像有重叠
    //std::map<int, std::map<int, std::vector<bool>>> m_imagesFeaturesMask;
    std::map<int, std::vector<bool>> m_mapImagesFeaturesMask;

    //mutable vector<vector<FLOAT_TYPE> > images_minimum_line_distortion_rotation;//最小线段失真对应的图像旋转角度

    //mutable vector<vector<vector<Point2> > > images_lines_projects; /* [m1][m2] img1 lines project on img2 *///m1的某个线段两个端点投影到m2后在m2上的坐标
    //std::map<int, std::map<int, std::vector<Point2>>> m_imagesLinesProjects;
    std::map<int, std::vector<Point2>> m_mapImagesLinesProjects;


};

#endif //APAP_PROJECT_THREAD_H