//
//  FBO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ObjectManager.h"
//#include "FileManager.h"

ColorBuffer::ColorBuffer(bool shadowMapB, bool cubeMapB, unsigned int widthB, unsigned int heightB)
    :shadowMap(shadowMapB), cubeMap(cubeMapB), width(widthB), height(heightB) {
    glGenTextures(1, &texture);
    
    GLenum textureTarget = (cubeMap) ? GL_TEXTURE_CUBE_MAP : ((width == height) ? GL_TEXTURE_2D : GL_TEXTURE_RECTANGLE);
    glBindTexture(textureTarget, texture);
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(textureTarget, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    if(cubeMap) glTexParameteri(textureTarget, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    if(shadowMap) {
        glTexParameteri(textureTarget, GL_TEXTURE_COMPARE_MODE,  GL_COMPARE_REF_TO_TEXTURE);
        glTexParameteri(textureTarget, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
        if(cubeMap) {
            for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; side ++)
                glTexImage2D(side, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        }else
            glTexImage2D(textureTarget, 0, GL_DEPTH_COMPONENT16, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    }else{
        if(cubeMap) {
            for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; side ++)
                glTexImage2D(side, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        }else
            glTexImage2D(textureTarget, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    }
    if(glGetError() == GL_OUT_OF_MEMORY)
        log(warning_log, "GL_OUT_OF_MEMORY");
}

ColorBuffer::~ColorBuffer() {
    glDeleteTextures(1, &texture);
}

void ColorBuffer::use(GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture((cubeMap) ? GL_TEXTURE_CUBE_MAP : GL_TEXTURE_2D, texture);
}

void ColorBuffer::mipmapTexture() {
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}



FBO::FBO() {
    for(unsigned char i = 0; i < gBuffersCount; i ++)
        gBuffers[i] = 0;
    frameBuffer = 0;
}

FBO::~FBO() {
    for(unsigned char i = 0; i < gBuffersCount; i ++)
        if(gBuffers[i])
            glDeleteTextures(1, &gBuffers[i]);
    glDeleteFramebuffers(1, &frameBuffer);
}

void FBO::initBuffer(unsigned int index) {
    glGenTextures(1, &gBuffers[index]);
    glBindTexture(GL_TEXTURE_RECTANGLE, gBuffers[index]);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
}

void FBO::init() {
    unsigned int width = optionsState.videoWidth * optionsState.videoScale,
                 height = optionsState.videoHeight * optionsState.videoScale;
    shadowMapSize = pow(2, ceil(log(max(width, height)) / log(2)));
    
    for(unsigned char i = 0; i < gBuffersCount; i ++)
        if(gBuffers[i])
            glDeleteTextures(1, &gBuffers[i]);
    if(frameBuffer)
        glDeleteFramebuffers(1, &frameBuffer);
    
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    
    initBuffer(depthDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_DEPTH_COMPONENT16,
                 width, height,
                 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    initBuffer(colorDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB,
                 width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    initBuffer(materialDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB,
                 width, height,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    initBuffer(normalDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F,
                 width, height,
                 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(positionDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB32F,
                 width, height,
                 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(specularDBuffer); //Needs shadowMapSize because it is used as color buffer for shadow map calculations
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB,
                 shadowMapSize, shadowMapSize,
                 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    initBuffer(diffuseDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB16F,
                 width, height,
                 0, GL_RGB, GL_UNSIGNED_SHORT, NULL);
    initBuffer(transparentDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGBA,
                 width, height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    initBuffer(ssaoDBuffer);
    glTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_R16F,
                 optionsState.videoWidth, optionsState.videoHeight,
                 0, GL_RED, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glBindTexture(GL_TEXTURE_RECTANGLE, 0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    shadowMapSize = min(1024U, shadowMapSize);
}

void FBO::copyBuffer(GLuint source, GLuint destination) {
    if(source == 0)
        glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    else{
        glBindFramebuffer(GL_READ_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_READ_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, source, 0);
    }
    if(destination == 0)
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    else{
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, frameBuffer);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, destination, 0);
    }
    glReadBuffer((source == 0) ? GL_BACK : GL_COLOR_ATTACHMENT0);
    glDrawBuffer((destination == 0) ? GL_BACK : GL_COLOR_ATTACHMENT1);
    glBlitFramebuffer(0, 0, optionsState.videoWidth*optionsState.videoScale, optionsState.videoHeight*optionsState.videoScale,
                      0, 0, optionsState.videoWidth*optionsState.videoScale, optionsState.videoHeight*optionsState.videoScale,
                      GL_COLOR_BUFFER_BIT, GL_NEAREST);
    glBindFramebuffer(GL_READ_FRAMEBUFFER, 0);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void FBO::renderInGBuffers(GLuint colorBuffer, bool specular) {
    glClearColor(0, 0, 0, 0);
    glViewport(0, 0, optionsState.videoWidth*optionsState.videoScale, optionsState.videoHeight*optionsState.videoScale);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3, GL_COLOR_ATTACHMENT4 };
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, gBuffers[depthDBuffer], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gBuffers[diffuseDBuffer], 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_RECTANGLE, gBuffers[specularDBuffer], 0);
    glDrawBuffers(2, drawBuffers);
    glClear(GL_COLOR_BUFFER_BIT);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, colorBuffer, 0);
    for(unsigned char o = 0; o < 4; o ++)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1+o, GL_TEXTURE_RECTANGLE, gBuffers[materialDBuffer+o], 0);
    if(colorBuffer == gBuffers[transparentDBuffer]) {
        //glDrawBuffers(3, drawBuffers); //Clear normalDBuffer, positionDBuffer too
        glDrawBuffers(1, drawBuffers);
        glClear(GL_COLOR_BUFFER_BIT);
    }else{
        glClear(GL_DEPTH_BUFFER_BIT);
    }
    glDrawBuffers((specular) ? 5 : 4, drawBuffers);
}

void FBO::renderInBuffers(bool fillScreen, GLuint* inBuffers, unsigned char inBuffersCount, GLuint* outBuffers, unsigned char outBuffersCount) {
    if(outBuffersCount) {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        GLenum* drawBuffers = new GLenum[outBuffersCount];
        for(unsigned char o = 0; o < outBuffersCount; o ++) {
            drawBuffers[o] = GL_COLOR_ATTACHMENT0+o;
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[o], GL_TEXTURE_RECTANGLE, outBuffers[o], 0);
        }
        glDrawBuffers(outBuffersCount, drawBuffers);
		delete[] drawBuffers;
    }else
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    for(unsigned char i = 0; i < inBuffersCount; i ++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_RECTANGLE, inBuffers[i]);
    }
    
    if(!fillScreen) return;
    rectVAO.draw();
}

void FBO::renderInTexture(ColorBuffer* colorBuffer, GLenum textureTarget) {
    glViewport(0, 0, colorBuffer->width, colorBuffer->height);
    
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    if(colorBuffer->shadowMap) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, textureTarget, colorBuffer->texture, 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_RECTANGLE, gBuffers[specularDBuffer], 0);
        glDrawBuffer(GL_NONE);
        glClear(GL_DEPTH_BUFFER_BIT);
    }else{
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_RECTANGLE, gBuffers[depthDBuffer], 0);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureTarget, colorBuffer->texture, 0);
        glDrawBuffer(GL_COLOR_ATTACHMENT0);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    }
}

FBO mainFBO;