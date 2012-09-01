//
//  Light.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

#define coneAccuracy 12
#define sphereAccuracyX 10
#define sphereAccuracyY 6
#define parabolidAccuracyY 3

Texture randomNormalMap;
GLuint lightVolumeBuffers[8];
Vector3 directionalLightVertices[boxVerticesCount];
Vector3 spotLightVertices[parabolidVerticesCount(coneAccuracy, 0)];
Vector3 positionalLightVertices[parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY)];

static unsigned int copyVertices(Vector3* positions, float* vertices, unsigned int count) {
    for(unsigned int i = 0; i < count; i ++) {
        vertices[i*3  ] = positions[i].x;
        vertices[i*3+1] = positions[i].y;
        vertices[i*3+2] = positions[i].z;
    }
    return count;
}

unsigned char inBuffersA[] = { positionDBuffer, normalDBuffer, materialDBuffer, diffuseDBuffer, specularDBuffer }, outBuffersA[] = { diffuseDBuffer, specularDBuffer };
unsigned char inBuffersB[] = { depthDBuffer }, outBuffersB[] = { ssaoDBuffer };
unsigned char inBuffersC[] = { colorDBuffer, diffuseDBuffer, specularDBuffer, materialDBuffer, normalDBuffer, ssaoDBuffer }, outBuffersC[] = { colorDBuffer };
unsigned char inBuffersD[] = { depthDBuffer, colorDBuffer }, outBuffersD[] = { colorDBuffer };

static bool drawLightVolume(Vector3* verticesSource, unsigned int verticesCount) {
    Vector3* vertices = new Vector3[verticesCount];
    for(unsigned int i = 0; i < verticesCount; i ++)
        vertices[i] = verticesSource[i] * modelMat;
    if(!currentCam->frustum.testPolyhedronInclusiveHit(vertices, verticesCount)) {
        delete [] vertices;
        return false;
    }
    if(currentCam->frustum.front.testPolyhedronExclusiveHit(vertices, verticesCount)) {
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }else{
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    }
    delete [] vertices;
    mainFBO.renderDeferred(false, inBuffersA, 5, outBuffersA, 2);
    return true;
}



Light::Light() {
    lightManager.lights.push_back(this);
    shadowMap = NULL;
    color = Vector3(1.0, 1.0, 1.0);
}

Light::~Light() {
    if(shadowMap) mainFBO.deleteTexture(shadowMap);
    for(int i = 0; i < lightManager.lights.size(); i ++)
        if(lightManager.lights[i] == this) {
            lightManager.lights.erase(lightManager.lights.begin()+i);
            return;
        }
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
    currentShaderProgram->setUniformF("lRange", range);
    currentShaderProgram->setUniformVec3("lColor", color);
    currentShaderProgram->setUniformVec3("lDirection", direction*-1.0);
}

void Light::selectShaderProgram(bool skeletal) {
    log(error_log, "Unreachable function called!");
}

bool LightPrioritySorter::operator()(Light* a, Light* b) {
    return a->getPriority(position) < b->getPriority(position);
}



DirectionalLight::DirectionalLight() {
    type = LightType_Directional;
    position = Vector3(0.0, 50.0, 0.0);
    direction = Vector3(0.0, -1.0, 0.0);
    upDir = Vector3(0.0, 0.0, 1.0);
    range = 100.0;
    width = height = 10.0;
    shadowCam.fov = 0.0;
    shadowCam.near = 1.0;
}

bool DirectionalLight::calculate(bool shadowActive) {
    shadowCam.camMat.setIdentity();
    shadowCam.camMat.setDirection(direction, upDir);
    shadowCam.camMat.translate(position);
    shadowCam.width = width;
    shadowCam.height = height;
    shadowCam.far = range;
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
    modelMat.scale(Vector3(width, height, range));
    modelMat *= shadowCam.camMat;
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.scale(Vector3(0.5, 0.5, 1.0));
        shadowMat.translate(Vector3(0.5, 0.5, 0.0));
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[directionalLightSP]->use();
    Light::use();
    
    if(drawLightVolume(directionalLightVertices, boxVerticesCount)) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[0]);
        glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[1]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
        glDrawElements(GL_TRIANGLES, boxTrianglesCount*3, GL_UNSIGNED_INT, 0);
    }
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
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
}

bool SpotLight::calculate(bool shadowActive) {
    shadowCam.camMat.setIdentity();
    shadowCam.camMat.setDirection(direction, upDir);
    shadowCam.camMat.translate(position);
    shadowCam.fov = cutoff*2.0;
    shadowCam.far = range;
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
    float radius = range*tan(cutoff)*1.05;
    modelMat.setIdentity();
    modelMat.translate(Vector3(0.0, 0.0, -1.0));
    modelMat.scale(Vector3(radius, radius, range));
    modelMat *= shadowCam.camMat;
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.scale(Vector3(0.5, 0.5, 1.0));
        shadowMat.translate(Vector3(0.5, 0.5, 0.0));
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 5);
    }else
        shaderPrograms[spotLightSP]->use();
    currentShaderProgram->setUniformF("lCutoff", cos(cutoff));
    currentShaderProgram->setUniformVec3("lPosition", position);
    Light::use();
    
    if(drawLightVolume(spotLightVertices, parabolidVerticesCount(coneAccuracy/2, 0))) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[2]);
        glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[3]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
        glDrawElements(GL_TRIANGLES, parabolidTrianglesCount(coneAccuracy, 0)*3, GL_UNSIGNED_INT, 0);
    }
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
    shadowCam.fov = M_PI_2;
    shadowCam.width = 1.0;
    shadowCam.height = 1.0;
    shadowCam.near = 0.1;
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) mainFBO.deleteTexture(shadowMapB);
}

bool PositionalLight::calculate(bool shadowActive) {
    shadowCam.camMat.setIdentity();
    shadowCam.camMat.setDirection(direction, upDir);
    shadowCam.camMat.translate(position);
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
        shadowCam.frustum.hemisphere = true;
        if(!Light::calculate(shadowActive)) return false;
        if(omniDirectional && !shadowMapB)
            shadowMapB = mainFBO.addTexture(1024, true, false);
    }
    
    glDisable(GL_BLEND);
    Matrix4 viewMat;
    if(cubemapsEnabled) {
        for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; side ++) {
            shadowCam.camMat.setIdentity();
            switch(side) {
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                    shadowCam.camMat.rotateZ(M_PI);
                    shadowCam.camMat.rotateY(M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                    shadowCam.camMat.rotateZ(M_PI);
                    shadowCam.camMat.rotateY(-M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                    shadowCam.camMat.rotateX(-M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                    shadowCam.camMat.rotateX(M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                    shadowCam.camMat.rotateZ(M_PI);
                    shadowCam.camMat.rotateY(M_PI);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                    shadowCam.camMat.rotateZ(M_PI);
                break;
            }
            shadowCam.camMat.setDirection(direction, upDir);
            shadowCam.camMat.translate(position);
            shadowCam.far = range;
            shadowCam.calculate();
            shadowCam.use();
            mainFBO.renderInTexture(shadowMap, side);
            objectManager.draw();
        }
        shadowCam.camMat.setIdentity();
        shadowCam.camMat.setDirection(direction, upDir);
        shadowCam.camMat.translate(position);
    }else{
        shaderPrograms[solidParabolidShadowSP]->use();
        shaderPrograms[solidParabolidShadowSP]->setUniformF("lRange", range);
        shaderPrograms[skeletalParabolidShadowSP]->use();
        shaderPrograms[skeletalParabolidShadowSP]->setUniformF("lRange", range);
        viewMat = shadowCam.camMat.getInverse();
        if(shadowMapB) {
            shadowCam.viewMat = viewMat;
            shadowCam.viewMat.scale(Vector3(-1.0, 1.0, -1.0));
            mainFBO.renderInTexture(shadowMapB, 0);
            objectManager.draw();
        }
        shadowCam.viewMat = viewMat;
        mainFBO.renderInTexture(shadowMap, 0);
        objectManager.draw();
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
    modelMat.rotateY(M_PI);
    modelMat.scale(Vector3(range*1.05, range*1.05, range*1.05));
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
    
    if(cubemapsEnabled) {
        Matrix4 viewMat;
        viewMat.setIdentity();
        viewMat.setDirection(direction, upDir);
        viewMat = viewMat.getInverse();
        currentShaderProgram->setUniformMatrix3("lShadowMat", &viewMat);
    }else
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam.viewMat);
    currentShaderProgram->setUniformF("lCutoff", (omniDirectional) ? -1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", position);
    Light::use();
    
    if(!cubemapsEnabled && !omniDirectional) {
        if(drawLightVolume(spotLightVertices, parabolidVerticesCount(sphereAccuracyX/2, parabolidAccuracyY/2))) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[4]);
            glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[5]);
            currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
            glDrawElements(GL_TRIANGLES, parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)*3, GL_UNSIGNED_INT, 0);
        }
        return;
    }
    
    Bs3 bs(&modelMat, range);
    if(!currentCam->frustum.testBsInclusiveHit(&bs)) return;
    if(currentCam->frustum.front.testBsExclusiveHit(&bs)) {
        glEnable(GL_DEPTH_TEST);
        glFrontFace(GL_CCW);
    }else{
        glDisable(GL_DEPTH_TEST);
        glFrontFace(GL_CW);
    }
    mainFBO.renderDeferred(false, inBuffersA, sizeof(inBuffersA)/sizeof(unsigned char), outBuffersA, sizeof(outBuffersA)/sizeof(unsigned char));
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[6]);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[7]);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
    glDrawElements(GL_TRIANGLES, sphereTrianglesCount(sphereAccuracyX, sphereAccuracyY)*3, GL_UNSIGNED_INT, 0);
}

void PositionalLight::selectShaderProgram(bool skeletal) {
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
    for(unsigned char i = 0; i < sizeof(lightVolumeBuffers)/sizeof(GLuint); i ++)
        glDeleteBuffers(1, &lightVolumeBuffers[i]);
}

void LightManager::init() {
    modelMat.setIdentity();
    Box3 directionalLightVolume(&modelMat, Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 0.0));
    Parabolid3 spotLightVolume(&modelMat, 1.0);
    Parabolid3 positionalLightVolume(&modelMat, 1.0);
    Bs3 positionalDualLightVolume(&modelMat, 1.0);
    
    unsigned int index, trianglesCount = (parabolidTrianglesCount(coneAccuracy, 0)+
                                parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)+
                                sphereTrianglesCount(sphereAccuracyX, sphereAccuracyY)),
                positionsCount = boxVerticesCount+parabolidVerticesCount(coneAccuracy, 0)+
                                 parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY)+
                                 sphereVerticesCount(sphereAccuracyX, sphereAccuracyY),
                *indecies = new unsigned int[trianglesCount*3];
    
    glGenBuffers(1, &lightVolumeBuffers[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxIndecies), boxIndecies, GL_STATIC_DRAW);
    
    index = spotLightVolume.getIndecies(indecies, coneAccuracy, 0);
    glGenBuffers(1, &lightVolumeBuffers[2]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    
    index = positionalLightVolume.getIndecies(indecies, sphereAccuracyX, parabolidAccuracyY);
    glGenBuffers(1, &lightVolumeBuffers[4]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[4]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    
    index = positionalDualLightVolume.getIndecies(indecies, sphereAccuracyX, sphereAccuracyY);
    glGenBuffers(1, &lightVolumeBuffers[6]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[6]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    delete [] indecies;
    
    float* vertices = new float[positionsCount*3];
    Vector3* positions = new Vector3[sphereVerticesCount(sphereAccuracyX, sphereAccuracyY)];
    index = copyVertices(positions, vertices, directionalLightVolume.getVertices(positions))*3;
    glGenBuffers(1, &lightVolumeBuffers[1]);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[1]);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(float), vertices, GL_STATIC_DRAW);
    
    index = copyVertices(positions, vertices, spotLightVolume.getVertices(positions, coneAccuracy, 0))*3;
    glGenBuffers(1, &lightVolumeBuffers[3]);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[3]);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(float), vertices, GL_STATIC_DRAW);
    
    index = copyVertices(positions,vertices, positionalLightVolume.getVertices(positions, sphereAccuracyX, parabolidAccuracyY))*3;
    glGenBuffers(1, &lightVolumeBuffers[5]);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[5]);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(float), vertices, GL_STATIC_DRAW);
    
    index = copyVertices(positions, vertices, positionalDualLightVolume.getVertices(positions, sphereAccuracyX, sphereAccuracyY))*3;
    glGenBuffers(1, &lightVolumeBuffers[7]);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[7]);
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(float), vertices, GL_STATIC_DRAW);
    
    delete [] positions;
    glBufferData(GL_ARRAY_BUFFER, index*sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] vertices;
    
    directionalLightVolume.getVertices(directionalLightVertices);
    spotLightVolume.getVertices(spotLightVertices, coneAccuracy/2, 0);
    positionalLightVolume.getVertices(positionalLightVertices, sphereAccuracyX/2, parabolidAccuracyY/2);
    
    randomNormalMap.width = 128;
    randomNormalMap.height = 128;
    randomNormalMap.loadRandomInRAM();
    randomNormalMap.uploadToVRAM(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
    randomNormalMap.unloadFromRAM();
}

void LightManager::clear() {
    for(unsigned int i = 0; i < lights.size(); i ++)
        delete lights[i];
    lights.clear();
    currentShadowLight = NULL;
}

void LightManager::calculateShadows(unsigned int maxShadows) {
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->camMat.pos;
    std::sort(lights.begin(), lights.end(), lightPrioritySorter);
    
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->calculate(i < maxShadows);
    
    currentShadowLight = NULL;
}

void LightManager::illuminate() {
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->use();
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
        glViewport(0, 0, videoInfo->current_w >> 1, videoInfo->current_h >> 1);
        randomNormalMap.use(GL_TEXTURE_2D, 1);
        shaderPrograms[ssaoSP]->use();
        mainFBO.renderDeferred(true, inBuffersB, 1, outBuffersB, 1);
        glViewport(0, 0, videoInfo->current_w, videoInfo->current_h);
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