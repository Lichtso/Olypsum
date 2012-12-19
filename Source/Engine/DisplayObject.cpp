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
    BaseObject::remove();
}



ModelObject::ModelObject(std::shared_ptr<Model> modelB) :model(modelB), skeletonPose(NULL) {
    if(model->skeleton) {
        skeletonPose = new btTransform[model->skeleton->bones.size()];
        setupModelObjectBones(this, model->skeleton->rootBone);
    }
}

void ModelObject::setupModelObjectBones(BaseObject* object, Bone* bone) {
    BoneObject* boneObject = new BoneObject(bone);
    new TransformLink(object, boneObject, bone->name);
    for(auto childBone : bone->children)
        setupModelObjectBones(boneObject, childBone);
}

void ModelObject::updateSkeletonPose(BaseObject* object, Bone* bone) {
    skeletonPose[bone->jointIndex] = object->getTransformation() * bone->absoluteInv;
    for(auto childBone : bone->children)
        updateSkeletonPose(object->links[childBone->name]->getOther(object), childBone);
}

void ModelObject::drawBonePose(BaseObject* object, Bone* bone, float axesSize, float linesSize, float textSize) {
    btTransform transform = object->getTransformation(), inverseTransform = transform.inverse();
    btVector3* childrenPos = (linesSize > 0.0) ? new btVector3[bone->children.size()] : NULL;
    for(unsigned int i = 0; i < bone->children.size(); i ++) {
        BaseObject* childObject = object->links[bone->children[i]->name]->getOther(object);
        drawBonePose(childObject, bone->children[i], axesSize, linesSize, textSize);
        if(linesSize > 0.0) childrenPos[i] = inverseTransform(modelMat.getOrigin());
    }
    
    if(textSize > 0.0)
        mainFont->renderStringToScreen(bone->name.c_str(), transform.getOrigin(), 0.02, Color4(1.0), false);
    modelMat = transform;
    shaderPrograms[colorSP]->use();
    
    float* vertices;
    {
        unsigned int verticesCount = 36+12*bone->children.size();
        if(axesSize > 0.0) verticesCount += 36;
        if(linesSize > 0.0) verticesCount += 12*bone->children.size();
        vertices = (verticesCount) ? new float[verticesCount] : NULL;
    }
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, vertices);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &vertices[3]);
    
    if(axesSize > 0.0) {
        for(unsigned int i = 0; i < 6; i ++) {
            vertices[i*6  ] = (i == 1) ? axesSize : 0.0;
            vertices[i*6+1] = (i == 3) ? axesSize : 0.0;
            vertices[i*6+2] = (i == 5) ? axesSize : 0.0;
            vertices[i*6+3] = (i < 2) ? 1.0 : 0.0;
            vertices[i*6+4] = (i == 2 || i == 3) ? 1.0 : 0.0;
            vertices[i*6+5] = (i >= 4) ? 1.0 : 0.0;
        }
        glDrawArrays(GL_LINES, 0, 6);
    }
    
    if(linesSize > 0.0) {
        for(unsigned int i = 0; i < bone->children.size(); i ++) {
            btVector3 pos = childrenPos[i];
            vertices[i*12] = 0.0;
            vertices[i*12+1] = 0.0;
            vertices[i*12+2] = 0.0;
            vertices[i*12+3] = 1.0;
            vertices[i*12+4] = 1.0;
            vertices[i*12+5] = 1.0;
            vertices[i*12+6] = pos.x();
            vertices[i*12+7] = pos.y();
            vertices[i*12+8] = pos.z();
            vertices[i*12+9] = 0.5;
            vertices[i*12+10] = 0.5;
            vertices[i*12+11] = 0.5;
        }
        glLineWidth(linesSize);
        glDrawArrays(GL_LINES, 0, bone->children.size()*2);
        glLineWidth(1);
        delete [] childrenPos;
    }
    
    if(vertices) delete [] vertices;
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
}

bool ModelObject::gameTick() {
    BaseObject::gameTick();
    if(model->skeleton) {
        //if(!skeletonPose)
        //    skeletonPose = new btTransform[model->skeleton->bones.size()];
        Bone* rootBone = model->skeleton->rootBone;
        updateSkeletonPose(links[rootBone->name]->getOther(this), rootBone);
    }
    return true;
}

void ModelObject::draw() {
    modelMat = getTransformation();
    model->draw(this);
}

void ModelObject::drawAccumulatedMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

void ModelObject::drawSkeletonPose(float axesSize, float linesSize, float textSize) {
    Bone* rootBone = model->skeleton->rootBone;
    drawBonePose(links[rootBone->name]->getOther(this), rootBone, axesSize, linesSize, textSize);
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
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose, model->skeleton->bones.size());
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

bool WaterObject::gameTick() {
    for(unsigned int i = 0; i < waves.size(); i ++) {
        waves[i].age += worldManager.animationFactor;
        if(waves[i].age >= waves[i].maxAge) {
            waves.erase(waves.begin()+i);
            i --;
        }
    }
    return BaseObject::gameTick();
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