//
//  Mesh2D.h
//  UglyMan_Stitching
//
//  Created by uglyman.nothinglo on 2015/8/15.
//  Copyright (c) 2015 nothinglo. All rights reserved.
//

#ifndef __UglyMan_Stitiching__Mesh2D__
#define __UglyMan_Stitiching__Mesh2D__

#include "../Configure.h"
#include "../Util/Transform.h"

const int EDGE_VERTEX_SIZE = 2;

class Edge {
public:
    Edge(const int _e1, const int _e2) {
        indices[0] = _e1;
        indices[1] = _e2;
    }
    int indices[EDGE_VERTEX_SIZE];
private:
};

class Indices { /* 3 or 4 */
public:
    vector<int> indices;
    Indices() {
        
    }
    Indices(const int _i0, const int _i1, const int _i2) {
        indices.emplace_back(_i0);
        indices.emplace_back(_i1);
        indices.emplace_back(_i2);
    }
    Indices(const int _i0, const int _i1, const int _i2, const int _i3) {
        indices.emplace_back(_i0);
        indices.emplace_back(_i1);
        indices.emplace_back(_i2);
        indices.emplace_back(_i3);
    }
private:
};

class InterpolateVertex {
public:
    int polygon;
    vector<double> weights;
    InterpolateVertex() {
        polygon = -1;
    }
    InterpolateVertex(const InterpolateVertex & _iv) {
        polygon = _iv.polygon;
        weights = _iv.weights;
    }
    InterpolateVertex(const int _polygon,
                      const double _w0, const double _w1, const double _w2) {
        polygon = _polygon;
        weights.emplace_back(_w0);
        weights.emplace_back(_w1);
        weights.emplace_back(_w2);
    }
    InterpolateVertex(const int _polygon,
                      const vector<double> & _weights) {
        polygon = _polygon;
        weights = _weights;
    }
private:
};

class Mesh2D {
public:
    int nw, nh;
    double lw, lh;
    Mesh2D(const int _cols, const int _rows);
    virtual ~Mesh2D();
    virtual const vector<Point2> & getVertices() const = 0;
    virtual const vector<Edge> & getEdges() const = 0;
    virtual const vector<Indices> & getPolygonsIndices() const = 0;
    virtual const vector<Indices> & getPolygonsNeighbors() const = 0;
    virtual const vector<Indices> & getPolygonsEdges() const = 0;
    virtual const vector<Indices> & getVertexStructures() const = 0;
    virtual const vector<Indices> & getEdgeStructures() const = 0; /* grid neighbor */
    virtual const vector<Indices> & getTriangulationIndices() const = 0;
    virtual const int & getPolygonVerticesCount() const = 0;
    virtual const vector<int> & getBoundaryVertexIndices() const = 0; /* clockwise order */
    virtual const vector<int> & getBoundaryEdgeIndices() const = 0;
    
    virtual InterpolateVertex getInterpolateVertex(const Point_<float> & _p) const = 0;
    virtual InterpolateVertex getInterpolateVertex(const Point_<double> & _p) const = 0;
    
    virtual const vector<Point2> & getPolygonsCenter() const;
    
    //获取像素坐标点p所属网格的索引
    template <typename T>
    int getGridIndexOfPoint(const Point_<T> & _p) const;
    
protected:
    mutable vector<Point2> vertices; //保存网格每个顶点的坐标，像素坐标
    mutable vector<Point2> polygons_center;//保存每个网格中心点的坐标，像素坐标
    mutable vector<Edge> edges;  //保存网格每一条边的两个端点坐标，网格坐标
    mutable vector<Indices> polygons_indices;  //保存的是每个网格四个顶点对应的网格坐标索引
    mutable vector<Indices> polygons_neighbors; //保存与每个网格顶点相领的四个顶点的网格坐标索引
    mutable vector<Indices> polygons_edges;
    mutable vector<Indices> vertex_structures;  //网格顶点相邻的四个顶点坐标索引
    mutable vector<Indices> edge_structures;
    mutable vector<Indices> triangulation_indices;
    mutable vector<int> boundary_vertex_indices;
    mutable vector<int> boundary_edge_indices;
};

#endif /* defined(__UglyMan_Stitiching__Mesh2D__) */
