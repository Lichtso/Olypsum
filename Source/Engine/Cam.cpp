//
//  Cam.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenAL/al.h>
#import "WorldManager.h"

Cam::Cam() :fov(70.0/180.0*M_PI), near(1.0), far(100000.0), width(screenSize[0]/2), height(screenSize[1]/2), frustumShape(NULL) {
    camMat.setIdentity();
    
    frustumBody = new btCollisionObject();
    frustumBody->setCollisionFlags(frustumBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE | btCollisionObject::CF_KINEMATIC_OBJECT);
    frustumBody->setActivationState(DISABLE_DEACTIVATION);
    calculateFrustum(btVector3(-1, -1, 0), btVector3(1, 1, 0));
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
    if(frustumShape)
        delete frustumShape;
    delete frustumBody;
}

Ray3 Cam::getRayAt(btVector3 screenPos, bool absolute) {
    Ray3 ray;
    
    if(fov > 0.0) {
        float aux = tan(fov*0.5);
        ray.direction.setX(screenPos.x()*aux*(width/height));
        ray.direction.setY(-screenPos.y()*aux);
        ray.direction.normalize();
        ray.origin = ray.direction*-screenPos.z();
    }else{
        ray.direction = btVector3(0, 0, -1);
        ray.origin = btVector3(screenPos.x()*width, -screenPos.y()*height, -screenPos.z());
    }
    
    if(absolute) {
        btMatrix3x3 normalMat = camMat.getBasis();
        ray.origin = ray.origin*normalMat+camMat.getOrigin();
        ray.direction = ray.direction*normalMat;
    }
    return ray;
}

void Cam::calculateFrustum(btVector3 screenMin, btVector3 screenMax) {
    if(frustumShape) delete frustumShape;
    if(fov > 0.0) {
        Ray3 rayLT = getRayAt(btVector3(screenMin.x(), screenMin.y(), near), false),
             rayLB = getRayAt(btVector3(screenMin.x(), screenMax.y(), near), false),
             rayRT = getRayAt(btVector3(screenMax.x(), screenMin.y(), near), false),
             rayRB = getRayAt(btVector3(screenMax.x(), screenMax.y(), near), false);
        btVector3 vertices[5];
        vertices[0] = btVector3(0, 0, 0);
        vertices[1] = rayLT.origin+rayLT.direction*far;
        vertices[2] = rayLB.origin+rayLB.direction*far;
        vertices[3] = rayRT.origin+rayRT.direction*far;
        vertices[4] = rayRB.origin+rayRB.direction*far;
        frustumBody->setCollisionShape(new btConvexHullShape((btScalar*)vertices, 5));
        frustumBody->setWorldTransform(camMat);
    }else if(fov < M_PI) {
        screenMin.setZ(near);
        screenMax.setZ(far);
        btVector3 halfSize = (screenMax-screenMin)*0.5;
        frustumBody->setCollisionShape(new btBoxShape(halfSize));
        btVector3 shift(halfSize.x()*0.5, halfSize.y()*0.5, -near-halfSize.z());
        shift = shift*camMat.getBasis();
        btTransform frustumMat = camMat;
        frustumMat.setOrigin(shift+camMat.getOrigin());
        frustumBody->setWorldTransform(frustumMat);
    }else{
        LightParabolidVolume volume(far, 10, 4);
        unsigned int verticesCount;
        std::unique_ptr<btScalar[]> vertices = volume.getVertices(verticesCount);
        frustumBody->setCollisionShape(new btConvexHullShape(vertices.get(), verticesCount, sizeof(btScalar)*3));
        frustumBody->setWorldTransform(camMat);
    }
}

void Cam::calculate() {
    viewMat = Matrix4(camMat).getInverse();
    
    if(fov > 0.0) {
        viewMat.perspective(fov, width/height, near, far);
        frustumBody->setWorldTransform(camMat);
    }else if(fov < M_PI) {
        viewMat.ortho(width, height, near, far);
        btVector3 shift(0.0, 0.0, -near-far*0.5);
        shift = shift*camMat.getBasis();
        btTransform frustumMat = camMat;
        frustumMat.setOrigin(shift+camMat.getOrigin());
        frustumBody->setWorldTransform(frustumMat);
    }else{
        frustumBody->setWorldTransform(camMat);
    }
    
    velocity = camMat.getOrigin()-prevPos;
    prevPos = camMat.getOrigin();
}

void Cam::use() {
    currentCam = this;
    btMatrix3x3 mat = camMat.getBasis();
    btVector3 up = mat.getRow(1), front = mat.getRow(2), pos = camMat.getOrigin();
    float orientation[] = { front.x(), front.y(), front.z(), up.x(), up.y(), up.z() };
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
    alListener3f(AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
}

Cam *mainCam, *guiCam, *currentCam;