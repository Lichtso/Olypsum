//
//  FBO.h
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Texture.h"

#ifndef FBO_h
#define FBO_h

#define maxFBOSize 1024
#define maxColorBufferCount 8

struct ColorBuffer {
    unsigned int size;
    GLuint texture;
};

class FBO {
    GLuint frameBuffer, depthBuffer;
    public:
    std::vector<ColorBuffer> colorBuffers;
    ~FBO();
    void init();
    unsigned int addTexture(unsigned int size);
    void renderInTexture(unsigned int index);
    void mipmapTexture(unsigned int index);
    void blurTexture(unsigned int index, float blurFactor);
    void useTexture(unsigned int index, GLuint targetIndex);
    void deleteTexture(unsigned int index);
};

extern FBO mainFBO;

#endif
