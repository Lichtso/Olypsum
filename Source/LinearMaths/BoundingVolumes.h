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

class Aabb3 {
    public:
    Vector3 min, max;
    Aabb3();
    Aabb3(Vector3 min, Vector3 max);
    void set(Vector3 min, Vector3 max);
    bool testPointHit(Vector3 pos);
    bool testAabbHit(Aabb3* aabb);
};

class Bs3 {
    public:
    Vector3 center;
    float radius;
    Bs3();
    Bs3(Vector3 center, float radius);
    void set(Vector3 center, float radius);
    bool testPointHit(Vector3 pos);
    bool testBsHit(Bs3* bs);
    bool testAabbHit(Aabb3* aabb);
};

#endif
