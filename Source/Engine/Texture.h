//
//  Texture.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenGL/gl.h>
#import <SDL/SDL.h>
#import "Utilities.h"

#ifndef Texture_h
#define Texture_h

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
    unsigned int width, height; //!< The size of this Texture
    GLuint GLname; //!< The OpenGL identifier if the Texture is present in the VRAM
    GLenum minFilter, magFilter; //!< The OpenGL texture filters
    SDL_Surface* surface; //!< The SDL_Surface if the Texture is present in the RAM
    Texture();
    ~Texture();
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackageB, const std::string& name);
    //! Loads random data in a SDL_Surface
    void loadRandom();
    //! Deletes the SDL_Surface of the Texture (unload from RAM)
    void unloadImage();
    //! Uploads the SDL_Surface into the VRAM
    bool uploadTexture(GLenum textureTarget, GLenum format);
    //! Generates and uploades a normal map into the VRAM from a existing SDL_Surface
    bool uploadNormalMap(float processingValue);
    //! Deletes the OpenGL texture (unload from VRAM)
    void unloadTexture();
    /*! Sets the Texture as OpenGL texture target
     @param textureTarget GL_TEXTURE_2D, GL_TEXTURE_RECTANGLE_ARB or GL_TEXTURE_CUBE_MAP_x
     @param targetIndex A index between 0 and x will be a texture target between GL_TEXTURE0 and GL_TEXTUREx
     */
    void use(GLenum textureTarget, GLuint targetIndex);
};

#endif
