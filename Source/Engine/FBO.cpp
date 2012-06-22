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
    glBindTexture(GL_TEXTURE_2D, dBuffers[index]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void FBO::init() {
    maxFBOSize = max(videoInfo->current_w, videoInfo->current_h);
    maxFBOSize = pow(2, ceil(log(maxFBOSize) / log(2)));
    glGenFramebuffers(1, &frameBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    glGenRenderbuffers(1, &depthBuffer);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, maxFBOSize, maxFBOSize);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    
    initBuffer(colorDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, videoInfo->current_w, videoInfo->current_h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    initBuffer(positionDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGBA, GL_FLOAT, NULL);
    initBuffer(normalDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(materialDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    initBuffer(diffuseDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    initBuffer(specularDBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F_ARB, videoInfo->current_w, videoInfo->current_h, 0, GL_RGB, GL_FLOAT, NULL);
    
    //printf("glCheckFramebufferStatus %d\n", glCheckFramebufferStatus(GL_FRAMEBUFFER));
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FBO::renderInDeferredBuffers() {
    glViewport(0, 0, videoInfo->current_w, videoInfo->current_h);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
    for(unsigned char o = 0; o < 4; o ++)
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+o, GL_TEXTURE_2D, dBuffers[o], 0);
    GLenum drawBuffers[] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2, GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, drawBuffers);
}

void FBO::renderDeferred(std::vector<unsigned char>* inBuffers, std::vector<unsigned char>* outBuffers) {
    if(outBuffers->size() == 0) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }else{
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);
        GLenum drawBuffers[outBuffers->size()];
        for(unsigned char o = 0; o < outBuffers->size(); o ++) {
            drawBuffers[o] = GL_COLOR_ATTACHMENT0+o;
            glFramebufferTexture2D(GL_FRAMEBUFFER, drawBuffers[o], GL_TEXTURE_2D, dBuffers[(*outBuffers)[o]], 0);
        }
        //for(unsigned char o = outBuffers->size(); o < 4; o ++)
        //    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+o, GL_TEXTURE_2D, 0, 0);
        glDrawBuffers(outBuffers->size(), drawBuffers);
    }
    
    for(unsigned char i = 0; i < inBuffers->size(); i ++) {
        glActiveTexture(GL_TEXTURE0+i);
        glBindTexture(GL_TEXTURE_2D, dBuffers[(*inBuffers)[i]]);
        /*bool trought = false;
        for(unsigned char o = 0; o < outBuffers->size(); o ++)
            if((*inBuffers)[i] == (*outBuffers)[o]) {
                trought = true;
                break;
            }
        if(!trought) continue;
        */
    }
    
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    float vertices[12] = {
        0.0, 0.0,
        1.0, 0.0,
        1.0, 1.0,
        0.0, 1.0
    };
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, colorBuffer->size, colorBuffer->size, 0, GL_RED, GL_FLOAT, NULL);
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
unsigned int maxFBOSize;