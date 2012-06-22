//
//  Light.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Game.h"

Light::Light() {
    shadowCam = NULL;
    shadowMap = NULL;
    color = Vector3(1.0, 1.0, 1.0);
}

Light::~Light() {
    if(shadowCam) delete shadowCam;
    if(shadowMap) mainFBO.deleteTexture(shadowMap);
}

float Light::getPriority(Vector3 position) {
    if(range <= 0.0) return 1.0;
    switch(type) {
        case LightType_Directional:
            return -1.0;
        case LightType_Spot:
            return 0.0;
        case LightType_Positional: {
            PositionalLight* light = (PositionalLight*)this;
            return fmin(1.0, (position-light->position).getLength()/range);
        }
    }
}

bool Light::calculateShadowmap() {
    if(!shadowMap) {
        shadowMap = mainFBO.addTexture(1024);
        if(!shadowMap) {
            deleteShadowmap();
            return false;
        }
        if(!shadowCam) shadowCam = new Cam();
    }
    lightManager.currentShadowLight = this;
    return true;
}

void Light::deleteShadowmap() {
    if(shadowCam) {
        delete shadowCam;
        shadowCam = NULL;
    }
    if(!shadowMap) return;
    mainFBO.deleteTexture(shadowMap);
    shadowMap = NULL;
}

void Light::use() {
    currentShaderProgram->setUniformF("lRange", range);
    currentShaderProgram->setUniformVec3("lColor", color);
    currentShaderProgram->setUniformVec3("lDirection", direction);
}

void Light::selectShaderProgram(bool skeletal) {
    printf("ERROR: Unreachable function called!\n");
}

bool LightPrioritySorter::operator()(Light* a, Light* b) {
    return a->getPriority(position) < b->getPriority(position);
}

DirectionalLight::DirectionalLight() {
    type = LightType_Directional;
    direction = Vector3(0.0, -1.0, 0.0);
    upDir = Vector3(0.0, 0.0, 1.0);
    distance = 50.0;
    range = 100.0;
    width = height = 10.0;
}

bool DirectionalLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(direction*distance*-1.0);
    shadowCam->width = width;
    shadowCam->height = height;
    shadowCam->fov = 0.0;
    shadowCam->near = 1.0;
    shadowCam->far = range;
    shadowCam->calculate();
    shadowCam->use();
    currentCam = shadowCam;
    glDisable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    mainFBO.renderInTexture(shadowMap);
    renderScene();
    glEnable(GL_BLEND);
    return true;
}

void DirectionalLight::deleteShadowmap() {
    Light::deleteShadowmap();
}

void DirectionalLight::use() {
    if(!shadowMap)
        shaderPrograms[directionalLightSP]->use();
    else
        shaderPrograms[directionalShadowLightSP]->use();
    Light::use();
    if(!shadowMap) return;
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, 5);
}

void DirectionalLight::selectShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}

SpotLight::SpotLight() {
    type = LightType_Spot;
    position = Vector3(0.0, 0.0, 0.0);
    direction = Vector3(0.0, 0.0, 1.0);
    upDir = Vector3(0.0, 1.0, 0.0);
    cutoff = 45.0/180.0*M_PI;
    range = 50.0;
}

bool SpotLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(position);
    shadowCam->width = 1.0;
    shadowCam->height = 1.0;
    shadowCam->fov = cutoff*2.0;
    shadowCam->near = 1.0;
    shadowCam->far = range;
    shadowCam->calculate();
    shadowCam->use();
    currentCam = shadowCam;
    glDisable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    mainFBO.renderInTexture(shadowMap);
    renderScene();
    //Render circle mask -----> TODO <------
    /*float vertices[12] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0
    };
    
    shaderPrograms[solidShadowSP]->use();
    shaderPrograms[solidShadowSP]->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);*/
    return true;
}

void SpotLight::deleteShadowmap() {
    Light::deleteShadowmap();
}

void SpotLight::use() {
    if(!shadowMap)
        shaderPrograms[spotLightSP]->use();
    else
        shaderPrograms[spotShadowLightSP]->use();
    Light::use();
    currentShaderProgram->setUniformF("lCutoff", cutoff);
    currentShaderProgram->setUniformVec3("lPosition", position);
    if(!shadowMap) return;
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, 5);
}

void SpotLight::selectShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalShadowSP]->use();
    else
        shaderPrograms[solidShadowSP]->use();
}

PositionalLight::PositionalLight() {
    type = LightType_Positional;
    shadowMapB = NULL;
    position = Vector3(0.0, 0.0, 0.0);
    direction = Vector3(0.0, 0.0, 1.0);
    upDir = Vector3(0.0, 1.0, 0.0);
    range = 10.0;
    omniDirectional = true;
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) mainFBO.deleteTexture(shadowMapB);
}

bool PositionalLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    if(omniDirectional && !shadowMapB)
        shadowMapB = mainFBO.addTexture(1024);
    
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(position);
    shadowCam->viewMat = shadowCam->camMat.getInverse();
    shadowCam->shadowMat = shadowCam->viewMat;
    shadowCam->use();
    currentCam = shadowCam;
    glDisable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    if(shadowMapB) {
        Matrix4 restoreMat = shadowCam->viewMat;
        shadowCam->viewMat.setIdentity();
        shadowCam->viewMat.setDirection(direction*-1.0, upDir);
        shadowCam->viewMat.translate(position);
        shadowCam->viewMat = shadowCam->viewMat.getInverse();
        mainFBO.renderInTexture(shadowMapB);
        renderScene();
        shadowCam->viewMat = restoreMat;
    }
    mainFBO.renderInTexture(shadowMap);
    renderScene();
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
    if(!shadowMap)
        shaderPrograms[positionalLightSP]->use();
    else if(!shadowMapB)
        shaderPrograms[positionalShadowLightSP]->use();
    else
        shaderPrograms[positionalDualShadowLightSP]->use();
    Light::use();
    currentShaderProgram->setUniformF("lCutoff", (omniDirectional) ? 4.0 : M_PI_2);
    currentShaderProgram->setUniformVec3("lPosition", position);
    if(!shadowMap) return;
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, 5);
    if(shadowMapB) mainFBO.useTexture(shadowMapB, 6);
}

void PositionalLight::selectShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalParabolidShadowSP]->use();
    else
        shaderPrograms[solidParabolidShadowSP]->use();
}

LightManager::~LightManager() {
    for(unsigned int i = 0; i < lights.size(); i ++)
        delete lights[i];
}

void LightManager::calculateShadows(unsigned int maxShadows) {
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->camMat.pos;
    std::sort(lights.begin(), lights.end(), lightPrioritySorter);
    
    for(unsigned int i = 0; i < fmin(maxShadows, lights.size()); i ++)
        lights[i]->calculateShadowmap();
    
    for(unsigned int i = maxShadows; i < lights.size(); i ++)
        lights[i]->deleteShadowmap();
    
    currentShadowLight = NULL;
}

void LightManager::useLights() {
    std::vector<unsigned char> inBuffers, outBuffers;
    inBuffers.push_back(colorDBuffer);
    inBuffers.push_back(materialDBuffer);
    outBuffers.push_back(diffuseDBuffer);
    outBuffers.push_back(specularDBuffer);
    shaderPrograms[deferredPrepareSP]->use();
    mainFBO.renderDeferred(&inBuffers, &outBuffers);
    
    inBuffers.clear();
    inBuffers.push_back(diffuseDBuffer);
    inBuffers.push_back(specularDBuffer);
    inBuffers.push_back(positionDBuffer);
    inBuffers.push_back(normalDBuffer);
    inBuffers.push_back(materialDBuffer);
    outBuffers.clear();
    outBuffers.push_back(diffuseDBuffer);
    outBuffers.push_back(specularDBuffer);
    for(unsigned int i = 0; i < lights.size(); i ++) {
        lights[i]->use();
        mainFBO.renderDeferred(&inBuffers, &outBuffers);
    }
    
    inBuffers.clear();
    inBuffers.push_back(colorDBuffer);
    inBuffers.push_back(diffuseDBuffer);
    inBuffers.push_back(specularDBuffer);
    outBuffers.clear();
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(&inBuffers, &outBuffers);
}

LightManager lightManager;