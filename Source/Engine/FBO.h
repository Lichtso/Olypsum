//
//  FBO.h
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Texture.h"

#ifndef FBO_h
#define FBO_h

#define maxColorBufferCount 6
#define gBuffersCount 9

struct ColorBuffer {
    unsigned int size;
    bool shadowMap, cubeMap;
    GLuint texture;
};

class FBO {
    GLuint frameBuffer, gBuffers[gBuffersCount];
    int getColorBufferIndex(ColorBuffer* colorBuffer);
    void initBuffer(unsigned int index);
    public:
    unsigned int maxSize;
    std::vector<ColorBuffer*> colorBuffers;
    FBO();
    ~FBO();
    void init();
    void copyColorBuffer(unsigned char source, unsigned char destination);
    void renderInDeferredBuffers(bool transparent);
    void renderTransparentInDeferredBuffers();
    void renderDeferred(bool fillScreen, unsigned char* inBuffers, unsigned char inBuffersCount, unsigned char* outBuffers, unsigned char outBuffersCount);
    ColorBuffer* addTexture(unsigned int size, bool shadowMap, bool cubeMap);
    void renderInTexture(ColorBuffer* colorBuffer, GLenum side);
    void mipmapTexture(ColorBuffer* colorBuffer);
    void useTexture(ColorBuffer* colorBuffer, GLuint targetIndex);
    void deleteTexture(ColorBuffer* colorBuffer);
    bool generateNormalMap(Texture* heightMap, float processingValue);
};

enum DeferredBufferNames {
    depthDBuffer = 0,
    colorDBuffer = 1,
    materialDBuffer = 2,
    normalDBuffer = 3,
    positionDBuffer = 4,
    specularDBuffer = 5,
    diffuseDBuffer = 6,
    transparentDBuffer = 7,
    ssaoDBuffer = 8
};

extern FBO mainFBO;

#endif
