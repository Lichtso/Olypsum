//
//  Texture.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Texture.h"

Texture::Texture() {
    useCounter = 1;
    GLname = 0;
    minFilter = GL_LINEAR_MIPMAP_LINEAR;
    magFilter = GL_LINEAR;
    surface = NULL;
}

Texture::~Texture() {
    if(surface) SDL_FreeSurface(surface);
    if(GLname) glDeleteTextures(1, &GLname);
}

bool Texture::loadImageInRAM(const char* filePath) {
    if(surface) return false;
    
    surface = IMG_Load(filePath);
    if(!surface) {
        log(error_log, std::string("Unable to load texture ")+filePath+".\n"+IMG_GetError());
        return false;
    }
    
    switch(surface->format->BitsPerPixel) {
        case 8:
        case 24:
        case 32:
            break;
        default:
            log(error_log, std::string("Unable to load texture ")+filePath+".\nUnsupported bit-depth.");
            return false;
    }
    
    if(surface->format->palette) {
        log(error_log, std::string("Unable to load texture ")+filePath+".\nTexture uses a color palette.");
        return false;
    }
    
    width = surface->w;
    height = surface->h;
    
    return true;
}

void Texture::loadRandomInRAM() {
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

void Texture::unloadFromRAM() {
    if(surface) SDL_FreeSurface(surface);
    surface = NULL;
}

bool Texture::uploadToVRAM(GLenum textureTarget, GLenum format) {
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
    
    return true;
}

void Texture::unloadFromVRAM() {
    if(GLname) glDeleteTextures(1, &GLname);
    GLname = 0;
}

void Texture::use(GLenum textureTarget, GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(textureTarget, GLname);
}