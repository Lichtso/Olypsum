//
//  Light.cpp
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

Texture randomNormalMap;
LightBoxVolume lightBox(btVector3(1, 1, 1));
LightSphereVolume lightSphere(1, sphereAccuracyX, sphereAccuracyY);
LightParabolidVolume lightCone(1, coneAccuracy, 0);
LightParabolidVolume lightParabolid(1, sphereAccuracyX, parabolidAccuracyY);

unsigned char inBuffersA[] = { positionDBuffer, normalDBuffer, materialDBuffer, diffuseDBuffer, specularDBuffer }, outBuffersA[] = { diffuseDBuffer, specularDBuffer };
unsigned char inBuffersB[] = { depthDBuffer }, outBuffersB[] = { ssaoDBuffer };
unsigned char inBuffersC[] = { colorDBuffer, diffuseDBuffer, specularDBuffer, materialDBuffer, normalDBuffer, ssaoDBuffer }, outBuffersC[] = { colorDBuffer };
unsigned char inBuffersD[] = { depthDBuffer, colorDBuffer }, outBuffersD[] = { colorDBuffer };

static bool drawLightVolume() {
    //TODO: Reimplement
    /*if(!) { //NOT IN FRUSTUM
        return false;
    }
    if() { //ALL IN FRUSTUM
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }else{ //PART IN FRUSTUM
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    }*/
    mainFBO.renderDeferred(false, inBuffersA, sizeof(inBuffersA)/sizeof(unsigned char), outBuffersA, sizeof(outBuffersA)/sizeof(unsigned char));
    return true;
}



Light::Light() {
    lightManager.lights.push_back(this);
    shadowMap = NULL;
    color = Color4(1.0);
}

Light::~Light() {
    if(shadowMap) mainFBO.deleteTexture(shadowMap);
    for(int i = 0; i < lightManager.lights.size(); i ++)
        if(lightManager.lights[i] == this) {
            lightManager.lights.erase(lightManager.lights.begin()+i);
            return;
        }
}

bool Light::calculate(bool shadowActive) {
    if(!shadowActive) {
        deleteShadowmap();
        return false;
    }
    if(!shadowMap) {
        shadowMap = mainFBO.addTexture(min(1024U, mainFBO.maxSize), true, false);
        if(!shadowMap) {
            deleteShadowmap();
            return false;
        }
    }
    lightManager.currentShadowLight = this;
    return true;
}

void Light::deleteShadowmap() {
    if(!shadowMap) return;
    mainFBO.deleteTexture(shadowMap);
    shadowMap = NULL;
}

void Light::use() {
    currentShaderProgram->setUniformF("lRange", shadowCam.far);
    currentShaderProgram->setUniformVec3("lColor", color.getVector());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.camMat.getBasis().getRow(2)*-1.0);
}

void Light::prepareShaderProgram(bool skeletal) {
    log(error_log, "Unreachable function called!");
}

void Light::setRange(float range) {
    shadowCam.far = range;
    shadowCam.calculateFrustum(btVector3(-1, -1, 0), btVector3(1, 1, 0));
}

float Light::getPriority(btVector3 position) {
    if(shadowCam.far <= 0.0) return 1.0;
    switch(type) {
        case LightType_Directional:
            return -1.0;
        case LightType_Spot:
            return 0.0;
        case LightType_Positional: {
            PositionalLight* light = (PositionalLight*)this;
            return fmin(1.0, (position-light->shadowCam.camMat.getOrigin()).length()/shadowCam.far);
        }
    }
}

bool LightPrioritySorter::operator()(Light* a, Light* b) {
    return a->getPriority(position) < b->getPriority(position);
}



DirectionalLight::DirectionalLight() {
    type = LightType_Directional;
    shadowCam.camMat.setIdentity();
    shadowCam.camMat.setRotation(btQuaternion(btVector3(1, 0, 0), M_PI));
    shadowCam.camMat.setOrigin(btVector3(0, 50, 0));
    shadowCam.width = 10.0;
    shadowCam.height = 10.0;
    shadowCam.fov = 0.0;
    shadowCam.near = 1.0;
    shadowCam.far = 100.0;
}

bool DirectionalLight::calculate(bool shadowActive) {
    shadowCam.calculate();
    shadowCam.use();
    if(!Light::calculate(shadowActive)) return false;
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, 0);
    objectManager.draw();
    glEnable(GL_BLEND);
    return true;
}

void DirectionalLight::deleteShadowmap() {
    Light::deleteShadowmap();
}

void DirectionalLight::use() {
    modelMat.setIdentity();
    btMatrix3x3 basis = modelMat.getBasis();
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.width, shadowCam.height, shadowCam.far)));
    modelMat *= shadowCam.camMat;
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[directionalLightSP]->use();
    Light::use();
    
    if(drawLightVolume())
        lightBox.draw();
}

void DirectionalLight::prepareShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}



SpotLight::SpotLight() {
    type = LightType_Spot;
    shadowCam.camMat.setIdentity();
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.fov = 90.0/180.0*M_PI;
    shadowCam.near = 0.1;
    shadowCam.far = 50.0;
}

bool SpotLight::calculate(bool shadowActive) {
    shadowCam.calculate();
    shadowCam.use();
    if(!Light::calculate(shadowActive)) return false;
    glDisable(GL_BLEND);
    mainFBO.renderInTexture(shadowMap, 0);
    //Render circle mask
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    shaderPrograms[spotShadowCircleLightSP]->use();
    shaderPrograms[spotShadowCircleLightSP]->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    objectManager.draw();
    glEnable(GL_BLEND);
    
    return true;
}

void SpotLight::deleteShadowmap() {
    Light::deleteShadowmap();
}

void SpotLight::use() {
    float radius = shadowCam.far*tan(shadowCam.fov*0.5)*1.05;
    modelMat.setIdentity();
    btMatrix3x3 basis = modelMat.getBasis();
    modelMat.setBasis(basis.scaled(btVector3(radius, radius, shadowCam.far)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far));
    modelMat *= shadowCam.camMat;
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[spotLightSP]->use();
    currentShaderProgram->setUniformF("lCutoff", cos(shadowCam.fov*0.5));
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.camMat.getOrigin());
    Light::use();
    
    if(drawLightVolume())
        lightCone.draw();
}

void SpotLight::prepareShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}



PositionalLight::PositionalLight() {
    type = LightType_Positional;
    shadowMapB = NULL;
    omniDirectional = true;
    shadowCam.camMat.setIdentity();
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
    shadowCam.far = 10.0;
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) mainFBO.deleteTexture(shadowMapB);
}

bool PositionalLight::calculate(bool shadowActive) {
    shadowCam.calculate();
    shadowCam.use();
    if(cubemapsEnabled) {
        if(!shadowActive) {
            deleteShadowmap();
            return false;
        }
        if(!shadowMap) {
            shadowMap = mainFBO.addTexture(1024, true, true);
            if(!shadowMap) {
                deleteShadowmap();
                return false;
            }
        }
        lightManager.currentShadowLight = this;
    }else{
        if(!Light::calculate(shadowActive)) return false;
        if(omniDirectional && !shadowMapB)
            shadowMapB = mainFBO.addTexture(1024, true, false);
    }
    
    Matrix4 viewMat = shadowCam.viewMat;
    glDisable(GL_BLEND);
    if(cubemapsEnabled) {
        btTransform camMat = shadowCam.camMat;
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
            shadowCam.camMat = rotTransform * camMat;
            shadowCam.fov = M_PI_2;
            shadowCam.calculate();
            mainFBO.renderInTexture(shadowMap, side);
            objectManager.draw();
        }
        shadowCam.camMat = camMat;
        shadowCam.viewMat = viewMat;
    }else{
        shaderPrograms[solidParabolidShadowSP]->use();
        shaderPrograms[solidParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        shaderPrograms[skeletalParabolidShadowSP]->use();
        shaderPrograms[skeletalParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        shadowCam.fov = M_PI;
        mainFBO.renderInTexture(shadowMap, 0);
        objectManager.draw();
        if(shadowMapB) {
            shadowCam.viewMat.scale(btVector3(-1.0, 1.0, -1.0));
            mainFBO.renderInTexture(shadowMapB, 0);
            objectManager.draw();
            shadowCam.viewMat = viewMat;
        }
    }
    glEnable(GL_BLEND);
    return true;
}

void PositionalLight::deleteShadowmap() {
    Light::deleteShadowmap();
    if(this->shadowMapB == 0) return;
    mainFBO.deleteTexture(shadowMapB);
    shadowMapB = NULL;
}

void PositionalLight::use() {
    modelMat.setIdentity();
    btMatrix3x3 basis(btQuaternion(btVector3(0.0, 1.0, 0.0), M_PI));
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.far*1.05, shadowCam.far*1.05, shadowCam.far*1.05)));
    modelMat *= shadowCam.camMat;
    
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
    currentShaderProgram->setUniformF("lCutoff", (omniDirectional) ? -1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.camMat.getOrigin());
    Light::use();
    
    if(!cubemapsEnabled && !omniDirectional) {
        if(drawLightVolume())
            lightParabolid.draw();
        return;
    }else if(drawLightVolume())
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



LightManager::LightManager() {
    currentShadowLight = NULL;
}

LightManager::~LightManager() {
    clear();
}

void LightManager::init() {
    lightBox.init();
    lightSphere.init();
    lightCone.init();
    lightParabolid.init();
    
    randomNormalMap.width = 128;
    randomNormalMap.height = 128;
    randomNormalMap.loadRandom();
    randomNormalMap.uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
}

void LightManager::clear() {
    for(unsigned int i = 0; i < lights.size(); i ++)
        delete lights[i];
    lights.clear();
    currentShadowLight = NULL;
}

void LightManager::calculateShadows(unsigned int maxShadows) {
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->camMat.getOrigin();
    std::sort(lights.begin(), lights.end(), lightPrioritySorter);
    
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->calculate(i < maxShadows);
    
    currentShadowLight = NULL;
}

void LightManager::illuminate() {
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->use();
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
}

void LightManager::drawDeferred() {
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    illuminate();
    
    if(ssaoQuality) {
        glViewport(0, 0, screenSize[0] >> 1, screenSize[1] >> 1);
        randomNormalMap.use(GL_TEXTURE_2D, 1);
        shaderPrograms[ssaoSP]->use();
        mainFBO.renderDeferred(true, inBuffersB, 1, outBuffersB, 1);
        glViewport(0, 0, screenSize[0], screenSize[1]);
    }
    
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(true, inBuffersC, (ssaoQuality) ? 6 : 4, outBuffersC, (objectManager.transparentAccumulator.size() > 0) ? 1 : 0);
    mainFBO.renderTransparentInDeferredBuffers();
    
    if(screenBlurFactor > 0.0) {
        shaderPrograms[blurSP]->use();
        currentShaderProgram->setUniformF("processingValue", screenBlurFactor);
        mainFBO.renderDeferred(true, inBuffersC, 1, outBuffersC, 0);
    }else{
        if(edgeSmoothEnabled) {
            shaderPrograms[edgeSmoothSP]->use();
            mainFBO.renderDeferred(true, inBuffersD, 2, outBuffersD, (depthOfFieldQuality) ? 1 : 0);
        }
        
        if(depthOfFieldQuality) {
            shaderPrograms[depthOfFieldSP]->use();
            mainFBO.renderDeferred(true, inBuffersD, 2, outBuffersD, 0);
        }
    }
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

LightManager lightManager;