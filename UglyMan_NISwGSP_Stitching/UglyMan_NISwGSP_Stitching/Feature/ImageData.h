//
//  ImageData.h
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#ifndef __UglyMan_Stitiching__ImageData__
#define __UglyMan_Stitiching__ImageData__

#include <memory>
#include "../Util/Statistics.h"
#include "../Feature/FeatureController.h"
#include "../Mesh/MeshGrid.h"
#include "../siftgpu/siftgpu.h"

class LineData {
public:
    LineData(const Point2 & _a,
             const Point2 & _b,
             const double _width,
             const double _length);
    Point2 data[2];
    double width, length;
private:
};

typedef const bool (LINES_FILTER_FUNC)(const double _data, \
                                       const Statistics & _statistics);

LINES_FILTER_FUNC LINES_FILTER_NONE;
LINES_FILTER_FUNC LINES_FILTER_WIDTH;
LINES_FILTER_FUNC LINES_FILTER_LENGTH;


class ImageData {
public:
    string file_name, file_extension;
    const string * file_dir, * debug_dir;
    ImageData(const string & _file_dir,
              const string & _file_full_name,
              LINES_FILTER_FUNC * _width_filter,
              LINES_FILTER_FUNC * _length_filter,
              bool bUseSiftgpu,
              const string * _debug_dir = NULL);
    
    ImageData(cv::Mat& img, int nId, bool bUseSiftgpu = true,
              LINES_FILTER_FUNC * _width_filter  = &LINES_FILTER_WIDTH,
             LINES_FILTER_FUNC * _length_filter = &LINES_FILTER_LENGTH,
              const string * _debug_dir = NULL);
            
    //~ImageData();
    void clear();
    
    const Mat & getGreyImage() const;
    const vector<LineData> & getLines() const;
    const vector<Point2> & getFeaturePoints() const;
    const vector<FeatureDescriptor> & getFeatureDescriptors() const;

    //siftgpu version
    //vector<Point2> & getFeaturePointsSiftgpu();
    colmap::FeatureDescriptors& getFeatureDescriptorsSiftgpu() const;
    int detectFeature();
    int getFeaturePointSize();
    
public: 
    Mat img, rgba_img, alpha_mask;
    unique_ptr<Mesh2D> mesh_2d;
    int m_nID;
    
private:
    LINES_FILTER_FUNC * width_filter, * length_filter;
    
    mutable Mat grey_img;
    mutable vector<LineData> img_lines; //图像中所有线段的两个端点坐标
    mutable vector<Point2> feature_points; //图像中提取的特征点
    mutable vector<FeatureDescriptor> feature_descriptors;//特征描述子

private:
    
    bool m_bUseSiftGPU;
    mutable bool m_bIsFeatureDetected;
    mutable colmap::FeatureKeypoints m_vecFeaturePoints;
    mutable colmap::FeatureDescriptors m_vecFeatureDescriptors;
};

#endif /* defined(__UglyMan_Stitiching__ImageData__) */
