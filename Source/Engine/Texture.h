//
//  Texture.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <OpenGL/gl3.h>
#include <SDL/SDL.h>
#include "XMLUtilities.h"

#ifndef Texture_h
#define Texture_h
#define GL_TEXTURE_MAX_ANISOTROPY 0x84FE
#define GL_MAX_TEXTURE_MAX_ANISOTROPY 0x84FF

class FilePackage;

//! A resource represented by a file in a FilePackage
class FilePackageResource {
    public:
    FilePackage* filePackage; //!< The parent FilePackage
    std::string name; //!< The name of the file
    FilePackageResource() :filePackage(NULL) { }
    virtual ~FilePackageResource();
    /*! Loads the file
     @param filePackage The FilePackage as parent
     @param name The file name to be loaded
     */
    virtual std::shared_ptr<FilePackageResource> load(FilePackage* filePackage, const std::string& name);
};

//! A texture used for graphics
class Texture : public FilePackageResource {
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
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackageB, const std::string& name);
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
};

#endif
