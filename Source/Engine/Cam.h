//
//  Cam.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "FBO.h"
#import "Matrix4.h"

#ifndef Cam_h
#define Cam_h

enum CollisionMask {
    CollisionMask_Frustum = 1,
    CollisionMask_Zone = 2,
    CollisionMask_Static = 4,
    CollisionMask_Object = 8
};

class Cam {
    btCollisionShape* frustumShape;
    btVector3 prevPos, velocity;
    public:
    btTransform camMat;
    Matrix4 viewMat;
    float fov, near, far, width, height;
    btCollisionObject* frustumBody;
    Cam();
    ~Cam();
    Ray3 getRayAt(btVector3 screenPos, bool absolute);
    void calculateFrustum(btVector3 screenMin, btVector3 screenMax);
    void calculate();
    void use();
};

extern Cam *mainCam, *guiCam, *currentCam;

#endif