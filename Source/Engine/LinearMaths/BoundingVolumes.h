//
//  BoundingVolumes.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Matrix4.h"

#ifndef BoundingVolumes_h
#define BoundingVolumes_h

class Plane3;

class BoundingVolume {
    public:
    Matrix4* transformation;
    BoundingVolume(Matrix4* transformation);
};

class Aabb3 : public BoundingVolume {
    public:
    Vector3 min, max;
    Aabb3(Vector3 min, Vector3 max, Matrix4* transformation);
    void getVertices(Vector3 vertices[8]);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
    bool testAabbHit(Aabb3* aabb);
};

class Box3 : public Aabb3 {
    public:
    Box3(Vector3 min, Vector3 max, Matrix4* transformation);
    void getVertices(Vector3 vertices[8]);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
};

class Bs3 : public BoundingVolume {
    public:
    float radius;
    Bs3(float radius, Matrix4* transformation);
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3* bs);
};

#endif
