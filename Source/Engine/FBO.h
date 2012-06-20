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
    int getColorBufferIndex(ColorBuffer* colorBuffer);
    public:
    std::vector<ColorBuffer*> colorBuffers;
    ~FBO();
    void init();
    ColorBuffer* addTexture(unsigned int size);
    void renderInTexture(ColorBuffer* colorBuffer);
    void mipmapTexture(ColorBuffer* colorBuffer);
    void blurTexture(ColorBuffer* colorBuffer, float blurFactor);
    void useTexture(ColorBuffer* colorBuffer, GLuint targetIndex);
    void deleteTexture(ColorBuffer* colorBuffer);
};

extern FBO mainFBO;

enum RenderingState {
    RenderingShadow,
    RenderingScreen
};

extern RenderingState renderingState;

#endif
