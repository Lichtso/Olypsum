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

void Texture::uploadToVRAM() {
    if(!surface) return;
    
    if(!GLname) glGenTextures(1, &GLname);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, GLname);
    
    unsigned int formatA, formatB;
    switch(surface->format->BitsPerPixel) {
        case 8:
            formatA = formatB = GL_LUMINANCE;
            break;
        case 24:
            formatA = GL_RGB;
            formatB = GL_BGR;
            break;
        case 32:
            formatA = GL_RGBA;
            formatB = GL_BGRA;
            break;
        default:
            printf("Couldn't load texture to VRAM.\nERROR: Unsupported bit-depth.%d\n", surface->format->BitsPerPixel);
            return;
    }
    
    if(surface->format->palette) {
        printf("Couldn't load texture to VRAM.\nERROR: Image uses a color palette.\n");
        return;
    }
    
    width = surface->w;
    height = surface->h;
    
    glTexImage2D(GL_TEXTURE_2D, 0, formatA, width, height, 0, formatB, GL_UNSIGNED_BYTE, surface->pixels);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
    glGenerateMipmap(GL_TEXTURE_2D);
}

void Texture::unloadFromVRAM() {
    if(GLname) glDeleteTextures(1, &GLname);
    GLname = 0;
}

void Texture::use(GLuint index) {
    glActiveTexture(GL_TEXTURE0+index);
    glBindTexture(GL_TEXTURE_2D, GLname);
}