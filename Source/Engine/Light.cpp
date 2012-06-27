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

Vector3 directionalLightVertices[boxVerticesCount];
Vector3 spotLightVertices[parabolidVerticesCount(coneAccuracy, 0)];
Vector3 positionalLightVertices[parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY)];
Vector3 positionalDualLightVertices[sphereVerticesCount(sphereAccuracyX, sphereAccuracyY)];

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
    mainFBO.renderDeferred(inBuffersA, sizeof(inBuffersA)/sizeof(unsigned char), outBuffersA, sizeof(outBuffersA)/sizeof(unsigned char));
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
    modelMat.translate(Vector3(0,0,-1));
    //modelMat.scale(Vector3(width, height, range));
    //modelMat *= shadowCam->camMat;
    
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
        //currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), 0);
        //glDrawElements(GL_TRIANGLES, boxTrianglesCount*3, GL_UNSIGNED_INT, 0);
        
        glFrontFace(GL_CCW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        float vertices[] = {
            1.0, -1.0, 1.0,
            -1.0, -1.0, 1.0,
            1.0, 1.0, 1.0,
            -1.0, 1.0, 1.0
        };
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), vertices);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    shadowCam->near = 1.0;
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
    float radius = 1.0/tan(cutoff);
    modelMat.setIdentity();
    modelMat.translate(Vector3(0.0, 0.0, 1.0));
    modelMat.scale(Vector3(radius, radius, range*-1.0));
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
    
    if(drawLightVolume(spotLightVertices, parabolidVerticesCount(coneAccuracy, 0))) {
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), (GLfloat*)(boxVerticesCount*3*sizeof(float)));
        glDrawElements(GL_TRIANGLES, boxTrianglesCount*3, GL_UNSIGNED_INT, (GLuint*)(boxTrianglesCount*3*sizeof(unsigned int)));
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
    modelMat.scale(Vector3(range, range, range));
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
        if(drawLightVolume(spotLightVertices, sphereVerticesCount(sphereAccuracyX, sphereAccuracyY))) {
            currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float),
            (GLfloat*)((boxVerticesCount+parabolidVerticesCount(coneAccuracy, 0)+parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY))*3*sizeof(float)));
            glDrawElements(GL_TRIANGLES, sphereTrianglesCount(sphereAccuracyX, sphereAccuracyY)*3, GL_UNSIGNED_INT,
            (GLuint*)((boxTrianglesCount+parabolidTrianglesCount(coneAccuracy, 0)+parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY))*3*sizeof(unsigned int)));
        }
    }else if(drawLightVolume(spotLightVertices, parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY))) {
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float),
        (GLfloat*)((boxVerticesCount+parabolidVerticesCount(coneAccuracy, 0))*3*sizeof(float)));
        glDrawElements(GL_TRIANGLES, parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)*3, GL_UNSIGNED_INT,
        (GLuint*)((boxTrianglesCount+parabolidTrianglesCount(coneAccuracy, 0))*3*sizeof(unsigned int)));
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
    
    glGenBuffers(1, &lightVolumesIbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumesIbo);
    unsigned int index, *indecies = new unsigned int[(boxTrianglesCount+parabolidTrianglesCount(coneAccuracy, 0)+
                                 parabolidTrianglesCount(sphereAccuracyX, parabolidAccuracyY)+
                                 sphereTrianglesCount(sphereAccuracyX, sphereAccuracyY))*3],
                positionsCount = boxVerticesCount+parabolidVerticesCount(coneAccuracy, 0)+
                                 parabolidVerticesCount(sphereAccuracyX, parabolidAccuracyY)+
                                 sphereVerticesCount(sphereAccuracyX, sphereAccuracyY);
    index = directionalLightVolume.getIndecies(indecies);
    index += spotLightVolume.getIndecies(&indecies[index], coneAccuracy, 0);
    index += positionalLightVolume.getIndecies(&indecies[index], sphereAccuracyX, parabolidAccuracyY);
    positionalDualLightVolume.getIndecies(&indecies[index], sphereAccuracyX, sphereAccuracyY);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    delete [] indecies;
    
    glGenBuffers(1, &lightVolumesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumesVbo);
    float* vertices = new float[positionsCount*3];
    Vector3* positions = new Vector3[sphereVerticesCount(sphereAccuracyX, sphereAccuracyY)];
    modelMat.setIdentity();
    index = copyVertices(positions, vertices, directionalLightVolume.getVertices(positions));
    index += copyVertices(positions, &vertices[index], spotLightVolume.getVertices(positions, coneAccuracy, 0));
    index += copyVertices(positions, &vertices[index], positionalLightVolume.getVertices(positions, sphereAccuracyX, parabolidAccuracyY));
    copyVertices(positions, &vertices[index], positionalDualLightVolume.getVertices(positions, sphereAccuracyX, sphereAccuracyY));
    delete [] positions;
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] vertices;
    
    directionalLightVolume.getVertices(directionalLightVertices);
    spotLightVolume.getVertices(spotLightVertices, coneAccuracy/2, 0);
    positionalLightVolume.getVertices(positionalLightVertices, sphereAccuracyX/2, parabolidAccuracyY/2);
    positionalDualLightVolume.getVertices(positionalDualLightVertices, sphereAccuracyX/2, sphereAccuracyY/2);
}

LightManager::~LightManager() {
    glDeleteBuffers(1, &lightVolumesVbo);
    glDeleteBuffers(1, &lightVolumesIbo);
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
    glBindBuffer(GL_ARRAY_BUFFER, lightVolumesVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, lightVolumesIbo);
    for(unsigned int i = 0; i < lights.size(); i ++)
        lights[i]->use();
    glDepthMask(GL_TRUE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(inBuffersB, sizeof(inBuffersB)/sizeof(unsigned char), outBuffersB, sizeof(outBuffersB)/sizeof(unsigned char));
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    modelMat.setIdentity();
    currentShaderProgram->setUniformMatrix4("modelViewMat", &modelMat);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

LightManager lightManager;