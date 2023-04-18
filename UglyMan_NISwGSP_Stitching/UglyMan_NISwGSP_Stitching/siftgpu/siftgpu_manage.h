#ifndef SIFT_GPU_MANAGE_H
#define SIFT_GPU_MANAGE_H
#include <iostream>
#include "opencv2/core/core.hpp"

#include "SiftGPU/SiftGPU.h"
#include "siftgpu.h"
class CSiftgpuManage
{
public:

    static CSiftgpuManage& getInstance()
    {
        static CSiftgpuManage inst;
        return inst;
    }

    CSiftgpuManage(const CSiftgpuManage&) = delete;
    CSiftgpuManage& operator=(const CSiftgpuManage&) = delete;

    bool  FeatureDetect(cv::Mat greyImg,  colmap::FeatureKeypoints& keypoints, colmap::FeatureDescriptors& descriptors);
    void  FeatureMatch(colmap::FeatureMatches& matches,  colmap::FeatureDescriptors& descriptors1, colmap::FeatureDescriptors& descriptors2);
private:
    CSiftgpuManage();

private:
    SiftGPU m_siftgpu;
    colmap::SiftExtractionOptions m_siftExtractOps;

    SiftMatchGPU m_siftgpuMatch;
    colmap::SiftMatchingOptions m_siftMatchOps;
};

#endif