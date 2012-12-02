//
//  DisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

GraphicObject::GraphicObject() {
    objectManager.graphicObjects.insert(this);
}

void GraphicObject::remove() {
    objectManager.graphicObjects.erase(this);
    delete this;
}



ModelObject::ModelObject(std::shared_ptr<Model> modelB) :model(modelB) {
    
}

void ModelObject::draw() {
    modelMat = getTransformation();
    model->draw(this);
}

void ModelObject::drawAccumulatedMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

void ModelObject::prepareShaderProgram(Mesh* mesh) {
    if(!objectManager.currentShadowLight) {
        char index = (mesh->heightMap) ? 1 : 0;
        if(model->skeleton) index |= 2;
        if(mesh->transparent && blendingQuality > 0) index |= 4;
        ShaderProgramNames lookup[] = {
            solidGeometrySP, solidBumpGeometrySP, skeletalGeometrySP, skeletalBumpGeometrySP,
            glassGeometrySP, glassBumpGeometrySP, glassSkeletalGeometrySP, glassSkeletalBumpGeometrySP
        };
        shaderPrograms[lookup[index]]->use();
    }
    
    currentShaderProgram->setUniformF("discardDensity", 1.0);
    //if(model->skeleton) TODO
    //    currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose->mats, skeletonPose->skeleton->bones.size());
}



void simpleMotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const {
    centerOfMassWorldTrans = transformation;
}

void simpleMotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans) {
    transformation = centerOfMassWorldTrans;
}



void comMotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const {
    centerOfMassWorldTrans = centerOfMassOffset.inverse() * transformation;
}

void comMotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans) {
    transformation = centerOfMassWorldTrans * centerOfMassOffset;
}



RigidObject::RigidObject(std::shared_ptr<Model> modelB, btRigidBody::btRigidBodyConstructionInfo& rBCI) :ModelObject(modelB) {
    rBCI.m_collisionShape->calculateLocalInertia(rBCI.m_mass, rBCI.m_localInertia);
    body = new btRigidBody(rBCI);
    body->setUserPointer(this);
    btRigidBody* body = getBody();
    if(rBCI.m_mass == 0.0)
        worldManager.physicsWorld->addRigidBody(body, CollisionMask_Static, CollisionMask_Object);
    else
        worldManager.physicsWorld->addRigidBody(body, CollisionMask_Object, CollisionMask_Zone | CollisionMask_Static | CollisionMask_Object);
}

RigidObject::~RigidObject() {
    if(body) {
        btRigidBody* rigidBody = getBody();
        while(rigidBody->getNumConstraintRefs() > 0) {
            btTypedConstraint* constraint = rigidBody->getConstraintRef(0);
            worldManager.physicsWorld->removeConstraint(constraint);
            delete constraint;
        }
        delete rigidBody->getMotionState();
        worldManager.physicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        body = NULL;
    }
}

void RigidObject::setTransformation(const btTransform& transformation) {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    motionState->transformation = transformation;
}

btTransform RigidObject::getTransformation() {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    return motionState->transformation;
}

void RigidObject::draw() {
    modelMat = getTransformation();
    /*
    //TODO: DEBUG
    static float rotY;
    rotY -= 0.01;
    skeletonPose->bonePoses["Back"].setIdentity();
    skeletonPose->bonePoses["Back"].setOrigin(btVector3(0, -0.9, 0));
    //skeletonPose->bonePoses["Back"].setRotation(btQuaternion(-1, 0, 0));
    skeletonPose->bonePoses["Thigh_Right"].setIdentity();
    skeletonPose->bonePoses["Thigh_Right"].setRotation(btQuaternion(0, 0, rotY));
    skeletonPose->bonePoses["Shin_Right"].setIdentity();
    skeletonPose->bonePoses["Shin_Right"].setRotation(btQuaternion(0, 0, rotY*2.0));
    skeletonPose->needsUpdate = true;
    //skeletonPose->draw(0.1, 0.0, 0.02);
    */
    LightBoxVolume bV(btVector3(1.0, 1.0, 0.2));
    bV.init();
    bV.drawWireFrame(Color4(1.0, 1.0, 0.0, 1.0));
    
    ModelObject::draw();
}


WaterObject::WaterObject(std::shared_ptr<Model> modelB, btCollisionShape* shapeB, const btTransform& transformB) :ModelObject(modelB), waveSpeed(0.05) {
    
}

WaterObject::~WaterObject() {
    
}

void WaterObject::addWave(float maxAge, float ampitude, float length, float originX, float originY) {
    if(waves.size() >= MAX_WAVES) return;
    Wave wave;
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
    if(!objectManager.currentShadowLight)
        ModelObject::draw();
}