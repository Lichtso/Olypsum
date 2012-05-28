//
//  FBO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ShaderProgram.h"

FBO::~FBO() {
    for(unsigned int i = 0; i < colorBuffers.size(); i ++)
        glDeleteTextures(1, &colorBuffers[i].texture);
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
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

unsigned int FBO::addTexture(unsigned int size) {
    ColorBuffer colorBuffer;
    colorBuffer.size = size;
    glGenTextures(1, &colorBuffer.texture);
    glBindTexture(GL_TEXTURE_2D, colorBuffer.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, colorBuffer.size, colorBuffer.size, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    colorBuffers.push_back(colorBuffer);
    return colorBuffers.size()-1;
}

void FBO::renderInTexture(unsigned int index) {
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffers[index].texture, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    glViewport(0, 0, colorBuffers[index].size, colorBuffers[index].size);
}

void FBO::mipmapTexture(unsigned int index) {
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void FBO::blurTexture(unsigned int index, float blurFactor) {
    GLuint auxTexture;
    glGenTextures(1, &auxTexture);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, auxTexture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glCopyTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 0, 0, colorBuffers[index].size, colorBuffers[index].size, 0);
    
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

void FBO::useTexture(unsigned int index, GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[index].texture);
}

void FBO::deleteTexture(unsigned int index) {
    glDeleteTextures(1, &colorBuffers[index].texture);
    colorBuffers.erase(colorBuffers.begin()+index);
}

FBO mainFBO;
RenderingState renderingState;