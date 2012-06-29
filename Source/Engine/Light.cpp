//
//  Light.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Game.h"

#define coneAccuracy 12
#define sphereAccuracyX 10
#define sphereAccuracyY 6
#define parabolidAccuracyY 3

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
unsigned char inBuffersB[] = { colorDBuffer, materialDBuffer, diffuseDBuffer, specularDBuffer }, outBuffersB[] = { };

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
    mainFBO.renderDeferred(false, inBuffersA, sizeof(inBuffersA)/sizeof(unsigned char), outBuffersA, sizeof(outBuffersA)/sizeof(unsigned char));
    return true;
}



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
    currentShaderProgram->setUniformVec3("lDirection", direction*-1.0);
}

void Light::selectShaderProgram(bool skeletal) {
    printf("ERROR: Unreachable function called!\n");
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
}

bool DirectionalLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(position);
    shadowCam->width = width;
    shadowCam->height = height;
    shadowCam->fov = 0.0;
    shadowCam->near = 1.0;
    shadowCam->far = range;
    shadowCam->calculate();
    shadowCam->use();
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
    modelMat.setIdentity();
    modelMat.scale(Vector3(width, height, range));
    modelMat *= shadowCam->camMat;
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam->viewMat;
        shadowMat.scale(Vector3(0.5, 0.5, 1.0));
        shadowMat.translate(Vector3(0.5, 0.5, 0.0));
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 8);
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
}

bool SpotLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(position);
    shadowCam->width = 1.0;
    shadowCam->height = 1.0;
    shadowCam->fov = cutoff*2.0;
    shadowCam->near = 0.1;
    shadowCam->far = range;
    shadowCam->calculate();
    shadowCam->use();
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
    float radius = range*tan(cutoff)*1.05;
    modelMat.setIdentity();
    modelMat.translate(Vector3(0.0, 0.0, -1.0));
    modelMat.scale(Vector3(radius, radius, range));
    modelMat *= shadowCam->camMat;
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam->viewMat;
        shadowMat.scale(Vector3(0.5, 0.5, 1.0));
        shadowMat.translate(Vector3(0.5, 0.5, 0.0));
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        mainFBO.useTexture(shadowMap, 8);
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
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) mainFBO.deleteTexture(shadowMapB);
}

bool PositionalLight::calculateShadowmap() {
    if(!Light::calculateShadowmap()) return false;
    if(omniDirectional && !shadowMapB)
        shadowMapB = mainFBO.addTexture(1024);
    
    shaderPrograms[solidParabolidShadowSP]->use();
    shaderPrograms[solidParabolidShadowSP]->setUniformF("lRange", range);
    shaderPrograms[skeletalParabolidShadowSP]->use();
    shaderPrograms[skeletalParabolidShadowSP]->setUniformF("lRange", range);
    shadowCam->camMat.setIdentity();
    shadowCam->camMat.setDirection(direction, upDir);
    shadowCam->camMat.translate(position);
    shadowCam->use();
    shadowCam->frustum.hemisphere = true;
    glDisable(GL_BLEND);
    glClearColor(1, 1, 1, 1);
    if(shadowMapB) {
        shadowMatB.setIdentity();
        shadowMatB.setDirection(direction*-1.0, upDir); //TODO: Optimize with scale -1
        shadowMatB.translate(position);
        shadowMatB = shadowMatB.getInverse();
        shadowCam->viewMat = shadowMatB;
        mainFBO.renderInTexture(shadowMapB);
        renderScene();
    }
    shadowCam->viewMat = shadowCam->camMat.getInverse();
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
    modelMat.setIdentity();
    modelMat.rotateY(M_PI);
    modelMat.scale(Vector3(range*1.05, range*1.05, range*1.05));
    modelMat *= shadowCam->camMat;
    
    if(shadowMap) {
        if(shadowMapB) {
            shaderPrograms[positionalShadowDualLightSP]->use();
            mainFBO.useTexture(shadowMapB, 9);
        }else
            shaderPrograms[positionalShadowLightSP]->use();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam->viewMat);
        mainFBO.useTexture(shadowMap, 8);
    }else
        shaderPrograms[positionalLightSP]->use();
    currentShaderProgram->setUniformF("lCutoff", (omniDirectional) ? -1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", position);
    Light::use();
    
    if(omniDirectional) {
        Bs3 bs(range, &modelMat);
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
    }else if(drawLightVolume(spotLightVertices, parabolidVerticesCount(sphereAccuracyX/2, parabolidAccuracyY/2))) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[4]);
        glBindBuffer(GL_ARRAY_BUFFER, lightVolumeBuffers[5]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
        glDrawElements(GL_TRIANGLES, parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)*3, GL_UNSIGNED_INT, 0);
    }
}

void PositionalLight::selectShaderProgram(bool skeletal) {
    if(skeletal)
        shaderPrograms[skeletalParabolidShadowSP]->use();
    else
        shaderPrograms[solidParabolidShadowSP]->use();
}



void LightManager::init() {
    Box3 directionalLightVolume(Vector3(-1.0, -1.0, -1.0), Vector3(1.0, 1.0, 0.0), &modelMat);
    Parabolid3 spotLightVolume(1.0, &modelMat);
    Parabolid3 positionalLightVolume(1.0, &modelMat);
    Bs3 positionalDualLightVolume(1.0, &modelMat);
    
    unsigned int index, trianglesCount = (boxTrianglesCount+parabolidTrianglesCount(coneAccuracy, 0)+
                                parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)+
                                sphereTrianglesCount(sphereAccuracyX, sphereAccuracyY)),
                positionsCount = boxVerticesCount+parabolidVerticesCount(coneAccuracy, 0)+
                                 parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY)+
                                 sphereVerticesCount(sphereAccuracyX, sphereAccuracyY),
                *indecies = new unsigned int[trianglesCount*3];
    
    index = directionalLightVolume.getIndecies(indecies);
    glGenBuffers(1, &lightVolumeBuffers[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumeBuffers[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, index*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    
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
    modelMat.setIdentity();
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
    //positionalDualLightVolume.getVertices(positionalDualLightVertices, sphereAccuracyX/2, sphereAccuracyY/2);
}

LightManager::~LightManager() {
    for(unsigned char i = 0; i < sizeof(lightVolumeBuffers)/sizeof(GLuint); i ++)
        glDeleteBuffers(1, &lightVolumeBuffers[i]);
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
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    mainFBO.clearDeferredBuffers();
    
    glDepthMask(GL_FALSE);
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->use();
    glDepthMask(GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(true, inBuffersB, sizeof(inBuffersB)/sizeof(unsigned char), outBuffersB, sizeof(outBuffersB)/sizeof(unsigned char));
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

LightManager lightManager;