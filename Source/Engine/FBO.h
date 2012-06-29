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

#define maxColorBufferCount 6
#define dBuffersCount 6

struct ColorBuffer {
    unsigned int size;
    GLuint texture;
};

class FBO {
    GLuint frameBuffer, depthBuffer, dBuffers[dBuffersCount];
    int getColorBufferIndex(ColorBuffer* colorBuffer);
    void initBuffer(unsigned int index);
    public:
    std::vector<ColorBuffer*> colorBuffers;
    ~FBO();
    void init();
    void clearDeferredBuffers();
    void renderInDeferredBuffers();
    void renderDeferred(bool fillScreen, unsigned char* inBuffers, unsigned char inBuffersCount, unsigned char* outBuffers, unsigned char outBuffersCount);
    ColorBuffer* addTexture(unsigned int size);
    void renderInTexture(ColorBuffer* colorBuffer);
    void mipmapTexture(ColorBuffer* colorBuffer);
    void useTexture(ColorBuffer* colorBuffer, GLuint targetIndex);
    void deleteTexture(ColorBuffer* colorBuffer);
    GLuint generateNormalMap(GLuint heightMap, unsigned int width, unsigned int height, float processingValue);
};

enum DeferredBufferNames {
    colorDBuffer = 0,
    positionDBuffer = 1,
    normalDBuffer = 2,
    materialDBuffer = 3,
    diffuseDBuffer = 4,
    specularDBuffer = 5
};

extern FBO mainFBO;
extern unsigned int maxFBOSize;

#endif
