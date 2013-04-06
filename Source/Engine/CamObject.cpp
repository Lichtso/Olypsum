//
//  CamObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptSimpleObject.h"

//! @cond
struct SingleHitCallback : btDbvt::ICollide {
    bool hit;
    
    SingleHitCallback() : hit(false) { }
    
    void Process(const btDbvtNode* leaf) {
        hit = true;
    }
};

struct FrustumCullingCallback : btDbvt::ICollide {
    short int filterMask;
    
    FrustumCullingCallback(short int _filterMask) : filterMask(_filterMask) {
        
    }
    
    void Process(const btDbvtNode* leaf) {
        btBroadphaseProxy* proxy = static_cast<btBroadphaseProxy*>(leaf->data);
        if((proxy->m_collisionFilterGroup & filterMask) == 0) return;
        
        btCollisionObject* co = static_cast<btCollisionObject*>(proxy->m_clientObject);
        GraphicObject* go = static_cast<GraphicObject*>(co->getUserPointer());
        go->inFrustum = true;
    }
};
//! @endcond



CamObject::CamObject() :fov(90.0/180.0*M_PI), near(1.0), far(10000.0), width(prevOptionsState.videoWidth>>1), height(prevOptionsState.videoHeight>>1) {
    setTransformation(btTransform::getIdentity());
}

CamObject::CamObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader)
    :width(prevOptionsState.videoWidth>>1), height(prevOptionsState.videoHeight>>1) {
    objectManager.simpleObjects.insert(this);
    BaseObject::init(node, levelLoader);
    
    rapidxml::xml_node<xmlUsedCharType>* boundsNode = node->first_node("Bounds");
    if(!boundsNode) {
        log(error_log, "Tried to construct Cam without \"Bounds\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = boundsNode->first_attribute("fov");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"fov\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &fov);
    attribute = boundsNode->first_attribute("near");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"near\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &near);
    attribute = boundsNode->first_attribute("far");
    if(!attribute) {
        log(error_log, "Tried to construct Cam without \"far\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &far);
    
    if(!mainCam) mainCam = this;
}

CamObject::~CamObject() {
    if(mainCam == this) mainCam = NULL;
    if(currentCam == this) currentCam = NULL;
}

void CamObject::remove() {
    objectManager.simpleObjects.erase(this);
    BaseObject::remove();
}

Matrix4 CamObject::getCamMatrix() {
    PlaneReflective* planeReflective = dynamic_cast<PlaneReflective*>(objectManager.currentReflective);
    if(planeReflective) {
        Matrix4 mat = transformation;
        btVector3 translation = planeReflective->plane*planeReflective->plane.w();
        mat.translate(translation);
        mat.reflect(planeReflective->plane);
        mat.translate(translation*-1.0);
        return mat;
    }else
        return Matrix4(transformation);
}

Ray3 CamObject::getRelativeRayAt(float x, float y) {
    Ray3 ray;
    if(fov > 0.0) { //Perspective
        float aux = tan(fov*0.5);
        ray.direction.setX(x*aux*(width/height));
        ray.direction.setY(-y*aux);
        ray.direction.setZ(-1.0);
        ray.origin = ray.direction*near;
    }else{ //Ortho
        ray.direction = btVector3(0, 0, -1);
        ray.origin = btVector3(x*width, -y*height, -near);
    }
    return ray;
}

Ray3 CamObject::getRayAt(float x, float y) {
    Ray3 ray = getRelativeRayAt(x, y);
    ray.origin = transformation(ray.origin);
    ray.direction = transformation.getBasis()*ray.direction;
    return ray;
}

bool CamObject::doFrustumCulling() {
    Matrix4 virtualMat = getCamMatrix();
    btVector3 planes_n[6];
    btScalar planes_o[6];
    planes_n[1] = virtualMat.z;
    planes_o[1] = -planes_n[1].dot(virtualMat.w-planes_n[1]*far);
    planes_n[0] = -planes_n[1];
    
    if(fov < M_PI) {
        planes_o[0] = -planes_n[0].dot(virtualMat.w+planes_n[0]*near);
        PlaneReflective* planeReflective = dynamic_cast<PlaneReflective*>(objectManager.currentReflective);
        
        if(fov > 0.0) { //Perspective
            btMatrix3x3 virtualMatBasis = virtualMat.getBTMatrix3x3();
            Ray3 leftRay = getRelativeRayAt(-1, 0),
                rightRay = getRelativeRayAt(1, 0),
                   upRay = getRelativeRayAt(0, -1),
               bottomRay = getRelativeRayAt(0, 1);
            planes_n[2] = -virtualMat.y.cross(virtualMatBasis*leftRay.direction.normalize()).normalize();
            planes_n[3] =  virtualMat.y.cross(virtualMatBasis*rightRay.direction.normalize()).normalize();
            planes_n[4] = -virtualMat.x.cross(virtualMatBasis*upRay.direction.normalize()).normalize();
            planes_n[5] =  virtualMat.x.cross(virtualMatBasis*bottomRay.direction.normalize()).normalize();
            if(planeReflective)
                for(char i = 2; i < 6; i ++)
                    planes_n[i] *= -1.0;
            planes_o[2] = -planes_n[2].dot(virtualMat.w);
            planes_o[3] = -planes_n[3].dot(virtualMat.w);
            planes_o[4] = -planes_n[4].dot(virtualMat.w);
            planes_o[5] = -planes_n[5].dot(virtualMat.w);
        }else{ //Ortho
            planes_n[2] =  virtualMat.x;
            planes_n[3] = -planes_n[2];
            planes_n[5] =  virtualMat.y;
            planes_n[4] = -planes_n[5];
            if(planeReflective)
                for(char i = 2; i < 6; i ++)
                    planes_n[i] *= -1.0;
            planes_o[2] = -planes_n[2].dot(virtualMat.w-planes_n[2]*width);
            planes_o[3] = -planes_n[3].dot(virtualMat.w-planes_n[3]*width);
            planes_o[4] = -planes_n[4].dot(virtualMat.w-planes_n[4]*height);
            planes_o[5] = -planes_n[5].dot(virtualMat.w-planes_n[5]*height);
        }
        
        if(planeReflective) {
            btTransform objectTransform = objectManager.currentReflective->object->getTransformation();
            btBoxShape* shape = static_cast<btBoxShape*>(objectManager.currentReflective->object->getBody()->getCollisionShape());
            btVector3 points[8];
            for(char i = 0; i < 8; i ++) {
                shape->getVertex(i, points[i]);
                points[i] = objectTransform(points[i]);
            }
            btVector3 axes[4] = {
                 virtualMat.y,
                -virtualMat.y,
                 virtualMat.x,
                -virtualMat.x
            };
            
            if(fov > 0.0) { //Perspective
                for(char plane = 0; plane < 4; plane ++) {
                    int behindFrustumPlane = 0;
                    float bestFactor = 1.0;
                    btVector3 bestNormal(0.0, 0.0, 0.0);
                    for(char i = 0; i < 8; i ++) {
                        if(planes_n[plane+2].dot(points[i]) > planes_o[plane+2]) { //Behind frustum plane
                            behindFrustumPlane ++;
                            continue;
                        }
                        btVector3 normal = axes[plane].cross((points[i]-virtualMat.w).normalized());
                        float factor = planes_n[1].dot(normal);
                        if(factor < bestFactor) { //New best plane
                            bestFactor = factor;
                            bestNormal = normal;
                        }
                    }
                    
                    if(behindFrustumPlane == 0) { //New frustum plane
                        planes_n[plane+2] = bestNormal.normalize();
                        planes_o[plane+2] = -planes_n[plane+2].dot(virtualMat.w);
                    }else if(behindFrustumPlane == 8)
                        return true; //Invalid frustum
                    
                    //if(profiler.isFirstFrameInSec()) //TODO: Debug
                    //    controlsMangager->consoleAdd(stringOf(behindFrustumPlane) + ": " + stringOf(planes_n[plane+2]) +" | "+ stringOf(bestFactor), 0.9);
                }
            }else //Ortho
                for(char plane = 0; plane < 4; plane ++) {
                    int behindFrustumPlane = 0;
                    float bestFactor = ((plane < 2) ? width : height), startFactor = -bestFactor;
                    btVector3 compareAxis = axes[(plane+2)%4];
                    for(char i = 0; i < 8; i ++) {
                        float factor = compareAxis.dot(points[i]-virtualMat.w);
                        if(factor < startFactor) //Behind frustum plane
                            behindFrustumPlane ++;
                        else if(factor < bestFactor) //New best plane
                            bestFactor = factor;
                    }
                    
                    if(behindFrustumPlane == 0) { //New frustum plane
                        planes_o[plane+2] = -planes_n[plane+2].dot(virtualMat.w+planes_n[plane+2]*bestFactor);
                    }else if(behindFrustumPlane == 8)
                        return true; //Invalid frustum
                }
            
            //Set mirror plane as front plane
            planes_n[0] = planeReflective->plane;
            planes_o[0] = -planes_n[0].dot(planeReflective->plane*planeReflective->plane.w());
        }
    }else{
        if(abs(fov-M_PI) < 0.001)
            planes_o[0] = -planes_n[0].dot(virtualMat.w);
        else
            planes_o[0] = -planes_n[0].dot(virtualMat.w-planes_n[0]*far);
        planes_n[2] = virtualMat.x;
        planes_o[2] = -planes_n[2].dot(virtualMat.w-planes_n[2]*far);
        planes_n[3] = -planes_n[2];
        planes_o[3] = -planes_n[3].dot(virtualMat.w-planes_n[3]*far);
        planes_n[4] = virtualMat.y;
        planes_o[4] = -planes_n[4].dot(virtualMat.w-planes_n[4]*far);
        planes_n[5] = -planes_n[4];
        planes_o[5] = -planes_n[5].dot(virtualMat.w-planes_n[5]*far);
    }
    
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
    
    FrustumCullingCallback fCC(filterMask);
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    btDbvt::collideKDOP(broadphase->m_sets[0].m_root, planes_n, planes_o, 6, fCC);
    btDbvt::collideKDOP(broadphase->m_sets[1].m_root, planes_n, planes_o, 6, fCC);
    return false;
}

bool CamObject::testInverseNearPlaneHit(btDbvtProxy* node) {
    Matrix4 virtualMat = getCamMatrix();
    btVector3 planes_n[1];
    btScalar planes_o[1];
    
    planes_n[0] = (dynamic_cast<PlaneReflective*>(objectManager.currentReflective)) ? -virtualMat.z : virtualMat.z;
    if(fov < M_PI)
        planes_o[0] = -planes_n[0].dot(virtualMat.w-planes_n[0]*near);
    else
        planes_o[0] = -planes_n[0].dot(virtualMat.w);
    
    SingleHitCallback fCC;
    btDbvt::collideKDOP(node->leaf, planes_n, planes_o, 1, fCC);
    return fCC.hit;
}

void CamObject::drawDebugFrustum(Color4 color) {
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

bool CamObject::gameTick() {
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

void CamObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptCamObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
}

void CamObject::use() {
    currentCam = this;
}

void CamObject::updateViewMat() {
    viewMat = getCamMatrix().getInverse();
    
    if(fov == 0.0) //Ortho
        viewMat.ortho(width, height, near, far);
    else if(fov < M_PI) //Perspective
        viewMat.perspective(fov, width/height, near, far);
}

rapidxml::xml_node<xmlUsedCharType>* CamObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("CamObject");
    
    rapidxml::xml_node<xmlUsedCharType>* boundsNode = doc.allocate_node(rapidxml::node_element);
    boundsNode->name("Bounds");
    node->append_node(boundsNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("fov");
    attribute->value(doc.allocate_string(stringOf(fov).c_str()));
    boundsNode->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("near");
    attribute->value(doc.allocate_string(stringOf(near).c_str()));
    boundsNode->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("far");
    attribute->value(doc.allocate_string(stringOf(far).c_str()));
    boundsNode->append_attribute(attribute);
    return node;
}

CamObject *mainCam, *guiCam, *currentCam;