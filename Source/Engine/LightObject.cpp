//
//  LightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

#define coneAccuracy 12
#define sphereAccuracyX 10
#define sphereAccuracyY 6
#define parabolidAccuracyY 4

LightBoxVolume lightBox(btVector3(1, 1, 1));
LightSphereVolume lightSphere(1, sphereAccuracyX, sphereAccuracyY);
LightParabolidVolume lightCone(1, coneAccuracy, 0);
LightParabolidVolume lightParabolid(1, sphereAccuracyX, parabolidAccuracyY);

unsigned char inBuffers[] = { positionDBuffer, normalDBuffer, materialDBuffer, diffuseDBuffer, specularDBuffer }, outBuffers[] = { diffuseDBuffer, specularDBuffer };

void initLightVolumes() {
    lightBox.init();
    lightSphere.init();
    lightCone.init();
    lightParabolid.init();
}



LightObject::LightObject() {
    objectManager.lightObjects.push_back(this);
    body = new btCollisionObject();
    body->setCollisionShape(NULL);
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
    
    shadowMap = NULL;
    color = Color4(1.0);
}

LightObject::~LightObject() {
    if(shadowMap) mainFBO.deleteTexture(shadowMap);
}

void LightObject::remove() {
    for(int i = 0; i < objectManager.lightObjects.size(); i ++)
        if(objectManager.lightObjects[i] == this) {
            objectManager.lightObjects.erase(objectManager.lightObjects.begin()+i);
            return;
        }
    delete this;
}

btTransform LightObject::getTransformation() {
    return shadowCam.getTransformation();
}

void LightObject::setPhysicsShape(btCollisionShape* shape) {
    if(body->getCollisionShape())
        delete body->getCollisionShape();
    body->setCollisionShape(shape);
}

bool LightObject::gameTick(bool shadowActive) {
    if(!shadowActive) {
        mainFBO.deleteTexture(shadowMap);
        shadowMap = NULL;
        return false;
    }
    if(!shadowMap) {
        shadowMap = mainFBO.addTexture(min(1024U, mainFBO.maxSize), true,
                                       cubemapsEnabled && dynamic_cast<PositionalLight*>(this));
        if(!shadowMap)
            return false;
    }
    objectManager.currentShadowLight = this;
    return true;
}

void LightObject::draw() {
    currentShaderProgram->setUniformF("lRange", shadowCam.far);
    currentShaderProgram->setUniformVec3("lColor", color.getVector());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getRow(2)*-1.0);
    
    btStaticPlaneShape* shape = new btStaticPlaneShape(btVector3(0, 0, -1), 1);
    btCollisionObject* plane = new btCollisionObject();
    plane->setCollisionShape(shape);
    plane->setWorldTransform(shadowCam.getTransformation());
    plane->setCollisionFlags(plane->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    
    if(currentCam->testInverseNearPlaneHit(static_cast<btDbvtProxy*>(body->getBroadphaseHandle()))) { //PART IN FRUSTUM
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    }else{ //ALL IN FRUSTUM
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }
    
    mainFBO.renderDeferred(false, inBuffers, sizeof(inBuffers)/sizeof(unsigned char), outBuffers, sizeof(outBuffers)/sizeof(unsigned char));
}

bool LightPrioritySorter::operator()(LightObject* a, LightObject* b) {
    return a->getPriority(position) > b->getPriority(position);
}



DirectionalLight::DirectionalLight() {
    shadowCam.fov = 0.0;
    shadowCam.near = 1.0;
    setBounds(10.0, 10.0, 100.0);
}

void DirectionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(shiftMat * transformation);
}

void DirectionalLight::setBounds(float width, float height, float range) {
    shadowCam.width = width;
    shadowCam.height = height;
    shadowCam.far = range;
    
    setPhysicsShape(new btBoxShape(btVector3(width, height, range*0.5)));
}

bool DirectionalLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    shadowCam.use();
    if(!LightObject::gameTick(shadowActive)) return false;
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, 0);
    objectManager.drawScene();
    glEnable(GL_BLEND);
    return true;
}

void DirectionalLight::draw() {
    modelMat.setIdentity();
    btMatrix3x3 basis = modelMat.getBasis();
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.width, shadowCam.height, shadowCam.far)));
    modelMat *= shadowCam.getTransformation();
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[directionalLightSP]->use();
    
    LightObject::draw();
    lightBox.draw();
}

void DirectionalLight::prepareShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}

float DirectionalLight::getPriority(btVector3 position) {
    return 2.0;
}



SpotLight::SpotLight() {
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
    
    setBounds(90.0/180.0*M_PI, 10.0);
}

void SpotLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(shiftMat * transformation);
}

void SpotLight::setBounds(float cutoff, float range) {
    shadowCam.width = shadowCam.height = shadowCam.far*tan(cutoff*0.5);
    shadowCam.fov = cutoff;
    shadowCam.far = range;
    
    setPhysicsShape(new btConeShapeZ(shadowCam.width, range));
}

bool SpotLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    shadowCam.use();
    if(!LightObject::gameTick(shadowActive)) return false;
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, 0);
    //Render circle mask
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    shaderPrograms[spotShadowCircleLightSP]->use();
    shaderPrograms[spotShadowCircleLightSP]->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    objectManager.drawScene();
    glEnable(GL_BLEND);
    
    return true;
}

void SpotLight::draw() {
    modelMat.setIdentity();
    btMatrix3x3 basis = modelMat.getBasis();
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.width*1.05, shadowCam.height*1.05, shadowCam.far)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far));
    modelMat *= shadowCam.getTransformation();
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[spotLightSP]->use();
    
    currentShaderProgram->setUniformF("lCutoff", cos(shadowCam.fov*0.5));
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    LightObject::draw();
    lightCone.draw();
}

void SpotLight::prepareShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}

float SpotLight::getPriority(btVector3 position) {
    return 1.0;
}



PositionalLight::PositionalLight() {
    shadowMapB = NULL;
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
    
    setBounds(true, 10.0);
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) mainFBO.deleteTexture(shadowMapB);
}

void PositionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    if(shadowCam.fov != M_PI) {
        body->setWorldTransform(transformation);
        return;
    }
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(shiftMat * transformation);
}

void PositionalLight::setBounds(bool omniDirectional, float range) {
    shadowCam.fov = (cubemapsEnabled) ? M_PI_2 : ((omniDirectional) ? M_PI*2.0 : M_PI);
    shadowCam.far = range;
    
    if(omniDirectional)
        setPhysicsShape(new btSphereShape(range));
    else
        setPhysicsShape(new btCylinderShapeZ(btVector3(range, range, range)));
}

bool PositionalLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    shadowCam.use();
    if(!LightObject::gameTick(shadowActive)) return false;
    if(shadowCam.fov == M_PI*2.0 && !shadowMapB)
        shadowMapB = mainFBO.addTexture(1024, true, false);
    
    Matrix4 viewMat = shadowCam.viewMat;
    glDisable(GL_BLEND);
    if(cubemapsEnabled) {
        btTransform camMat = shadowCam.getTransformation();
        for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; side ++) {
            btQuaternion rotation;
            switch(side) {
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                    rotation.setEulerZYX(M_PI, M_PI_2, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                    rotation.setEulerZYX(M_PI, -M_PI_2, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                    rotation.setEulerZYX(0.0, 0.0, -M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                    rotation.setEulerZYX(0.0, 0.0, M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                    rotation.setEulerZYX(M_PI, M_PI, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                    rotation.setEulerZYX(M_PI, 0.0, 0.0);
                break;
            }
            btTransform rotTransform;
            rotTransform.setRotation(rotation);
            shadowCam.setTransformation(rotTransform * camMat);
            shadowCam.gameTick();
            mainFBO.renderInTexture(shadowMap, side);
            objectManager.drawScene();
        }
        shadowCam.setTransformation(camMat);
        shadowCam.viewMat = viewMat;
    }else{
        shaderPrograms[solidParabolidShadowSP]->use();
        shaderPrograms[solidParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        shaderPrograms[skeletalParabolidShadowSP]->use();
        shaderPrograms[skeletalParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        mainFBO.renderInTexture(shadowMap, 0);
        objectManager.drawScene();
        if(shadowMapB) {
            shadowCam.viewMat.scale(btVector3(-1.0, 1.0, -1.0));
            mainFBO.renderInTexture(shadowMapB, 0);
            objectManager.drawScene();
            shadowCam.viewMat = viewMat;
        }
    }
    glEnable(GL_BLEND);
    return true;
}

void PositionalLight::draw() {
    modelMat.setIdentity();
    btMatrix3x3 basis(btQuaternion(btVector3(0.0, 1.0, 0.0), M_PI));
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.far*1.05, shadowCam.far*1.05, shadowCam.far*1.05)));
    modelMat *= shadowCam.getTransformation();
    
    if(shadowMap) {
        mainFBO.useTexture(shadowMap, 5);
        if(shadowMapB) {
            mainFBO.useTexture(shadowMapB, 6);
            shaderPrograms[positionalShadowDualLightSP]->use();
        }else
            shaderPrograms[positionalShadowLightSP]->use();
    }else
        shaderPrograms[positionalLightSP]->use();
    
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam.viewMat);
    currentShaderProgram->setUniformF("lCutoff", (shadowCam.fov == M_PI*2.0) ? -1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    LightObject::draw();
    if(shadowCam.fov == M_PI)
        lightParabolid.draw();
    else
        lightSphere.draw();
}

void PositionalLight::prepareShaderProgram(bool skeletal) {
    if(cubemapsEnabled) {
        if(skeletal)
            shaderPrograms[skeletalShadowSP]->use();
        else
            shaderPrograms[solidShadowSP]->use();
    }else{
        if(skeletal)
            shaderPrograms[skeletalParabolidShadowSP]->use();
        else
            shaderPrograms[solidParabolidShadowSP]->use();
    }
}

float PositionalLight::getPriority(btVector3 position) {
    return 1.0-(position-shadowCam.getTransformation().getOrigin()).length()/shadowCam.far;
}