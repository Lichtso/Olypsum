//
//  FBO.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FBO.h"

FBO::FBO() {
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, maxFBOSize, maxFBOSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

FBO::~FBO() {
    for(unsigned int i = 0; i < colorBuffers.size(); i ++)
        glDeleteTextures(1, &colorBuffers[i].texture);
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteFramebuffers(1, &frameBuffer);
}

unsigned int FBO::addTexture(unsigned int size, bool resizedDisplay) {
    ColorBuffer colorBuffer;
    colorBuffer.resizedDisplay = resizedDisplay;
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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, colorBuffers[index].size, colorBuffers[index].size);
}

void FBO::useTexture(unsigned int index, GLuint targetIndex) {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(GL_TEXTURE_2D, colorBuffers[index].texture);
    if(colorBuffers[index].resizedDisplay) {
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    }else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
}

void FBO::deleteTexture(unsigned int index) {
    glDeleteTextures(1, &colorBuffers[index].texture);
    colorBuffers.erase(colorBuffers.begin()+index);
}

FBO* mainFBO;