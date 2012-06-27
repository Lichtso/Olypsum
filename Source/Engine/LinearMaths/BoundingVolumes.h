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

#define boxTrianglesCount 12
#define boxVerticesCount 8

class Aabb3 : public BoundingVolume {
    public:
    Vector3 min, max;
    Aabb3(Vector3 min, Vector3 max, Matrix4* transformation);
    unsigned int getIndecies(unsigned int indecies[boxTrianglesCount]);
    unsigned int getVertices(Vector3 vertices[boxVerticesCount]);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
    bool testAabbHit(Aabb3* aabb);
};

class Box3 : public Aabb3 {
    public:
    Box3(Vector3 min, Vector3 max, Matrix4* transformation);
    unsigned int getIndecies(unsigned int indecies[boxTrianglesCount]);
    unsigned int getVertices(Vector3 vertices[boxVerticesCount]);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
};

#define sphereTrianglesCount(accuracyX, accuracyY) (accuracyX*accuracyY*2)
#define sphereVerticesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2+2)

class Bs3 : public BoundingVolume {
    public:
    float radius;
    Bs3(float radius, Matrix4* transformation);
    unsigned int getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY);
    unsigned int getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY);
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3* bs);
};

#define parabolidTrianglesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2-2)
#define parabolidVerticesCount(accuracyX, accuracyY) (accuracyX+accuracyX*accuracyY+1)

class Parabolid3 : public BoundingVolume {
    public:
    float radius;
    Parabolid3(float radius, Matrix4* transformation);
    unsigned int getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY);
    unsigned int getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY);
};

#endif
