#include "siftgpu_manage.h"


CSiftgpuManage::CSiftgpuManage()
{
    if(!CreateSiftGPUExtractor(m_siftExtractOps, &m_siftgpu))
    {
         std::cout<< "CreateSiftGPUExtractor failed!"<<std::endl;
    }

    if (!CreateSiftGPUMatcher(m_siftMatchOps, &m_siftgpuMatch)) 
    {
        std::cout<< "CreateSiftGPUMatcher failed!"<<std::endl;
    }

}


bool  CSiftgpuManage::FeatureDetect(cv::Mat greyImg,  colmap::FeatureKeypoints& keypoints, colmap::FeatureDescriptors& descriptors)
{

    bool bResult = ExtractSiftFeaturesGPU(m_siftExtractOps, greyImg, &m_siftgpu, &keypoints, &descriptors);
    if(!bResult)
    {
        std::cout<<"ExtractSiftFeaturesGPU failed"<<std::endl;
    }
    return bResult;
}


void  CSiftgpuManage::FeatureMatch( colmap::FeatureMatches& matches,  colmap::FeatureDescriptors& descriptors1, colmap::FeatureDescriptors& descriptors2)
{
    
    MatchSiftFeaturesGPU(m_siftMatchOps, &descriptors1, &descriptors2,
                           &m_siftgpuMatch, &matches);


    return;
}