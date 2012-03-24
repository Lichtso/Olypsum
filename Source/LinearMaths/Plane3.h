//
//  Plane3.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Ray3.h"

#ifndef Plane3_h
#define Plane3_h

class Plane3 {
    public:
    float distance;
    Vector3 normal;
    Plane3();
    Plane3(float distance, Vector3 normal);
    Plane3(Vector3 pos, Vector3 normal);
    Plane3(Vector3 posA, Vector3 posB, Vector3 posC);
    Plane3(Ray3 ray);
    void set(float distance, Vector3 normal);
    void set(Vector3 pos, Vector3 normal);
    void set(Vector3 posA, Vector3 posB, Vector3 posC);
    void set(Ray3 ray);
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3 bs);
    bool testAabbHit(Aabb3* aabb);
    unsigned int testAabbHitCount(Aabb3* aabb);
};

#endif
