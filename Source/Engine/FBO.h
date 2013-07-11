//
//  FBO.h
//  Olypsum
//
//  Created by Alexander Meißner on 17.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "VertexArrayObject.h"

#ifndef FBO_h
#define FBO_h

#define maxColorBufferCount 6
#define gBuffersCount 9

//! A off screen texture used for background rendering
struct ColorBuffer {
    unsigned int width, height; //!< The width and height in pixels
    bool shadowMap, //!< Is it used as a shadow map
         cubeMap; //!< Is it a cube map (has 6 sides)
    GLuint texture; //!< The OpenGL identifier
    /*! Creates a new ColorBuffer
     @param size The width and height of this color buffer (should be 2^x)
     @param shadowMap True if it is a shadow map
     @param cubeMap True if it is a cube map
     */
    ColorBuffer(bool shadowMapB, bool cubeMapB, unsigned int width, unsigned int height);
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
    //! Copies the content from the buffer source to the buffer destination
    void copyBuffer(GLuint source, GLuint destination);
    /*! Prepares the g-buffers to be rendered in
     @param colorBuffer The buffer used to store the color
     @param specular Shall the specular buffer be a render target
     */
    void renderInGBuffers(GLuint colorBuffer, bool specular);
    /*! Prepares buffers to be rendered in and read out
     @param fillScreen If true this method will render the entire screen else it will only prepare the g-buffers
     @param inBuffers A array of buffers to be used as textures. The index of each element will also be its texture target index
     @param inBuffersCount The count of inBuffers elements which will be used
     @param outBuffers A array of buffers to be used as rendering targets
     @param outBuffersCount The count of outBuffers elements which will be used
     */
    void renderInBuffers(bool fillScreen, GLuint* inBuffers, unsigned char inBuffersCount, GLuint* outBuffers, unsigned char outBuffersCount);
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
