//
//  main.cpp
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#include <iostream>
#include "./Stitching/NISwGSP_Stitching.h"
#include "./Debugger/TimeCalculator.h"
#include "./ImagePair/GetImageThread.h"
#include "./ImagePair/FeatureMatchThread.h"
#include "./ImagePair/APAPProjectThread.h"

using namespace std;

int main(int argc, const char * argv[])
{
    //
    colmap::JobQueue<cv::Mat> queImages;
    CGetImageThread getImageThread;
    getImageThread.SetJobQueue(&queImages);
 
    //int Init(int nMaxQueueSize, std::string strSrcFile, std::string strSavePath);APAP-building APAP-garden/*JPG
    int nMaxQueueSize = 50;
    std::string strSrcFile = "/media/xxd/Data2/datasets/3d/mine/0315_zheyi/2023-03-15_082835/images/image_stitching/simple3/*jpg";//"/media/xxd/Data2/datasets/image_stitching/input-42-data/APAP-garden/*JPG";
    //std::string strSrcFile = "/media/xxd/Data2/datasets/image_stitching/input-42-data/APAP-garden/*JPG";
    std::string strSavePath = "/media/xxd/Data2/datasets/image_stitching/za/";
    //std::string strSrcFile = argv[1];
    //std::string strSavePath = argv[2];
    int nSrcImgType = 0; //0 - image list in folder,  1 - video file, 2 - camera
    getImageThread.Init(nMaxQueueSize, nSrcImgType, strSrcFile, strSavePath);

    CFeatureMatchThread featureMatchThread;
    featureMatchThread.SetJobQueue(&queImages);
    featureMatchThread.Init(strSavePath);

    CAPAPProjectThread apapProjectThread;
    apapProjectThread.Init();
    apapProjectThread.SetFeatureMatchObject(&featureMatchThread);

    MultiImages multi_images(strSavePath);
    multi_images.SetAPAPProjectObject(&apapProjectThread);
    multi_images.SetFeatureMatchObject(&featureMatchThread);

    //start threads
    getImageThread.Start();
    featureMatchThread.Start();
    apapProjectThread.Start();
    getImageThread.StartScanning();

    TimeCalculator timer;
    timer.start();

    //wait until preprocess finished
    sleep(5); //sleep 5s
    while(!getImageThread.IsIdle())
    {
        usleep(1000); //sleep 1ms
    }
    timer.end("getImageThread finish");
    timer.start();

    getImageThread.GetJobQueuePoiner()->Wait();
    
    timer.end("featurematch thread finish");
    timer.start();

    while(!apapProjectThread.IsIdle())
    {
        usleep(1000); //sleep 1ms
    }
    
    timer.end("apapProjectThread  finish");
    timer.start();

    //init member variables
    multi_images.InitMemberVariables();

    //stitching
    
    timer.end("InitMemberVariables  finish");
    timer.start();
    NISwGSP_Stitching niswgsp(multi_images);
    niswgsp.setWeightToAlignmentTerm(1);
    niswgsp.setWeightToLocalSimilarityTerm(0.75);
    niswgsp.setWeightToGlobalSimilarityTerm(6, 20, GLOBAL_ROTATION_2D_METHOD);
    timer.end("2d common: ");
    //timer.start();
    //niswgsp.writeImage(niswgsp.solve(BLEND_AVERAGE), BLENDING_METHODS_NAME[BLEND_AVERAGE]);
    //timer.end("2d BLEND_AVERAGE: ");
    timer.start();
    niswgsp.writeImage(niswgsp.solve(BLEND_LINEAR),  BLENDING_METHODS_NAME[BLEND_LINEAR]);
    timer.end("2d BLEND_LINEAR: ");


    getImageThread.StopThread();
    featureMatchThread.StopThread();
    apapProjectThread.StopThread();

    getImageThread.Wait();
    featureMatchThread.Wait();
    apapProjectThread.Wait();

    return 0;
}

int main_(int argc, const char * argv[]) 
{
    Eigen::initParallel(); /* remember to turn off "Hardware Multi-Threading */
    Eigen::setNbThreads(4);
    cout << "nThreads = " << Eigen::nbThreads() << endl;
    cout << "[#Images : " << argc - 2<< "]" << endl;

    TimeCalculator timer;
    std::string rootPath = argv[1];
    for(int i = 2; i < argc; ++i) {
        cout << "i = " << i << ", [Images : " << argv[i] << "]" << endl;
        //MultiImages multi_images(argv[i], LINES_FILTER_WIDTH, LINES_FILTER_LENGTH);
        MultiImages multi_images(rootPath, argv[i], LINES_FILTER_WIDTH, LINES_FILTER_LENGTH);
        
        timer.start();
        /* 2D */
        NISwGSP_Stitching niswgsp(multi_images);
        niswgsp.setWeightToAlignmentTerm(1);
        niswgsp.setWeightToLocalSimilarityTerm(0.75);
        niswgsp.setWeightToGlobalSimilarityTerm(6, 20, GLOBAL_ROTATION_2D_METHOD);
        timer.end("2d common: ");
        //timer.start();
        //niswgsp.writeImage(niswgsp.solve(BLEND_AVERAGE), BLENDING_METHODS_NAME[BLEND_AVERAGE]);
        //timer.end("2d BLEND_AVERAGE: ");
        timer.start();
        niswgsp.writeImage(niswgsp.solve(BLEND_LINEAR),  BLENDING_METHODS_NAME[BLEND_LINEAR]);
        timer.end("2d BLEND_LINEAR: ");
         /* 3D */
         /*
        timer.start();
       
        niswgsp.setWeightToAlignmentTerm(1);
        niswgsp.setWeightToLocalSimilarityTerm(0.75);
        niswgsp.setWeightToGlobalSimilarityTerm(6, 20, GLOBAL_ROTATION_3D_METHOD);
        niswgsp.writeImage(niswgsp.solve(BLEND_AVERAGE), BLENDING_METHODS_NAME[BLEND_AVERAGE]);
        niswgsp.writeImage(niswgsp.solve(BLEND_LINEAR),  BLENDING_METHODS_NAME[BLEND_LINEAR]);
        timer.end("[NISwGSP] " + multi_images.parameter.file_name);*/
    }
    return 0;
}
