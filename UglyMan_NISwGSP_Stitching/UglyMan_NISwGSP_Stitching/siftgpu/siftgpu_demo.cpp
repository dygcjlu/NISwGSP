
//#include <stdio.h>
//#include <string.h>
#include <cstring>
#include <iostream>
#include <string>
#include "./siftgpu.h"

#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "SiftGPU/SiftGPU.h"
#include "util/util_timer.h"

using namespace colmap;
using namespace std;
using namespace cv;
int main()
{
    //string strFile1 = "/home/xxd/dyg/image_stitch/mine/NISwGSP/UglyMan_NISwGSP_Stitching/UglyMan_NISwGSP_Stitching/siftgpu/data/800-1.jpg";
    //string strFile2 = "/home/xxd/dyg/image_stitch/mine/NISwGSP/UglyMan_NISwGSP_Stitching/UglyMan_NISwGSP_Stitching/siftgpu/data/640-1.jpg";
    string strFile1 = "/media/xxd/Data2/datasets/image_stitching/input-42-data/AANAP-building/building0.png";
    string strFile2 = "/media/xxd/Data2/datasets/image_stitching/input-42-data/AANAP-building/building1.png";

    Mat img1 = imread(strFile1);
    Mat img2 = imread(strFile2);

    SiftExtractionOptions options;
    FeatureKeypoints keypoints;
    FeatureDescriptors descriptors;
    Timer time;
    time.Start();

    ExtractSiftFeaturesCPU(options, img1, &keypoints, &descriptors);
    std::cout<<"des rows:"<<descriptors.rows()<<" col:"<<descriptors.cols()<<std::endl;
    
    for(int i = 0; i < descriptors.rows(); i++)
    {
        for(int j =0; j < descriptors.cols(); j++)
        {
            if(j < 10 && i < 10)
            {
                std::cout<<int(descriptors(i, j))<<" ";

            }
            

        }
        if(i < 10)
        {
             std::cout<< std::endl;
        }

       
    }
    std::cout<<"keypoint size:"<<keypoints.size()<<std::endl;
    time.PrintSeconds();

    //SiftGPU* sift = pCreateNewSiftGPU(1);
    SiftGPU sift_gpu;
    CreateSiftGPUExtractor(SiftExtractionOptions(), &sift_gpu);

    FeatureKeypoints keypoints1;
    FeatureDescriptors descriptors1;
    ExtractSiftFeaturesGPU(SiftExtractionOptions(), img1, &sift_gpu, &keypoints1, &descriptors1);
    std::cout<<"keypoint1 size:"<<keypoints1.size()<<std::endl;
     for(int i = 0; i < descriptors1.rows(); i++)
    {
        for(int j =0; j < descriptors1.cols(); j++)
        {
            if(j < 10 && i < 10)
            {
                std::cout<<int(descriptors1(i, j))<<" ";

            }
            

        }
        if(i < 10)
        {
             std::cout<< std::endl;
        }

       
    }

    FeatureKeypoints keypoints2;
    FeatureDescriptors descriptors2;
     time.Start();
    ExtractSiftFeaturesGPU(SiftExtractionOptions(), img2, &sift_gpu, &keypoints2, &descriptors2);
    std::cout<<"keypoint2 size:"<<keypoints2.size()<<std::endl;
    time.PrintSeconds();
  
    //////////////////
    SiftMatchGPU sift_match_gpu;
    if (!CreateSiftGPUMatcher(SiftMatchingOptions(), &sift_match_gpu)) 
    {
        std::cout << "ERROR: SiftGPU not fully supported" << std::endl;
        return 0;
    }
    FeatureMatches matches;
    time.Start();

    MatchSiftFeaturesGPU(SiftMatchingOptions(), &descriptors1, &descriptors2,
                           &sift_match_gpu, &matches);
    std::cout<<"match size:"<<matches.size()<<std::endl;
    time.PrintSeconds();

    FeatureMatches matches2;
     time.Start();

    MatchSiftFeaturesCPUFLANN(SiftMatchingOptions(), descriptors1, descriptors2, &matches2);
    std::cout<<"MatchSiftFeaturesCPUFLANN match size:"<<matches2.size()<<std::endl;
    time.PrintSeconds();

  


    return 0;
}