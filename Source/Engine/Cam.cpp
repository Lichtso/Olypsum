//
//  Cam.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <OpenAL/al.h>
#include "LevelManager.h"


//! @cond
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
//! @endcond



Cam::Cam() :fov(70.0/180.0*M_PI), near(1.0), far(100000.0), width(screenSize[0]/2), height(screenSize[1]/2) {
    setTransformation(btTransform::getIdentity());
}

Cam::Cam(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :width(screenSize[0]/2), height(screenSize[1]/2) {
    BaseObject::init(node, levelLoader);
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("fov");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"fov\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &fov);
    attribute = node->first_attribute("near");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"near\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &near);
    attribute = node->first_attribute("far");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"far\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &far);
}

Cam::~Cam() {
    if(currentCam == this)
        currentCam = NULL;
}

void Cam::remove() {
    objectManager.simpleObjects.erase(this);
    BaseObject::remove();
}

Ray3 Cam::getRelativeRayAt(float x, float y) {
    Ray3 ray;
    if(fov > 0.0) {
        float aux = tan(fov*0.5);
        ray.direction.setX(x*aux*(width/height));
        ray.direction.setY(-y*aux);
        ray.direction.setZ(-1.0);
        ray.origin = ray.direction*near;
    }else{
        ray.direction = btVector3(0, 0, -1);
        ray.origin = btVector3(x*width, -y*height, -near);
    }
    return ray;
}

Ray3 Cam::getRayAt(float x, float y) {
    Ray3 ray = getRelativeRayAt(x, y);
    btMatrix3x3 normalMat = transformation.getBasis();
    ray.origin = transformation(ray.origin);
    ray.direction = normalMat*ray.direction;
    return ray;
}

void Cam::doFrustumCulling(short int filterMask) {
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    btVector3* planes_n = new btVector3[6];
    btScalar planes_o[6];
    btVector3 origin = transformation.getOrigin();
    
    planes_n[0] = -transformation.getBasis().getColumn(2);
    planes_n[1] = -planes_n[0];
    planes_o[1] = -planes_n[1].dot(origin+planes_n[0]*far);
    
    if(fov < M_PI) {
        Ray3 rayLT = getRayAt(-1, -1),
             rayLB = getRayAt(-1, 1),
             rayRT = getRayAt(1, -1),
             rayRB = getRayAt(1, 1);
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
            getRayAt(-1, -1), getRayAt(-1, 1),
            getRayAt(1, -1), getRayAt(1, 1)
        };
        
        modelMat.setIdentity();
        FrustumVolume volume(bounds, far-near);
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

bool Cam::gameTick() {
    viewMat = Matrix4(transformation).getInverse();
    
    if(fov == 0.0)
        viewMat.ortho(width, height, near, far);
    else if(fov < M_PI)
        viewMat.perspective(fov, width/height, near, far);
    
    velocity = (transformation.getOrigin()-prevPos)/animationFactor;
    prevPos = transformation.getOrigin();
    return true;
}

void Cam::use() {
    currentCam = this;
}

void Cam::updateAudioListener() {
    btMatrix3x3 mat = transformation.getBasis();
    btVector3 up = mat.getColumn(1), front = mat.getColumn(2), pos = transformation.getOrigin();
    float orientation[] = { front.x(), front.y(), front.z(), up.x(), up.y(), up.z() };
    alListenerf(AL_GAIN, globalVolume);
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
    alListener3f(AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
}

rapidxml::xml_node<xmlUsedCharType>* Cam::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("Cam");
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("fov");
    attribute->value(doc.allocate_string(stringOf(fov).c_str()));
    node->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("near");
    attribute->value(doc.allocate_string(stringOf(near).c_str()));
    node->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("far");
    attribute->value(doc.allocate_string(stringOf(far).c_str()));
    node->append_attribute(attribute);
    return node;
}

Cam *mainCam, *guiCam, *currentCam;