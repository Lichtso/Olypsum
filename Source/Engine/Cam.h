//
//  Cam.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "FBO.h"
#import "Frustum3.h"

#ifndef Cam_h
#define Cam_h

class Cam {
    public:
    Matrix4 camMat, viewMat;
    float fov, near, far, width, height;
    Frustum3 frustum;
    Cam();
    ~Cam();
    Ray3 getRayAt(Vector3 screenPos);
    Frustum3 getFrustumOf(Vector3 screenMin, Vector3 screenMax);
    void calculate();
    void use();
};

extern Cam *mainCam, *guiCam, *currentCam;

#endif