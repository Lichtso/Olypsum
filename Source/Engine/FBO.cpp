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
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
}

int FBO::getColorBufferIndex(ColorBuffer* colorBuffer) {
    for(unsigned int i = 0; i < colorBuffers.size(); i ++)
        if(colorBuffers[i] == colorBuffer)
            return i;
    return -1;
}

void FBO::init() {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, maxFBOSize, maxFBOSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, colorBuffer->size, colorBuffer->size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    colorBuffers.push_back(colorBuffer);
    return colorBuffer;
}

void FBO::renderInTexture(ColorBuffer* colorBuffer) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer->texture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, colorBuffer->size, colorBuffer->size);
}

void FBO::mipmapTexture(ColorBuffer* colorBuffer) {
    glBindTexture(GL_TEXTURE_2D, colorBuffer->texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void FBO::blurTexture(ColorBuffer* colorBuffer, float blurFactor) {
    GLuint auxTexture;
    glGenTextures(1, &auxTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, auxTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, colorBuffer->size, colorBuffer->size, 0);
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    float vertices[12] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    };
    blurShaderProgram->use();
    blurShaderProgram->setUniformF("blurWidth", blurFactor);
    blurShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    
    glDeleteTextures(1, &auxTexture);
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

FBO mainFBO;
RenderingState renderingState;