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
        GLint viewport[4];
        glGetIntegerv(GL_VIEWPORT, viewport);
        float aux = tan(fov*0.5);
        ray.direction.x = screenPos.x*aux*(float)viewport[2]/(float)viewport[3];
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
    Ray3 front(camMat.pos+camMat.z*near, camMat.z*-1);
    Frustum3 frustumB;
    frustumB.front.set(front);
    frustumB.back.set(camMat.pos+camMat.z*far, camMat.z);
    frustumB.left.set(front.origin, front.origin+rayLT.direction, front.origin+rayLB.direction);
    frustumB.right.set(front.origin, front.origin+rayRB.direction, front.origin+rayRT.direction);
    frustumB.bottom.set(front.origin, front.origin+rayLB.direction, front.origin+rayRB.direction);
    frustumB.top.set(front.origin, front.origin+rayRT.direction, front.origin+rayLT.direction);
    return frustumB;
}

void Cam::calculate() {
    float aspect = width/height;
    viewMat = camMat.getInverse();
    
    if(fov > 0.0)
        viewMat.perspective(fov, aspect, near, far);
    else
        viewMat.ortho(width, height, near, far);
    
    shadowMat = viewMat;
    shadowMat.scale(Vector3(0.5, 0.5, 1.0));
    shadowMat.translate(Vector3(0.5, 0.5, 0.0));
    //shadowMat.scale((aspect >= 1.0) ? Vector3(1.0, 1.0/aspect, 1.0) : Vector3(aspect, 1.0, 1.0));
    
    frustum = getFrustumOf(Vector3(-1, -1, 0), Vector3(1, 1, 0));
}

void Cam::use() {
    currentCam = this;
    float orientation[] = { camMat.z.x, camMat.z.y, camMat.z.z, camMat.y.x, camMat.y.y, camMat.y.z };
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, camMat.pos.x, camMat.pos.y, camMat.pos.z);
    //alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);
}

void Cam::setFullScreen() {
    glViewport(0, 0, videoInfo->current_w, videoInfo->current_h);
}

Cam *mainCam, *guiCam, *currentCam;
SDL_Surface* screen;
const SDL_VideoInfo* videoInfo;