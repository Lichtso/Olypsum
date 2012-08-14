//
//  Cam.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Cam.h"

Cam::Cam() {
    camMat.setIdentity();
    fov = 70.0/180.0*M_PI;
    near = 1.0;
    far = 100000.0;
    width = height = 1.0;
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
}

Ray3 Cam::getRayAt(Vector3 screenPos) {
    Ray3 ray(Vector3(0,0,-screenPos.z), Vector3(0,0,-1));
    
    if(fov > 0.0) {
        float aux = tan(fov*0.5);
        ray.direction.x = screenPos.x*aux*(width/height);
        ray.direction.y = -screenPos.y*aux;
        ray.direction.normalize();
        
    }else{
        ray.origin.x = screenPos.x*width;
        ray.origin.y = -screenPos.y*height;
    }
    
    Matrix4 normalMat;
    normalMat.setMatrix3(camMat);
    ray.origin *= camMat;
    ray.direction *= normalMat;
    
    return ray;
}

Frustum3 Cam::getFrustumOf(Vector3 screenMin, Vector3 screenMax) {
    Ray3 rayLT = getRayAt(screenMin),
         rayLB = getRayAt(Vector3(screenMin.x, screenMax.y, 0.0)),
         rayRT = getRayAt(Vector3(screenMax.x, screenMin.y, 0.0)),
         rayRB = getRayAt(screenMax);
    Ray3 front(camMat.pos-camMat.z*near, camMat.z);
    Frustum3 frustumB;
    frustumB.front.set(front);
    frustumB.back.set(camMat.pos-camMat.z*far, camMat.z*-1.0);
    frustumB.left.set(rayLT.origin, rayLT.origin+rayLT.direction, rayLT.origin+rayLB.direction);
    frustumB.right.set(rayRB.origin, rayRB.origin+rayRB.direction, rayRB.origin+rayRT.direction);
    frustumB.bottom.set(rayLB.origin, rayLB.origin+rayLB.direction, rayLB.origin+rayRB.direction);
    frustumB.top.set(rayRT.origin, rayRT.origin+rayRT.direction, rayRT.origin+rayLT.direction);
    
    return frustumB;
}

void Cam::calculate() {
    viewMat = camMat.getInverse();
    
    if(fov > 0.0)
        viewMat.perspective(fov, width/height, near, far);
    else
        viewMat.ortho(width, height, near, far);
    
    frustum = getFrustumOf(Vector3(-1, -1, 0), Vector3(1, 1, 0));
}

void Cam::use() {
    currentCam = this;
    float orientation[] = { camMat.z.x, camMat.z.y, camMat.z.z, camMat.y.x, camMat.y.y, camMat.y.z };
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, camMat.pos.x, camMat.pos.y, camMat.pos.z);
    //alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

Cam *mainCam, *guiCam, *currentCam;