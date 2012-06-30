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
        printf("ERROR: %s\n", IMG_GetError());
        return false;
    }
    
    switch(surface->format->BitsPerPixel) {
        case 8:
        case 24:
        case 32:
            break;
        default:
            printf("Couldn't load image %s.\nERROR: Unsupported bit-depth %d.\n", filePath, surface->format->BitsPerPixel);
            return false;
    }
    
    if(surface->format->palette) {
        printf("Couldn't load image %s:\nERROR: Image uses a color palette.\n", filePath);
        return false;
    }
    
    width = surface->w;
    height = surface->h;
    
    return true;
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
            printf("Couldn't load texture to VRAM.\nERROR: Unsupported bit-depth.%d\n", surface->format->BitsPerPixel);
            return false;
    }
    
    if(surface->format->palette) {
        printf("Couldn't load texture to VRAM.\nERROR: Image uses a color palette.\n");
        return false;
    }
    
    width = surface->w;
    height = surface->h;
    
    glTexImage2D(textureTarget, 0, format, width, height, 0, readFormat, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(textureTarget, GL_TEXTURE_MIN_FILTER, (textureTarget == GL_TEXTURE_2D) ? minFilter : GL_NEAREST);
    glTexParameteri(textureTarget, GL_TEXTURE_MAG_FILTER, magFilter);
    if(textureTarget == GL_TEXTURE_2D)
        glGenerateMipmap(textureTarget);
    
    return true;
}

void Texture::unloadFromVRAM() {
    if(GLname) glDeleteTextures(1, &GLname);
    GLname = 0;
}

void Texture::use(GLuint targetIndex) {
    glActiveTexture(GL_TEXTURE0+targetIndex);
    glBindTexture(GL_TEXTURE_2D, GLname);
}