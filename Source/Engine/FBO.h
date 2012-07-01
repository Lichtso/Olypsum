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
#define dBuffersCount 7

struct ColorBuffer {
    unsigned int size;
    GLuint texture;
};

class FBO {
    GLuint frameBuffer, dBuffers[dBuffersCount];
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
    bool generateNormalMap(Texture* heightMap, float processingValue);
};

enum DeferredBufferNames {
    depthDBuffer = 0,
    colorDBuffer = 1,
    normalDBuffer = 2,
    materialDBuffer = 3,
    positionDBuffer = 4,
    diffuseDBuffer = 5,
    specularDBuffer = 6
};

extern FBO mainFBO;
extern unsigned int maxFBOSize;
extern bool positionBufferEnabled;

#endif
