//
//  Texture.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenGL/gl.h>
#import <SDL/SDL.h>
#import "Utilities.h"

#ifndef Texture_h
#define Texture_h

class Texture {
    public:
    unsigned int useCounter, width, height;
    GLuint GLname;
    GLenum minFilter, magFilter;
    SDL_Surface* surface;
    Texture();
    ~Texture();
    bool loadImageInRAM(const char* filePath);
    void loadRandomInRAM();
    void unloadFromRAM();
    bool uploadToVRAM(GLenum textureTarget, GLenum format);
    void unloadFromVRAM();
    void use(GLenum textureTarget, GLuint targetIndex);
};

#endif
