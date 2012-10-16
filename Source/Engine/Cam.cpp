//
//  Cam.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenAL/al.h>
#import "WorldManager.h"

Cam::Cam() :fov(70.0/180.0*M_PI), near(1.0), far(100000.0), width(1.0), height(1.0), frustumShape(NULL) {
    camMat.setIdentity();
    
    frustumBody = new btCollisionObject();
    frustumBody->setCollisionFlags(frustumBody->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(frustumBody, CollisionMask_Frustum, CollisionMask_Object);
    setFrustum(btVector3(-1, -1, 0), btVector3(1, 1, 0));
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
    if(frustumShape)
        delete frustumShape;
    delete frustumBody;
}

Ray3 Cam::getRayAt(btVector3 screenPos) {
    Ray3 ray(btVector3(0, 0, -screenPos.z()), btVector3(0, 0, -1));
    
    if(fov > 0.0) {
        float aux = tan(fov*0.5);
        ray.direction.setX(screenPos.x()*aux*(width/height));
        ray.direction.setY(-screenPos.y()*aux);
        ray.direction.normalize();
        
    }else{
        ray.origin.setX(screenPos.x()*width);
        ray.origin.setY(-screenPos.y()*height);
    }
    
    btMatrix3x3 normalMat = camMat.getBasis();
    ray.origin += camMat.getOrigin();
    ray.direction = ray.direction*normalMat;
    
    return ray;
}

void Cam::setFrustum(btVector3 screenMin, btVector3 screenMax) {
    if(fov > 0.0) {
        Ray3 rayLT = getRayAt(screenMin),
             rayLB = getRayAt(btVector3(screenMin.x(), screenMax.y(), 0.0)),
             rayRT = getRayAt(btVector3(screenMax.x(), screenMin.y(), 0.0)),
             rayRB = getRayAt(screenMax);
        btConvexHullShape* shape = dynamic_cast<btConvexHullShape*>(frustumShape);
        if(!shape) {
            if(frustumShape) delete frustumShape;
            btScalar vertices[5*3];
            vertices[0] = 0.0;
            vertices[1] = 0.0;
            vertices[2] = 0.0;
            frustumShape = shape = new btConvexHullShape(vertices, 5, sizeof(btScalar)*3);
            frustumBody->setCollisionShape(frustumShape);
        }
        btVector3* vertices = shape->getUnscaledPoints();
        vertices[1] = rayLT.origin+rayLT.direction*far;
        vertices[2] = rayLB.origin+rayLB.direction*far;
        vertices[3] = rayRT.origin+rayRT.direction*far;
        vertices[4] = rayRB.origin+rayRB.direction*far;
        frustumBody->setWorldTransform(camMat);
    }else{
        if(frustumShape) delete frustumShape;
        screenMin.setZ(near);
        screenMax.setZ(far);
        btVector3 halfSize = (screenMax-screenMin)*0.5;
        frustumShape = new btBoxShape(halfSize);
        frustumBody->setCollisionShape(frustumShape);
        btVector3 shift(halfSize.x()*0.5, halfSize.y()*0.5, -near-halfSize.z());
        shift = shift*camMat.getBasis();
        btTransform frustumMat = camMat;
        frustumMat.setOrigin(shift+camMat.getOrigin());
        frustumBody->setWorldTransform(frustumMat);
    }
}

void Cam::calculate() {
    viewMat = camMat.inverse();
    
    /*if(fov > 0.0)
        viewMat.perspective(fov, width/height, near, far);
    else
        viewMat.ortho(width, height, near, far);*/
    
    setFrustum(btVector3(-1, -1, 0), btVector3(1, 1, 0));
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