//
//  Decals.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

Decal::Decal() {
    diffuse = heightMap = NULL;
}

Decal::~Decal() {
    if(diffuse) fileManager.releaseTexture(diffuse);
    if(heightMap) fileManager.releaseTexture(heightMap);
}

DecalManager::~DecalManager() {
    glDeleteBuffers(1, &vbo);
}

void DecalManager::init() {
    float vertices[] = {
        -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0
    };
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void DecalManager::calculate() {
    for(int i = 0; i < decals.size(); i ++) {
        decals[i]->life -= animationFactor;
        if(decals[i]->life > 0.0) continue;
        decals.erase(decals.begin()+i);
        i --;
    }
}

void DecalManager::draw() {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    unsigned int byteStride = 8*sizeof(float);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(0*sizeof(float)));
    currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(3*sizeof(float)));
    currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(5*sizeof(float)));
    Bs3 bs3(modelMat, 1.0);
    for(int i = 0; i < decals.size(); i ++) {
        bs3.transformation = modelMat = decals[i]->transformation;
        if(!currentCam->frustum.testBsInclusiveHit(&bs3)) continue;
        
        if(decals[i]->diffuse)
            decals[i]->diffuse->use(GL_TEXTURE_2D, 0);
        
        if(decals[i]->heightMap) {
            decals[i]->heightMap->use(GL_TEXTURE_2D, 2);
            shaderPrograms[solidBumpGeometrySP]->use();
        }else{
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            shaderPrograms[solidGeometrySP]->use();
        }
        currentShaderProgram->setUniformF("discardDensity", min(1.0, decals[i]->life*0.5));
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

DecalManager decalManager;