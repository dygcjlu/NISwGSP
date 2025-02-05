//
//  NISwGSP_Stitching.cpp
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#include "NISwGSP_Stitching.h"

#include "../Debugger/TimeCalculator.h"

NISwGSP_Stitching::NISwGSP_Stitching(const MultiImages & _multi_images) : MeshOptimization(_multi_images) {
    
}

void NISwGSP_Stitching::setWeightToAlignmentTerm(const double _weight) {
    MeshOptimization::setWeightToAlignmentTerm(_weight);
}

void NISwGSP_Stitching::setWeightToLocalSimilarityTerm(const double _weight) {
    MeshOptimization::setWeightToLocalSimilarityTerm(_weight);
}

void NISwGSP_Stitching::setWeightToGlobalSimilarityTerm(const double _weight_beta,
                                                        const double _weight_gamma,
                                                        const enum GLOBAL_ROTATION_METHODS _global_rotation_method) {
    MeshOptimization::setWeightToGlobalSimilarityTerm(_weight_beta, _weight_gamma, _global_rotation_method);
}

Mat NISwGSP_Stitching::solve(const BLENDING_METHODS & _blend_method) {
    TimeCalculator timer;
      
    timer.start();
    const MultiImages & multi_images = getMultiImages();
    
    vector<Triplet<double> > triplets;
    vector<pair<int, double> > b_vector;
    
    reserveData(triplets, b_vector, DIMENSION_2D);
    timer.end("reserveData");
    timer.start();
    
    triplets.emplace_back(0, 0, STRONG_CONSTRAINT);
    triplets.emplace_back(1, 1, STRONG_CONSTRAINT);
    b_vector.emplace_back(0,    STRONG_CONSTRAINT);
    b_vector.emplace_back(1,    STRONG_CONSTRAINT);
    
    prepareAlignmentTerm(triplets);
    timer.end("prepareAlignmentTerm");
    timer.start();
    prepareSimilarityTerm(triplets, b_vector);
    timer.end(" prepareSimilarityTerm");
    timer.start();
    
    vector<vector<Point2> > original_vertices;

    original_vertices = getImageVerticesBySolving(triplets, b_vector);
    timer.end(" getImageVerticesBySolving");
    timer.start();
    
    Size2 target_size = normalizeVertices(original_vertices);
    timer.end("normalizeVertices");
    timer.start();
    
    Mat result = multi_images.textureMapping(original_vertices, target_size, _blend_method);
    timer.end("textureMapping");
    timer.start();
    
#ifndef NDEBUG
    multi_images.writeResultWithMesh(result, original_vertices, "-[NISwGSP]" +
                                     GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                                     BLENDING_METHODS_NAME[_blend_method] +
                                     "[Mesh]", false);
    multi_images.writeResultWithMesh(result, original_vertices, "-[NISwGSP]" +
                                     GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
                                     BLENDING_METHODS_NAME[_blend_method] +
                                     "[Border]", true);
#endif
    timer.end("writeResultWithMesh");
    return result;
}

void NISwGSP_Stitching::writeImage(const Mat & _image, const string _post_name) const {
    const MultiImages & multi_images = getMultiImages();
    const Parameter & parameter = multi_images.parameter;
    string file_name = parameter.file_name;
    
    imwrite(parameter.result_dir + file_name + "-" +
            "[NISwGSP]" +
            GLOBAL_ROTATION_METHODS_NAME[getGlobalRotationMethod()] +
            _post_name +
            ".png", _image);
}