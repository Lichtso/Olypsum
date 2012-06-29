//
//  FBO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ShaderProgram.h"

FBO::~FBO() {
    for(unsigned int i = 0; i < colorBuffers.size(); i ++) {
        glDeleteTextures(1, &colorBuffers[i]->texture);
        delete colorBuffers[i];
    }
    for(unsigned char i = 0; i < dBuffersCount; i ++)
        glDeleteTextures(1, &dBuffers[i]);
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
}

int FBO::getColorBufferIndex(ColorBuffer* colorBuffer) {
    for(unsigned int i = 0; i < colorBuffers.size(); i ++)
        if(colorBuffers[i] == colorBuffer)
            return i;
    return -1;
}

void FBO::initBuffer(unsigned int index) {
    glGenTextures(1, &dBuffers[index]);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, dBuffers[index]);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void FBO::init() {
    maxFBOSize = max(videoInfo->current_w, videoInfo->current_h);
    maxFBOSize = pow(2, ceil(log(maxFBOSize) / log(2)));
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, maxFBOSize, maxFBOSize);
    
    initBuffer(colorDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA, videoInfo->current_w, videoInfo->current_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    initBuffer(positionDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA32F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGBA, GL_FLOAT, NULL);
    initBuffer(normalDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(materialDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    initBuffer(diffuseDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(specularDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::clearDeferredBuffers() {
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE_ARB, dBuffers[diffuseDBuffer], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE_ARB, dBuffers[specularDBuffer], 0);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1};
    glDrawBuffers(2, drawBuffers);
    glClearColor(0, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT);
}

void FBO::renderInDeferredBuffers() {
    glViewport(0, 0, videoInfo->current_w, videoInfo->current_h);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    for(unsigned char o = 0; o < 4; o ++)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+o, GL_TEXTURE_RECTANGLE_ARB, dBuffers[o], 0);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, drawBuffers);
    glClearColor(0, 0, 0, 1);
}

void FBO::renderDeferred(bool fillScreen, unsigned char* inBuffers, unsigned char inBuffersCount, unsigned char* outBuffers, unsigned char outBuffersCount) {
    if(outBuffersCount == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }else{
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        GLenum drawBuffers[outBuffersCount];
        for(unsigned char o = 0; o < outBuffersCount; o ++) {
            drawBuffers[o] = GL_COLOR_ATTACHMENT0+o;
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[o], GL_TEXTURE_RECTANGLE_ARB, dBuffers[outBuffers[o]], 0);
        }
        glDrawBuffers(outBuffersCount, drawBuffers);
    }
    
    for(unsigned char i = 0; i < inBuffersCount; i ++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_RECTANGLE_ARB, dBuffers[inBuffers[i]]);
    }
    
    if(!fillScreen) return;
    modelMat.setIdentity();
    currentShaderProgram->setUniformMatrix4("modelViewMat", &modelMat);
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

ColorBuffer* FBO::addTexture(unsigned int size) {
    if(colorBuffers.size() >= maxColorBufferCount) return NULL;
    GLuint texture = 0;
    glGenTextures(1, &texture);
    if(texture == 0) return NULL;
    ColorBuffer* colorBuffer = new ColorBuffer();
    colorBuffer->size = size;
    colorBuffer->texture = texture;
    glBindTexture(GL_TEXTURE_2D, colorBuffer->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_DEPTH_TEXTURE_MODE, GL_INTENSITY);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, colorBuffer->size, colorBuffer->size, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    colorBuffers.push_back(colorBuffer);
    return colorBuffer;
}

void FBO::renderInTexture(ColorBuffer* colorBuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, colorBuffer->texture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, colorBuffer->size, colorBuffer->size);
}

void FBO::mipmapTexture(ColorBuffer* colorBuffer) {
    glBindTexture(GL_TEXTURE_2D, colorBuffer->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void FBO::useTexture(ColorBuffer* colorBuffer, GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(GL_TEXTURE_2D, colorBuffer->texture);
}

void FBO::deleteTexture(ColorBuffer* colorBuffer) {
    int index = getColorBufferIndex(colorBuffer);
    glDeleteTextures(1, &colorBuffer->texture);
    colorBuffers.erase(colorBuffers.begin()+index);
    delete colorBuffer;
}

GLuint FBO::generateNormalMap(GLuint heightMap, unsigned int width, unsigned int height, float processingValue) {
    GLuint normalMap = 0;
    glGenTextures(1, &normalMap);
    if(normalMap == 0) return 0;
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_RECTANGLE_ARB, normalMap);
    glTexImage2D(GL_TEXTURE_RECTANGLE_ARB, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, width, height);
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    Matrix4 mat;
    currentShaderProgram->setUniformF("processingValue", processingValue);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDeleteTextures(1, &heightMap);
    return normalMap;
}

FBO mainFBO;
unsigned int maxFBOSize;