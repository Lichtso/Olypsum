//
//  BoundingVolumes.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Matrix4.h"

#ifndef BoundingVolumes_h
#define BoundingVolumes_h

class Plane3;

class BoundingVolume {
    public:
    Matrix4 transformation, *parentTransformation;
    BoundingVolume() { transformation.setIdentity(); };
    BoundingVolume(Matrix4* parentTrans) : parentTransformation(parentTrans) { transformation.setIdentity(); };
    BoundingVolume(Matrix4 trans) : transformation(trans), parentTransformation(NULL) {};
    BoundingVolume(Matrix4 trans, Matrix4* parentTrans) : transformation(trans), parentTransformation(parentTrans) {};
    Matrix4 getTransformation();
};

extern unsigned int boxIndecies[36];
#define boxTrianglesCount 12
#define boxVerticesCount 8

class Aabb3 {
    public:
    Vector3 min, max;
    Aabb3(Vector3 min, Vector3 max);
    unsigned int getVertices(Vector3 vertices[boxVerticesCount]);
    void drawWireFrame(Vector3 color);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
    bool testAabbHit(Aabb3* aabb);
};

class Box3 : public BoundingVolume {
    public:
    Vector3 min, max;
    Box3(Matrix4 transformation, Vector3 min, Vector3 max);
    unsigned int getVertices(Vector3 vertices[boxVerticesCount]);
    void drawWireFrame(Vector3 color);
    void getPlanes(Plane3 planes[6]);
    bool testPointHit(Vector3 pos);
};

#define sphereTrianglesCount(accuracyX, accuracyY) (accuracyX*accuracyY*2)
#define sphereVerticesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2+2)

class Bs3 : public BoundingVolume {
    public:
    float radius;
    Bs3(Matrix4 transformation, float radius);
    Vector3 getPosition();
    unsigned int getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY);
    unsigned int getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY);
    void drawWireFrame(Vector3 color);
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3* bs);
};

#define parabolidTrianglesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2-2)
#define parabolidVerticesCount(accuracyX, accuracyY) (accuracyX+accuracyX*accuracyY+1)

class Parabolid3 : public BoundingVolume {
    public:
    float radius;
    Parabolid3(Matrix4 transformation, float radius);
    unsigned int getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY);
    unsigned int getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY);
    void drawWireFrame(Vector3 color);
};

#endif
