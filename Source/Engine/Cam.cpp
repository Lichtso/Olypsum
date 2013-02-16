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

Matrix4 Cam::getCamMatrix() {
    /*if(objectManager.currentReflective) {
        Matrix4 mat = transformation;
        btVector3 translation = objectManager.currentReflective->normal*objectManager.currentReflective->distance;
        mat.translate(translation*-1.0);
        mat.reflect(objectManager.currentReflective->normal);
        mat.translate(translation);
        return mat;
    }else*/
        return Matrix4(transformation);
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

void Cam::doFrustumCulling() {
    short int filterMask = CollisionMask_Static | CollisionMask_Object;
    
    for(auto graphicObject : objectManager.graphicObjects)
        graphicObject->inFrustum = false;
    
    if(!objectManager.currentShadowLight) {
        for(auto particlesObject : objectManager.particlesObjects)
            particlesObject->inFrustum = false;
        
        for(unsigned int i = 0; i < objectManager.lightObjects.size(); i ++)
            objectManager.lightObjects[i]->inFrustum = false;
        
        filterMask |= CollisionMask_Light;
    }
    
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    btVector3* planes_n = new btVector3[6];
    btScalar planes_o[6];
    btTransform mat = getCamMatrix().getBTMatrix();
    
    planes_n[1] = mat.getBasis().getColumn(2);
    planes_o[1] = -planes_n[1].dot(mat.getOrigin()-planes_n[1]*far);
    planes_n[0] = -planes_n[1];
    
    if(fov < M_PI) {
        Ray3 rayLT = getRayAt(-1, -1),
             rayLB = getRayAt(-1, 1),
             rayRT = getRayAt(1, -1),
             rayRB = getRayAt(1, 1);
        planes_o[0] = -planes_n[0].dot(mat.getOrigin()+planes_n[0]*near);
        planes_n[2] = (rayLB.direction).cross(rayLT.direction).normalize();
        planes_o[2] = -planes_n[2].dot(rayLB.origin);
        planes_n[3] = (rayRT.direction).cross(rayRB.direction).normalize();
        planes_o[3] = -planes_n[3].dot(rayRT.origin);
        planes_n[4] = (rayLT.direction).cross(rayRT.direction).normalize();
        planes_o[4] = -planes_n[4].dot(rayRT.origin);
        planes_n[5] = (rayRB.direction).cross(rayLB.direction).normalize();
        planes_o[5] = -planes_n[5].dot(rayLB.origin);
        /*
        if(objectManager.currentReflective) {
            planes_n[0] = objectManager.currentReflective->normal;
            planes_o[0] = -planes_n[0].dot(objectManager.currentReflective->normal*objectManager.currentReflective->distance);
            btDbvtProxy* proxy = static_cast<btDbvtProxy*>(objectManager.currentReflective->object->getBody()->getBroadphaseHandle());
            btVector3 points[8] = {
                btVector3(proxy->m_aabbMin.x(), proxy->m_aabbMin.y(), proxy->m_aabbMin.z()),
                btVector3(proxy->m_aabbMax.x(), proxy->m_aabbMin.y(), proxy->m_aabbMin.z()),
                btVector3(proxy->m_aabbMin.x(), proxy->m_aabbMax.y(), proxy->m_aabbMin.z()),
                btVector3(proxy->m_aabbMax.x(), proxy->m_aabbMax.y(), proxy->m_aabbMin.z()),
                btVector3(proxy->m_aabbMin.x(), proxy->m_aabbMin.y(), proxy->m_aabbMax.z()),
                btVector3(proxy->m_aabbMax.x(), proxy->m_aabbMin.y(), proxy->m_aabbMax.z()),
                btVector3(proxy->m_aabbMin.x(), proxy->m_aabbMax.y(), proxy->m_aabbMax.z()),
                btVector3(proxy->m_aabbMax.x(), proxy->m_aabbMax.y(), proxy->m_aabbMax.z())
            };
            btVector3 axes[4] = {
                -mat.getBasis().getColumn(1),
                mat.getBasis().getColumn(1),
                -mat.getBasis().getColumn(0),
                mat.getBasis().getColumn(0)
            };
            for(char plane = 0; plane < 4; plane ++) {
                float bestFactor = -2.0;
                planes_n[plane+2] = btVector3(0.0, 0.0, 0.0);
                for(char i = 0; i < 8; i ++) {
                    btVector3 normal = axes[plane].cross(points[i]-mat.getOrigin()).normalized();
                    float factor = axes[(plane+2)%4].dot(normal);
                    if(factor > bestFactor) {
                        bestFactor = factor;
                        planes_n[plane+2] = normal;
                    }
                }
                planes_o[plane+2] = -planes_n[plane+2].dot(mat.getOrigin());
            }
        }*/
    }else{
        if(abs(fov-M_PI) < 0.001)
            planes_o[0] = -planes_n[0].dot(mat.getOrigin());
        else
            planes_o[0] = -planes_n[0].dot(mat.getOrigin()-planes_n[0]*far);
        planes_n[2] = mat.getBasis().getColumn(0);
        planes_o[2] = -planes_n[2].dot(mat.getOrigin()-planes_n[2]*far);
        planes_n[3] = -planes_n[2];
        planes_o[3] = -planes_n[3].dot(mat.getOrigin()-planes_n[3]*far);
        planes_n[4] = mat.getBasis().getColumn(1);
        planes_o[4] = -planes_n[4].dot(mat.getOrigin()-planes_n[4]*far);
        planes_n[5] = -planes_n[4];
        planes_o[5] = -planes_n[5].dot(mat.getOrigin()-planes_n[5]*far);
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

void Cam::drawDebugFrustum(Color4 color) {
    if(fov < M_PI) {
        Ray3 bounds[] = {
            getRayAt(-1, -1), getRayAt(-1, 1),
            getRayAt(1, -1), getRayAt(1, 1)
        };
        
        modelMat.setIdentity();
        FrustumVolume volume(bounds, far-near);
        volume.init();
        volume.drawDebug(color);
    }else if(abs(fov-M_PI) < 0.001) {
        modelMat = transformation;
        modelMat.setOrigin(modelMat.getOrigin()-transformation.getBasis().getColumn(2)*far*0.5);
        LightBoxVolume volume(btVector3(far, far, far*0.5));
        volume.init();
        volume.drawDebug(color);
    }else{
        modelMat = transformation;
        LightBoxVolume volume(btVector3(far, far, far));
        volume.init();
        volume.drawDebug(color);
    }
}

bool Cam::gameTick() {
    velocity = (transformation.getOrigin()-prevPos)/profiler.animationFactor;
    prevPos = transformation.getOrigin();
    btMatrix3x3 mat = transformation.getBasis();
    btVector3 up = mat.getColumn(1), front = mat.getColumn(2), pos = transformation.getOrigin();
    float orientation[] = { front.x(), front.y(), front.z(), up.x(), up.y(), up.z() };
    alListenerf(AL_GAIN, optionsState.globalVolume);
    alListenerfv(AL_ORIENTATION, orientation);
    alListener3f(AL_POSITION, pos.x(), pos.y(), pos.z());
    alListener3f(AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
    return true;
}

void Cam::use() {
    currentCam = this;
}

void Cam::updateFrustum() {
    viewMat = getCamMatrix().getInverse();
    
    if(fov == 0.0)
        viewMat.ortho(width, height, near, far);
    else if(fov < M_PI)
        viewMat.perspective(fov, width/height, near, far);
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