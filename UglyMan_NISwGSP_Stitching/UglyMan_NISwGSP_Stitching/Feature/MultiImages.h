//
//  MultiImages.h
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#ifndef __UglyMan_Stitiching__MultiImages__
#define __UglyMan_Stitiching__MultiImages__

//#include <queue>
#include <vector>

#include "../Configure.h"
#include "../Stitching/Parameter.h"
#include "../Feature/ImageData.h"
#include "../Util/Statistics.h"
#include "../Debugger/ImageDebugger.h"
#include "../Stitching/APAP_Stitching.h"
#include "../Util/Blending.h"
#include "../Debugger/ColorMap.h"

#include <opencv2/calib3d.hpp> /* CV_RANSAC */
#include <opencv2/stitching/detail/autocalib.hpp> /* ImageFeatures, MatchesInfo */
#include <opencv2/stitching/detail/camera.hpp> /* CameraParams */
#include <opencv2/stitching/detail/motion_estimators.hpp> /* BundleAdjusterBase */

#include "../ImagePair/APAPProjectThread.h"
#include "../ImagePair/FeatureMatchThread.h"


const int PAIR_COUNT = 2;

class FeatureDistance {
public:
    double distance;
    int feature_index[PAIR_COUNT];
    FeatureDistance() {
        feature_index[0] = feature_index[1] = -1;
        distance = MAXFLOAT;
    }
    FeatureDistance(const double _distance,
                    const int _p_1,
                    const int _feature_index_1,
                    const int _feature_index_2) {
        distance = _distance;
        feature_index[ _p_1] = _feature_index_1;
        feature_index[!_p_1] = _feature_index_2;
    }
    bool operator < (const FeatureDistance & fd) const {
        return distance > fd.distance;
    }
private:
};

class SimilarityElements {
public:
    double scale;
    double theta;
    SimilarityElements(const double _scale,
                       const double _theta) {
        scale = _scale;
        theta = _theta;
    }
private:
};

class MultiImages {
public:
    /*
    MultiImages(const string & _file_name,
                LINES_FILTER_FUNC * _width_filter  = &LINES_FILTER_NONE,
                LINES_FILTER_FUNC * _length_filter = &LINES_FILTER_NONE);
                */
    MultiImages(const string & rootPath);
    MultiImages(const string & rootPath, const string & _file_name,
                LINES_FILTER_FUNC * _width_filter  = &LINES_FILTER_NONE,
                LINES_FILTER_FUNC * _length_filter = &LINES_FILTER_NONE);
    ~MultiImages();
    int Clear();
    
    const vector<detail::ImageFeatures> & getImagesFeaturesByMatchingPoints() const;
    const vector<detail::MatchesInfo>   & getPairwiseMatchesByMatchingPoints() const;
    const vector<detail::CameraParams>  & getCameraParams() const;
    
    const vector<vector<bool> > & getImagesFeaturesMaskByMatchingPoints() const;
    
    const vector<vector<vector<pair<int, int> > > > & getFeaturePairs() const;
    const vector<vector<vector<Point2> > >          & getFeatureMatches() const;
    
    const vector<vector<vector<bool> > >   & getAPAPOverlapMask() const;
    const vector<vector<vector<Mat> > >    & getAPAPHomographies() const;
    const vector<vector<vector<Point2> > > & getAPAPMatchingPoints() const;
    
    const vector<vector<InterpolateVertex> > & getInterpolateVerticesOfMatchingPoints() const;
    
    const vector<int> & getImagesVerticesStartIndex() const;
    const vector<SimilarityElements> & getImagesSimilarityElements(const enum GLOBAL_ROTATION_METHODS & _global_rotation_method) const;
    const vector<vector<pair<double, double> > > & getImagesRelativeRotationRange() const;
    
    const vector<vector<double> > & getImagesGridSpaceMatchingPointsWeight(const double _global_weight_gamma) const;

    const vector<Point2> & getImagesLinesProject(const int _from, const int _to) const;
    
    const vector<Mat> & getImages() const;
    
    FLOAT_TYPE getImagesMinimumLineDistortionRotation(const int _from, const int _to) const;
    
    Mat textureMapping(const vector<vector<Point2> > & _vertices,
                       const Size2 & _target_size,
                       const BLENDING_METHODS & _blend_method) const;
    
    Mat textureMapping(const vector<vector<Point2> > & _vertices,
                       const Size2 & _target_size,
                       const BLENDING_METHODS & _blend_method,
                       vector<Mat> & _warp_images) const;
    
    void writeResultWithMesh(const Mat & _result,
                             const vector<vector<Point2> > & _vertices,
                             const string & _postfix,
                             const bool _only_border) const;
    
    //vector<ImageData> images_data;
    vector<ImageData*> images_data;
    map<int, int> m_mapId2vecIndex;
    Parameter parameter;

public:
    int SetAPAPProjectObject(CAPAPProjectThread* pAPAPProjectObject);
    int SetFeatureMatchObject(CFeatureMatchThread* pFeatureMatchThread);
    int InitMemberVariables();
private:

    vector<pair<int, int> > getInitialFeaturePairsGPU(const pair<int, int> & _match_pair) const;
    vector<vector<vector<pair<int, int> > > > & getFeaturePairsGPU() const;
   
    

private:    
    /*** Debugger ***/
    void writeImageOfFeaturePairs(const string & _name,
                                  const pair<int, int> & _index_pair,
                                  const vector<pair<int, int> > & _pairs) const;
    /****************/
    
    void doFeatureMatching() const;
    void initialFeaturePairsSpace() const;
    
    vector<pair<int, int> > getInitialFeaturePairs(const pair<int, int> & _match_pair) const;
    
    vector<pair<int, int> > getFeaturePairsBySequentialRANSAC(const pair<int, int> & _match_pair,
                                                              const vector<Point2> & _X,
                                                              const vector<Point2> & _Y,
                                                              const vector<pair<int, int> > & _initial_indices) const;
    
    mutable vector<detail::ImageFeatures> images_features; //存储当前图像网格顶点坐标以及其他图像顶点投影到当前图像后的顶点坐标, (当前图像中与其他图像有匹配关系的匹配点坐标，即其他图像的网格顶点通过apap投影到当前图像的点的坐标)

    //保存匹配图相对的匹配信息,其中，D_matches的第一个值为M1的点，第二个值为m2的点，保存了两种匹配情况，
    //第一种为m1投影到m2，若在m2图像内，则保存m1源点与投影后在m2上的点，两个点为匹配点
    //第二种为M2投影到m1，若在m1图像内，则保存投影后的M1点，以及M2上的源点，两个点为匹配点对
    //此处保存的是匹配点在images_features中的索引
    mutable vector<detail::MatchesInfo>   pairwise_matches; 
    mutable vector<detail::CameraParams>  camera_params;
    
    mutable vector<vector<bool> > images_features_mask;  //images_features_mask[m1][k] = true; m1的第K个网格点，其他图像中有与它匹配的网格点,相当于各图像的网格顶点是否与其他图像有重叠
    
    mutable vector<vector<vector<pair<int, int> > > > feature_pairs; //图像之间特征点配对情况，存储的是特征点的索引
    mutable vector<vector<vector<Point2> > > feature_matches; /* [m1][m2][j], img1 j_th matches */ //m1与m2的第j个特征匹配点的坐标，m1上的坐标
    
    mutable vector<vector<vector<bool> > >   apap_overlap_mask; //apap_overlap_mask[m1][m2][k] = true; //m1的第K个网格顶点投影到m2图像，如果在M2图像内，则值为true
    mutable vector<vector<vector<Mat> > >    apap_homographies; //apap_homographies[m1][m2] m1投影到m2的单应矩阵列表
    mutable vector<vector<vector<Point2> > > apap_matching_points;//apap_matching_points[m1][m2] m1上所有网格顶点投影到m2后的坐标
    
    mutable vector<vector<InterpolateVertex> > mesh_interpolate_vertex_of_feature_pts;
    //保存图像每个匹配点所属网格的索引，以及匹配点到网格四个顶点的权重(距离).
    //mesh_interpolate_vertex_of_matching_pts[i][j] 第i张图像的第j个网格顶点对应的索引以及权重
    mutable vector<vector<InterpolateVertex> > mesh_interpolate_vertex_of_matching_pts; 
    
    mutable vector<int> images_vertices_start_index;
    mutable vector<SimilarityElements> images_similarity_elements_2D;
    mutable vector<SimilarityElements> images_similarity_elements_3D;
    mutable vector<vector<pair<double, double> > > images_relative_rotation_range;
    
    mutable vector<vector<double> > images_polygon_space_matching_pts_weight;
    
    /* Line */
    mutable vector<vector<FLOAT_TYPE> > images_minimum_line_distortion_rotation;//最小线段失真对应的图像旋转角度
    mutable vector<vector<vector<Point2> > > images_lines_projects; /* [m1][m2] img1 lines project on img2 *///m1的某个线段两个端点投影到m2后在m2上的坐标
    
    mutable vector<Mat> images;

private:
    bool m_bUseSiftGPU;
    CAPAPProjectThread* m_pAPAPProjectObject;
    CFeatureMatchThread* m_pFeatureMatchThread; //

};

#endif /* defined(__UglyMan_Stitiching__MultiImages__) */
