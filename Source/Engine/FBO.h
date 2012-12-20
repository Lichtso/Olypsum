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

//! A off screen texture used for background rendering
struct ColorBuffer {
    unsigned int size; //!< The width and height in pixels
    bool shadowMap, //!< Is it used as a shadow map
         cubeMap; //!< Is it a cube map (has 6 sides)
    GLuint texture; //!< The OpenGL identifier
    /*! Creates a new ColorBuffer
     @param size The width and height of this color buffer (should be 2^x)
     @param shadowMap True if it is a shadow map
     @param cubeMap True if it is a cube map
     */
    ColorBuffer(unsigned int size, bool shadowMap, bool cubeMap);
    ~ColorBuffer();
    void use(GLuint targetIndex);
    void mipmapTexture();
};

//! The frame buffer object used for graphics
class FBO {
    //! Internaly used to set up a g-buffer
    void initBuffer(unsigned int index);
    public:
    GLuint frameBuffer, //!< The OpenGL identifier of the frame buffer
           gBuffers[gBuffersCount]; //!< The OpenGL identifiers of the g-buffers
    unsigned int maxSize; //! The maximal width and height of a color buffer
    FBO();
    ~FBO();
    //! Initialize
    void init();
    //! Copies the content from the g-buffer source to the g-buffer destination
    void copyGBuffer(unsigned char source, unsigned char destination);
    /*! Prepares the g-buffers to be rendered in
     @param transparent True if this method is called by renderTransparentInDeferredBuffers()
     */
    void renderInDeferredBuffers(bool transparent);
    //! Renders all transparent meshes which have been accumulated
    void renderTransparentInDeferredBuffers();
    /*! Prepares the g-buffers to be rendered in and read out
     @param fillScreen If true this method will render the entire screen else it will only prepare the g-buffers
     @param inBuffers A array of g-buffer indecies to be used as textures. The index of each element will also be its texture target index
     @param inBuffersCount The count of inBuffers elements which will be used
     @param outBuffers A array of g-buffer indecies to be used as rendering targets
     @param outBuffersCount The count of outBuffers elements which will be used
     */
    void renderDeferred(bool fillScreen, unsigned char* inBuffers, unsigned char inBuffersCount, unsigned char* outBuffers, unsigned char outBuffersCount);
    /*! Prepares a ColorBuffer to be rendered in
     @param colorBuffer The ColorBuffer used as rendering target
     @param textureTarget The texture target used to bind the ColorBuffer
     */
    void renderInTexture(ColorBuffer* colorBuffer, GLenum textureTarget);
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
