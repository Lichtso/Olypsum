//
//  Texture.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <SDL_image/SDL_image.h>
#import "ShaderProgram.h"
#import "FileManager.h"

Texture::Texture() :GLname(0), surface(NULL), minFilter(GL_LINEAR_MIPMAP_LINEAR), magFilter(GL_LINEAR) {
        
}

Texture::~Texture() {
    if(surface) SDL_FreeSurface(surface);
    if(GLname) glDeleteTextures(1, &GLname);
}

std::shared_ptr<FilePackageResource> Texture::load(FilePackage* filePackageB, const std::string& name) {
    auto pointer = FilePackageResource::load(filePackageB, name);
    if(surface) return NULL;
    
    std::string filePath = filePackage->getUrlOfFile("Textures", name);
    surface = IMG_Load(filePath.c_str());
    if(!surface) {
        log(error_log, std::string("Unable to load texture ")+filePath+".\n"+IMG_GetError());
        return NULL;
    }
    
    switch(surface->format->BitsPerPixel) {
        case 8:
        case 24:
        case 32:
            break;
        default:
            log(error_log, std::string("Unable to load texture ")+filePath+".\nUnsupported bit-depth.");
            return NULL;
    }
    
    if(surface->format->palette) {
        log(error_log, std::string("Unable to load texture ")+filePath+".\nTexture uses a color palette.");
        return NULL;
    }
    
    width = surface->w;
    height = surface->h;
    
    return pointer;
}

void Texture::loadRandom() {
    if(surface) return;
    
    surface = SDL_CreateRGBSurface(SDL_SWSURFACE, width, height, 24, 0, 0, 0, 0);
    if(!surface) {
        log(error_log, std::string("Unable to create texture\n")+IMG_GetError());
        return;
    }
    
    SDL_LockSurface(surface);
    unsigned char* pixels = (unsigned char*) surface->pixels;
    for(unsigned int i = 0; i < 3*width*height; i ++)
        pixels[i] = rand()%255;
    SDL_UnlockSurface(surface);
}

void Texture::unloadImage() {
    if(!surface) return;
    SDL_FreeSurface(surface);
    surface = NULL;
}

bool Texture::uploadTexture(GLenum textureTarget, GLenum format) {
    if(!surface) return false;
    
    if(!GLname) glGenTextures(1, &GLname);
    if(!GLname) return false;
    glBindTexture(textureTarget, GLname);
    
    GLenum readFormat;
    switch(surface->format->BitsPerPixel) {
        case 8:
            readFormat = GL_LUMINANCE;
            break;
        case 24:
            readFormat = GL_BGR;
            break;
        case 32:
            readFormat = GL_BGRA;
            break;
        default:
            log(error_log, "Couldn't load texture to VRAM.\nUnsupported bit-depth.");
            return false;
    }
    
    if(surface->format->palette) {
        log(error_log, "Couldn't load texture to VRAM.\nTexture uses a color palette.");
        return false;
    }
    
    width = surface->w;
    height = surface->h;
    
    glTexImage2D(textureTarget, 0, format, width, height, 0, readFormat, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, (textureTarget == GL_TEXTURE_2D) ? minFilter : GL_NEAREST);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, magFilter);
    if(textureTarget == GL_TEXTURE_2D)
        glGenerateMipmap(textureTarget);
    
    GLint compressed;
    glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed);
    if(!compressed)
        log(info_log, "Texture has not been compressed.");
    
    unloadImage();
    return true;
}

bool Texture::uploadNormalMap(float processingValue) {
    if(!uploadTexture(GL_TEXTURE_RECTANGLE_ARB, GL_LUMINANCE)) return false;
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_RECTANGLE_ARB, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    
    GLuint normalMap;
    glGenTextures(1, &normalMap);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO.frameBuffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, normalMap, 0);
    glDrawBuffer(GL_COLOR_ATTACHMENT0);
    
    shaderPrograms[normalMapGenSP]->use();
    currentShaderProgram->setUniformF("processingValue", processingValue);
    float vertices[12] = { -1.0, -1.0, 1.0, -1.0, 1.0, 1.0, -1.0, 1.0 };
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 2*sizeof(float), vertices);
    glDisable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    unloadTexture();
    glBindTexture(GL_TEXTURE_2D, normalMap);
    glGenerateMipmap(GL_TEXTURE_2D);
    GLname = normalMap;
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    
    return true;
}

void Texture::unloadTexture() {
    if(GLname) glDeleteTextures(1, &GLname);
    GLname = 0;
}

void Texture::use(GLenum textureTarget, GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(textureTarget, GLname);
}