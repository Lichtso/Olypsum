//
//  Cam.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenAL/al.h>
#import "WorldManager.h"

struct PlaneCullingCallback : btDbvt::ICollide {
    bool hit;
    
    PlaneCullingCallback() : hit(false) { }
    
    void Process(const btDbvtNode* leaf) {
        hit = true;
    }
};

struct FrustumCullingCallback : btDbvt::ICollide {
    short int m_collisionFilterMask;
    
    FrustumCullingCallback(short int filterMask) : m_collisionFilterMask(filterMask) {
        
    }
    
    void Process(const btDbvtNode* leaf) {
        btBroadphaseProxy* proxy = static_cast<btBroadphaseProxy*>(leaf->data);
        if((proxy->m_collisionFilterGroup & m_collisionFilterMask) == 0) return;
        
        btCollisionObject* co = static_cast<btCollisionObject*>(proxy->m_clientObject);
        GraphicObject* go = static_cast<GraphicObject*>(co->getUserPointer());
        go->inFrustum = true;
    }
};



Cam::Cam() :fov(70.0/180.0*M_PI), near(1.0), far(100000.0), width(screenSize[0]/2), height(screenSize[1]/2) {
    setTransformation(btTransform::getIdentity());
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
}

Ray3 Cam::getRayAt(btVector3 screenPos) {
    Ray3 ray;
    
    if(fov > 0.0) {
        float aux = tan(fov*0.5);
        ray.direction.setX(screenPos.x()*aux*(width/height));
        ray.direction.setY(-screenPos.y()*aux);
        ray.direction.setZ(-1.0);
        ray.origin = ray.direction*near;
    }else{
        ray.direction = btVector3(0, 0, -1);
        ray.origin = btVector3(screenPos.x()*width, -screenPos.y()*height, -near);
    }
    
    btMatrix3x3 normalMat = transformation.getBasis();
    ray.origin = transformation(ray.origin);
    ray.direction = normalMat*ray.direction;
    return ray;
}

void Cam::doFrustumCulling(short int filterMask) {
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(worldManager.broadphase);
    btVector3* planes_n = new btVector3[6];
    btScalar planes_o[6];
    btVector3 origin = transformation.getOrigin();
    
    planes_n[0] = -transformation.getBasis().getColumn(2);
    planes_n[1] = -planes_n[0];
    planes_o[1] = -planes_n[1].dot(origin+planes_n[0]*far);
    
    if(fov < M_PI) {
        Ray3 rayLT = getRayAt(btVector3(-1, -1, near)),
             rayLB = getRayAt(btVector3(-1, 1, near)),
             rayRT = getRayAt(btVector3(1, -1, near)),
             rayRB = getRayAt(btVector3(1, 1, near));
        planes_o[0] = -planes_n[0].dot(origin+planes_n[0]*near);
        planes_n[2] = (rayLB.direction).cross(rayLT.direction).normalize();
        planes_o[2] = -planes_n[2].dot(rayLB.origin);
        planes_n[3] = (rayRT.direction).cross(rayRB.direction).normalize();
        planes_o[3] = -planes_n[3].dot(rayRT.origin);
        planes_n[4] = (rayLT.direction).cross(rayRT.direction).normalize();
        planes_o[4] = -planes_n[4].dot(rayRT.origin);
        planes_n[5] = (rayRB.direction).cross(rayLB.direction).normalize();
        planes_o[5] = -planes_n[5].dot(rayLB.origin);
    }else{
        if(abs(fov-M_PI) < 0.001)
            planes_o[0] = -planes_n[0].dot(origin);
        else
            planes_o[0] = -planes_n[0].dot(origin-planes_n[0]*far);
        planes_n[2] = transformation.getBasis().getColumn(0);
        planes_o[2] = -planes_n[2].dot(origin-planes_n[2]*far);
        planes_n[3] = -planes_n[2];
        planes_o[3] = -planes_n[3].dot(origin-planes_n[3]*far);
        planes_n[4] = transformation.getBasis().getColumn(1);
        planes_o[4] = -planes_n[4].dot(origin-planes_n[4]*far);
        planes_n[5] = -planes_n[4];
        planes_o[5] = -planes_n[5].dot(origin-planes_n[5]*far);
    }
    
    FrustumCullingCallback fCC(filterMask);
    btDbvt::collideKDOP(broadphase->m_sets[0].m_root, planes_n, planes_o, 6, fCC);
    btDbvt::collideKDOP(broadphase->m_sets[1].m_root, planes_n, planes_o, 6, fCC);
    delete [] planes_n;
}

bool Cam::testInverseNearPlaneHit(btDbvtProxy* node) {
    btVector3 planes_n[1];
    btScalar planes_o[1];
    
    planes_n[0] = transformation.getBasis().getColumn(2);
    if(fov < M_PI)
        planes_o[0] = -planes_n[0].dot(transformation.getOrigin()-planes_n[0]*near);
    else
        planes_o[0] = -planes_n[0].dot(transformation.getOrigin());
    
    PlaneCullingCallback fCC;
    btDbvt::collideKDOP(node->leaf, planes_n, planes_o, 1, fCC);
    return fCC.hit;
}

void Cam::drawWireframeFrustum(Color4 color) {
    if(fov < M_PI) {
        Ray3 bounds[] = {
            getRayAt(btVector3(-1, -1, near)), getRayAt(btVector3(-1, 1, near)),
            getRayAt(btVector3(1, -1, near)), getRayAt(btVector3(1, 1, near))
        };
        modelMat.setIdentity();
        FrustumVolume volume(bounds, far);
        volume.init();
        volume.drawWireFrame(color);
    }else if(abs(fov-M_PI) < 0.001) {
        modelMat = transformation;
        modelMat.setOrigin(modelMat.getOrigin()-transformation.getBasis().getColumn(2)*far*0.5);
        LightBoxVolume volume(btVector3(far, far, far*0.5));
        volume.init();
        volume.drawWireFrame(color);
    }else{
        modelMat = transformation;
        LightBoxVolume volume(btVector3(far, far, far));
        volume.init();
        volume.drawWireFrame(color);
    }
}

void Cam::gameTick() {
    viewMat = Matrix4(transformation).getInverse();
    
    if(fov > 0.0) {
        viewMat.perspective(fov, width/height, near, far);
    }else if(fov == 0.0) {
        viewMat.ortho(width, height, near, far);
    }
    
    velocity = transformation.getOrigin()-prevPos;
    prevPos = transformation.getOrigin();
}

void Cam::use() {
    currentCam = this;
    btMatrix3x3 mat = transformation.getBasis();
    btVector3 up = mat.getColumn(1), front = mat.getColumn(2), pos = transformation.getOrigin();
    float orientation[] = { front.x(), front.y(), front.z(), up.x(), up.y(), up.z() };
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
    alListener3f(AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
}

Cam *mainCam, *guiCam, *currentCam;