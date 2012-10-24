//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

ZoneObject::ZoneObject(btCollisionShape* shape, const btTransform& transform) {
    body = new btCollisionObject();
    body->setCollisionShape(shape);
    body->setWorldTransform(transform);
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(body, CollisionMask_Zone, CollisionMask_Object);
}

ZoneObject::~ZoneObject() {
    worldManager.physicsWorld->removeCollisionObject(body);
    delete body;
}

btTransform ZoneObject::getTransformation() {
    return body->getWorldTransform();
}



RigidObject::RigidObject(std::shared_ptr<Model> modelB, btRigidBody::btRigidBodyConstructionInfo& rBCI) :ModelObject(modelB) {
    rBCI.m_collisionShape->calculateLocalInertia(rBCI.m_mass, rBCI.m_localInertia);
    body = new btRigidBody(rBCI);
    body->setUserPointer(this);
    if(rBCI.m_mass == 0.0)
        worldManager.physicsWorld->addRigidBody(body, CollisionMask_Static, CollisionMask_Frustum | CollisionMask_Object);
    else
        worldManager.physicsWorld->addRigidBody(body, CollisionMask_Object, CollisionMask_Frustum | CollisionMask_Zone | CollisionMask_Static | CollisionMask_Object);
}

RigidObject::~RigidObject() {
    if(body) {
        while(body->getNumConstraintRefs() > 0) {
            btTypedConstraint* constraint = body->getConstraintRef(0);
            worldManager.physicsWorld->removeConstraint(constraint);
            delete constraint;
        }
        delete body->getMotionState();
        worldManager.physicsWorld->removeRigidBody(body);
        delete body;
    }
}

btTransform RigidObject::getTransformation() {
    btDefaultMotionState* motionState = static_cast<btDefaultMotionState*>(body->getMotionState());
    return motionState->m_graphicsWorldTrans;
}

void RigidObject::draw() {
    //TODO: DEBUG
    static float rotY;
    rotY -= 0.01;
    skeletonPose->bonePoses["Thigh_Right"].setIdentity();
    skeletonPose->bonePoses["Thigh_Right"].setRotation(btQuaternion(0, 0, rotY));
    skeletonPose->bonePoses["Shin_Right"].setIdentity();
    skeletonPose->bonePoses["Shin_Right"].setRotation(btQuaternion(0, 0, rotY*2.0));
    skeletonPose->calculate();
    skeletonPose->draw(0.1, 0.0, 0.02);
    
    //if(GraphicObject::isInFrustum(body))
        ModelObject::draw();
}



WaterObject::WaterObject(std::shared_ptr<Model> modelB, btCollisionShape* shapeB, const btTransform& transformB) :ModelObject(modelB), ZoneObject(shapeB, transformB), waveSpeed(0.05) {
    
}

WaterObject::~WaterObject() {
    
}

void WaterObject::addWave(float maxAge, float ampitude, float length, float originX, float originY) {
    if(waves.size() >= MAX_WAVES) return;
    WaterObjectWave wave;
    wave.age = 0.0;
    wave.maxAge = maxAge;
    wave.ampitude = ampitude;
    wave.length = length;
    wave.originX = originX;
    wave.originY = originY;
    waves.push_back(wave);
}

void WaterObject::gameTick() {
    for(unsigned int i = 0; i < waves.size(); i ++) {
        waves[i].age += worldManager.animationFactor;
        if(waves[i].age >= waves[i].maxAge) {
            waves.erase(waves.begin()+i);
            i --;
        }
    }
}

void WaterObject::prepareShaderProgram(Mesh* mesh) {
    shaderPrograms[waterSP]->use();
    char str[64];
    for(unsigned int i = 0; i < MAX_WAVES; i ++) {
        bool active = (i < waves.size());
        sprintf(str, "waveLen[%d]", i);
        currentShaderProgram->setUniformF(str, active ? (1.0/waves[i].length) : 0.0);
        sprintf(str, "waveAge[%d]", i);
        currentShaderProgram->setUniformF(str, active ? waves[i].age/waves[i].length*waveSpeed-M_PI*2.0 : 0.0);
        sprintf(str, "waveAmp[%d]", i);
        currentShaderProgram->setUniformF(str, active ? waves[i].ampitude*(1.0-waves[i].age/waves[i].maxAge) : 0.0);
        sprintf(str, "waveOri[%d]", i);
        if(active)
            currentShaderProgram->setUniformVec2(str, waves[i].originX, waves[i].originY);
        else
            currentShaderProgram->setUniformVec2(str, 0.0, 0.0);
    }
    currentShaderProgram->setUniformF("discardDensity", 1.0);
}

void WaterObject::draw() {
    if(!lightManager.currentShadowLight && GraphicObject::isInFrustum(body))
        ModelObject::draw();
}