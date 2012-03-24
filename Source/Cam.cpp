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
    width = 1.0;
    height = 0.0;
    viewport[0] = 0;
    viewport[1] = 0;
    viewport[2] = videoInfo->current_w;
    viewport[3] = videoInfo->current_h;
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
}

Ray3 Cam::getRayAt(Vector3 screenPos) {
    Ray3 ray(Vector3(0,0,-screenPos.z), Vector3(0,0,-1));
    
    if(fov > 0.0) {
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
    Ray3 front(camMat.pos+camMat.z*near, camMat.z);
    Frustum3 frustumB;
    frustumB.front.set(front);
    frustumB.back.set(camMat.pos+camMat.z*far, camMat.z*-1);
    frustumB.left.set(front.origin, front.origin+rayLB.direction, front.origin+rayLT.direction);
    frustumB.right.set(front.origin, front.origin+rayRT.direction, front.origin+rayRB.direction);
    frustumB.bottom.set(front.origin, front.origin+rayRB.direction, front.origin+rayLB.direction);
    frustumB.top.set(front.origin, front.origin+rayLT.direction, front.origin+rayRT.direction);
    return frustumB;
}

void Cam::calculate() {
    float aspect = (float)viewport[2]/(float)viewport[3];
    
    viewMat = camMat.getInverse();
    
    Matrix4 projectionMat;
    projectionMat.setIdentity();
    if(fov > 0.0) {
        projectionMat.perspective(fov, aspect, near, far);
    }else{
        projectionMat.ortho(width, height, near, far);
    }
    viewMat *= projectionMat;
    
    shadowMat = viewMat;
    shadowMat.scale(Vector3(0.5, 0.5, 1.0));
    shadowMat.translate(Vector3(0.5, 0.5, 1.0));
    shadowMat.scale((aspect >= 1.0) ? Vector3(1.0, 1.0/aspect, 1.0) : Vector3(aspect, 1.0, 1.0));
    
    frustum = getFrustumOf(Vector3(-1, -1, 0), Vector3(1, 1, 0));
}

void Cam::use() {
    currentCam = this;
    glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);
    /*
    float matData[16];
    viewMat.getOpenGLMatrix4(matData);
    glMatrixMode(GL_PROJECTION);
    glLoadMatrixf(matData);
    glMatrixMode(GL_MODELVIEW);*/
}

Cam *mainCam, *guiCam, *currentCam;
SDL_Surface* screen;
const SDL_VideoInfo* videoInfo;