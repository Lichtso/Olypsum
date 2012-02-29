//
//  Texture.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Frustum3.h"

#ifndef Texture_h
#define Texture_h

class Texture {
    public:
    GLuint GLname;
    GLenum minFilter, magFilter;
    SDL_Surface *surface;
    Texture();
    ~Texture();
    void loadImageInRAM(const char* fileName);
    void unloadFromRAM();
    void uploadToVRAM();
    void unloadFromVRAM();
    void use();
};

#endif
