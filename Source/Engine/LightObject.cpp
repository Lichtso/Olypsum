//
//  LightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LevelManager.h"

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
    body->setCollisionShape(new btSphereShape(1.0));
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
    
    shadowMap = NULL;
    color = Color4(1.0);
}

LightObject::~LightObject() {
    if(shadowMap)
        delete shadowMap;
}

void LightObject::remove() {
    for(int i = 0; i < objectManager.lightObjects.size(); i ++)
        if(objectManager.lightObjects[i] == this) {
            objectManager.lightObjects.erase(objectManager.lightObjects.begin()+i);
            break;
        }
    BaseObject::remove();
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
        if(shadowMap)
            delete shadowMap;
        shadowMap = NULL;
        return false;
    }
    if(!shadowMap) {
        shadowMap = new ColorBuffer(min(1024U, mainFBO.maxSize), true, cubemapsEnabled && dynamic_cast<PositionalLight*>(this));
        if(glGetError() == GL_OUT_OF_MEMORY) {
            delete shadowMap;
            shadowMap = NULL;
            return false;
        }
    }
    objectManager.currentShadowLight = this;
    return true;
}

void LightObject::draw() {
    currentShaderProgram->setUniformF("lRange", shadowCam.far);
    currentShaderProgram->setUniformVec3("lColor", color.getVector());
    
    if(currentCam->testInverseNearPlaneHit(static_cast<btDbvtProxy*>(body->getBroadphaseHandle()))) { //PART IN FRUSTUM
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    }else{ //ALL IN FRUSTUM
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }
    
    mainFBO.renderDeferred(false, inBuffers, sizeof(inBuffers)/sizeof(unsigned char), outBuffers, sizeof(outBuffers)/sizeof(unsigned char));
}

void LightObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    BaseObject::init(node, levelLoader);
    node = node->first_node("Color");
    if(!node) {
        log(error_log, "Tried to construct LightObject without \"Color\"-node.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(node->value(), "%f");
    color = Color4(vecData.data[0], vecData.data[1], vecData.data[2]);
}



DirectionalLight::DirectionalLight() {
    shadowCam.fov = 0.0;
    shadowCam.near = 1.0;
}

DirectionalLight::DirectionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :DirectionalLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct DirectionalLight without \"Bounds\"-node.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(bounds->value(), "%f");
    setBounds(vecData.data[0], vecData.data[1], vecData.data[2]);
    LightObject::init(node, levelLoader);
}

void DirectionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(transformation * shiftMat);
}

void DirectionalLight::setBounds(float width, float height, float range) {
    shadowCam.width = width;
    shadowCam.height = height;
    shadowCam.far = range;
    
    setPhysicsShape(new btBoxShape(btVector3(width, height, range*0.5)));
}

bool DirectionalLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    if(!LightObject::gameTick(shadowActive)) return true;
    shadowCam.use();
    
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
    objectManager.drawScene();
    glEnable(GL_BLEND);
    return true;
}

void DirectionalLight::draw() {
    modelMat.setIdentity();
    modelMat.setBasis(modelMat.getBasis().scaled(btVector3(shadowCam.width, shadowCam.height, shadowCam.far*0.5)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far*0.5));
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        shadowMap->use(5);
    }else
        shaderPrograms[directionalLightSP]->use();
    
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2));
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
}

SpotLight::SpotLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :SpotLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct SpotLight without \"Bounds\"-node.");
        return;
    }
    float cutoff, range;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = bounds->first_attribute("range");
    if(!attribute) {
        log(error_log, "Tried to construct SpotLight without \"range\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &range);
    attribute = bounds->first_attribute("cutoff");
    if(!attribute) {
        log(error_log, "Tried to construct SpotLight without \"cutoff\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &cutoff);
    setBounds(cutoff, range);
    LightObject::init(node, levelLoader);
}

void SpotLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(transformation * shiftMat);
}

void SpotLight::setBounds(float cutoff, float range) {
    shadowCam.width = shadowCam.height = 1.0;
    shadowCam.fov = cutoff;
    shadowCam.far = range;
    
    setPhysicsShape(new btConeShapeZ(tan(shadowCam.fov*0.5)*shadowCam.far, shadowCam.far));
}

bool SpotLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    if(!LightObject::gameTick(shadowActive)) return true;
    shadowCam.use();
    
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
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
    float radius = tan(shadowCam.fov*0.5)*shadowCam.far;
    modelMat.setIdentity();
    modelMat.setBasis(modelMat.getBasis().scaled(btVector3(radius*1.05, radius*1.05, shadowCam.far)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far));
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        shadowMap->use(5);
    }else
        shaderPrograms[spotLightSP]->use();
    
    currentShaderProgram->setUniformF("lCutoff", -cos(shadowCam.fov*0.5));
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2)*-1.0);
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



PositionalLight::PositionalLight() :shadowMapB(NULL) {
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
}

PositionalLight::PositionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :PositionalLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct PositionalLight without \"Bounds\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = bounds->first_attribute("range");
    if(!attribute) {
        log(error_log, "Tried to construct PositionalLight without \"range\"-attribute.");
        return;
    }
    float range;
    sscanf(attribute->value(), "%f", &range);
    attribute = bounds->first_attribute("omniDirectional");
    if(!attribute) {
        log(error_log, "Tried to construct PositionalLight without \"omniDirectional\"-attribute.");
        return;
    }
    setBounds(strcmp(attribute->value(), "true") == 0, range);
    LightObject::init(node, levelLoader);
}

PositionalLight::~PositionalLight() {
    if(shadowMapB)
        delete shadowMapB;
}

void PositionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    if(abs(shadowCam.fov-M_PI*2.0) < 0.001) {
        body->setWorldTransform(transformation);
        return;
    }
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(transformation * shiftMat);
}

void PositionalLight::setBounds(bool omniDirectional, float range) {
    shadowCam.fov = (cubemapsEnabled) ? M_PI_2 : ((omniDirectional) ? M_PI*2.0 : M_PI);
    shadowCam.far = range;
    
    if(omniDirectional)
        setPhysicsShape(new btSphereShape(range));
    else
        setPhysicsShape(new btCylinderShape(btVector3(range, range, range*0.5)));
}

bool PositionalLight::gameTick(bool shadowActive) {
    shadowCam.gameTick();
    if(!LightObject::gameTick(shadowActive)) return true;
    if(abs(shadowCam.fov-M_PI*2.0) < 0.001 && !shadowMapB) {
        shadowMapB = new ColorBuffer(1024, true, false);
        if(glGetError() == GL_OUT_OF_MEMORY) {
            delete shadowMapB;
            shadowMapB = NULL;
        }
    }
    shadowCam.use();
    
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
        mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
        objectManager.drawScene();
        if(shadowMapB) {
            shadowCam.viewMat.scale(btVector3(-1.0, 1.0, -1.0));
            mainFBO.renderInTexture(shadowMapB, GL_TEXTURE_2D);
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
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shadowMap->use(5);
        if(shadowMapB) {
            shadowMapB->use(6);
            shaderPrograms[positionalShadowDualLightSP]->use();
        }else
            shaderPrograms[positionalShadowLightSP]->use();
    }else
        shaderPrograms[positionalLightSP]->use();
    
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam.viewMat);
    currentShaderProgram->setUniformF("lCutoff", (abs(shadowCam.fov-M_PI*2.0) < 0.001) ? 1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2)*-1.0);
    LightObject::draw();
    if(abs(shadowCam.fov-M_PI) < 0.001)
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