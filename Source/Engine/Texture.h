//
//  Texture.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Texture_h
#define Texture_h
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY 0x84FF

#include "FileManager.h"

//! A class to store a color used for graphics
class Color4 {
    public:
    float r, //!< The red channel (0.0 - 1.0)
          g, //!< The green channel (0.0 - 1.0)
          b, //!< The blue channel (0.0 - 1.0)
          a; //!< The transparency channel (0.0 - 1.0)
    //! A black color
    Color4() : r(0), g(0), b(0), a(1) {};
    //! A gray color
    Color4(float gray) : r(gray), g(gray), b(gray), a(1) {};
    //! A transparent gray color
    Color4(float gray, float aB) : r(gray), g(gray), b(gray), a(aB) {};
    //! A rgb color
    Color4(float rB, float gB, float bB) : r(rB), g(gB), b(bB), a(1) {};
    //! A rgb color
    Color4(btVector3 vec) : r(vec.x()), g(vec.y()), b(vec.z()), a(1) {};
    //! A rgb color
    Color4(btQuaternion vec) : r(vec.x()), g(vec.y()), b(vec.z()), a(vec.w()) {};
    //! A transparent rgb color
    Color4(float rB, float gB, float bB, float aB) : r(rB), g(gB), b(bB), a(aB) {};
    Color4& operator=(const Color4& B) {
        r = B.r;
        g = B.g;
        b = B.b;
        a = B.a;
        return *this;
    }
    //! Converts this Color4 to a btVector3
    btVector3 getVector() {
        return btVector3(r, g, b);
    }
    //! Converts this Color4 to a btQuaternion
    btQuaternion getQuaternion() {
        return btQuaternion(r, g, b, a);
    }
    //! Converts this Color4 to a SDL_Color
    SDL_Color getSDL() {
        SDL_Color B;
        B.r = r*255;
        B.g = g*255;
        B.b = b*255;
        B.a = a*255;
        return B;
    }
};

//! A texture used for graphics
class Texture : public FileResource {
    public:
    //! A single frame used for texture animation
    struct AnimationFrame {
        float accBegin, //!< The acceleration at t = 0
              accEnd, //!< The acceleration at t = duration
              duration; //!< The duration of this frame in seconds
    };
    AnimationFrame* frames;
    unsigned int width, height, depth; //!< The size of this Texture
    GLuint GLname; //!< The OpenGL identifier if the Texture is present in the VRAM
    GLenum minFilter, magFilter; //!< The OpenGL texture filters
    SDL_Surface* surface; //!< The SDL_Surface if the Texture is present in the RAM
    Texture();
    ~Texture();
    FileResourcePtr<FileResource> load(FilePackage* filePackageB, const std::string& name);
    //! Prepare to be uploaded as a 3D Texture, returns success
    bool setAnimationFrames(std::vector<AnimationFrame> frames);
    //! Loads random data in a SDL_Surface
    void loadRandom();
    //! Deletes the SDL_Surface of the Texture (unload from RAM)
    void unloadImage();
    //! Calculates the GL format of the surface
    GLenum getSurfacesGLFormat();
    //! Uploads the SDL_Surface into the VRAM
    bool uploadTexture(GLenum textureTarget, GLenum format);
    //! Generates and uploades a mipmap into the VRAM from a existing SDL_Surface
    void uploadMipMap(GLenum textureTarget, GLenum format);
    //! Generates and uploades a normal map into the VRAM from a existing SDL_Surface
    bool uploadNormalMap(float processingValue);
    //! Deletes the OpenGL texture (unload from VRAM)
    void unloadTexture();
    //! Updates the OpenGL texture filters
    void updateFilters(GLenum textureTarget = 0);
    /*! Sets the Texture as OpenGL 2D texture target
     @param targetIndex A index between 0 and x will be a texture target between GL_TEXTURE0 and GL_TEXTUREx
     */
    void use(GLuint targetIndex);
    /*! Sets the Texture as OpenGL 3D texture target
     @param targetIndex A index between 0 and x will be a texture target between GL_TEXTURE0 and GL_TEXTUREx
     @param animationTime A float that is used to store the time of animation in seconds
     */
    void use(GLuint targetIndex, float& animationTime);
    /*! Unbindes the texture slot
     @param targetIndex A index between 0 and x will be a texture target between GL_TEXTURE0 and GL_TEXTUREx
     @param textureTarget The OpenGL texture target like GL_TEXTURE_2D or GL_TEXTURE_2D_ARRAY
     */
    static void unbind(GLuint targetIndex, GLenum textureTarget) {
        glActiveTexture(GL_TEXTURE0+targetIndex);
        glBindTexture(textureTarget, 0);
    };
};

#endif
