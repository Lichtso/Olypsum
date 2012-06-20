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
    glIndex = -1;
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
    char str[64];
    sprintf(str, "lightSources[%d].type", glIndex);
    currentShaderProgram->setUniformF(str, type);
    sprintf(str, "lightSources[%d].range", glIndex);
    currentShaderProgram->setUniformF(str, range);
    sprintf(str, "lightSources[%d].color", glIndex);
    currentShaderProgram->setUniformVec3(str, color);
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
    shadowSkeletonShaderProgram->use();
    shadowSkeletonShaderProgram->setUniformF("paraboloidRange", 0.0);
    shadowShaderProgram->use();
    shadowShaderProgram->setUniformF("paraboloidRange", 0.0);
    renderingState = RenderingShadow;
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
    if(glIndex < 0) return;
    Light::use();
    char str[64];
    sprintf(str, "lightSources[%d].cutoff", glIndex);
    currentShaderProgram->setUniformF(str, 4.0);
    sprintf(str, "lightSources[%d].direction", glIndex);
    currentShaderProgram->setUniformVec3(str, direction*-1.0);
    sprintf(str, "lightSources[%d].shadowFactor", glIndex);
    currentShaderProgram->setUniformF(str, (shadowMap) ? 0.005 : 0.0);
    if(shadowMap == 0) return;
    sprintf(str, "lightSources[%d].shadowMat", glIndex);
    currentShaderProgram->setUniformMatrix4(str, &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, glIndex*2+3);
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
    shadowCam->fov = cutoff;
    shadowCam->near = 1.0;
    shadowCam->far = range;
    shadowCam->calculate();
    shadowCam->use();
    currentCam = shadowCam;
    shadowSkeletonShaderProgram->use();
    shadowSkeletonShaderProgram->setUniformF("paraboloidRange", 0.0);
    shadowShaderProgram->use();
    shadowShaderProgram->setUniformF("paraboloidRange", 0.0);
    renderingState = RenderingShadow;
    glDisable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    mainFBO.renderInTexture(shadowMap);
    renderScene();
    //Render circle mask
    float vertices[12] = {
        -1.0, -1.0, 0.0,
        1.0, -1.0, 0.0,
        1.0, 1.0, 0.0,
        -1.0, 1.0, 0.0
    };
    shadowShaderProgram->use();
    shadowShaderProgram->setUniformF("paraboloidRange", -1.0);
    shadowShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);
    return true;
}

void SpotLight::deleteShadowmap() {
    Light::deleteShadowmap();
}

void SpotLight::use() {
    if(glIndex < 0) return;
    Light::use();
    char str[64];
    sprintf(str, "lightSources[%d].cutoff", glIndex);
    currentShaderProgram->setUniformF(str, cutoff*0.5);
    sprintf(str, "lightSources[%d].direction", glIndex);
    currentShaderProgram->setUniformVec3(str, direction);
    sprintf(str, "lightSources[%d].shadowFactor", glIndex);
    currentShaderProgram->setUniformF(str, (shadowMap) ? 0.015 : 0.0);
    sprintf(str, "lightSources[%d].position", glIndex);
    currentShaderProgram->setUniformVec3(str, position);
    if(shadowMap == 0) return;
    sprintf(str, "lightSources[%d].shadowMat", glIndex);
    currentShaderProgram->setUniformMatrix4(str, &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, glIndex*2+3);
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
    shadowSkeletonShaderProgram->use();
    shadowSkeletonShaderProgram->setUniformF("paraboloidRange", range);
    shadowShaderProgram->use();
    shadowShaderProgram->setUniformF("paraboloidRange", range);
    renderingState = RenderingShadow;
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
    if(glIndex < 0) return;
    Light::use();
    char str[64];
    sprintf(str, "lightSources[%d].cutoff", glIndex);
    currentShaderProgram->setUniformF(str, (omniDirectional) ? 4.0 : M_PI_2);
    sprintf(str, "lightSources[%d].direction", glIndex);
    currentShaderProgram->setUniformVec3(str, direction);
    sprintf(str, "lightSources[%d].shadowFactor", glIndex);
    currentShaderProgram->setUniformF(str, (shadowMap) ? 0.002 : 0.0);
    sprintf(str, "lightSources[%d].position", glIndex);
    currentShaderProgram->setUniformVec3(str, position);
    if(!shadowMap) return;
    sprintf(str, "lightSources[%d].shadowMat", glIndex);
    currentShaderProgram->setUniformMatrix4(str, &shadowCam->shadowMat);
    mainFBO.useTexture(shadowMap, glIndex*2+3);
    if(shadowMapB) mainFBO.useTexture(shadowMapB, glIndex*2+4);
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
}

void LightManager::setLights(Vector3 position) {
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = position;
    std::sort(lights.begin(), lights.end(), lightPrioritySorter);
    
    unsigned int activeLights = 0;
    for(unsigned int i = 0; i < lights.size(); i ++) {
        if(lights[i]->getPriority(position) >= 1.0) continue;
        lights[i]->glIndex = (activeLights < maxLightCount) ? activeLights : -1;
        lights[i]->use();
        activeLights ++;
    }
    
    char str[64];
    for(unsigned int i = activeLights; i < maxLightCount; i ++) {
        sprintf(str, "lightSources[%d].type", i);
        currentShaderProgram->setUniformF(str, 0);
    }
}

void LightManager::setAllLightsOff() {
    char str[64];
    for(unsigned int i = 0; i < maxLightCount; i ++) {
        sprintf(str, "lightSources[%d].type", i);
        currentShaderProgram->setUniformF(str, 0);
    }
}

LightManager lightManager;