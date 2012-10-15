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

class FilePackageResource {
    public:
    FilePackage* filePackage;
    std::string name;
    FilePackageResource() :filePackage(NULL) { }
    ~FilePackageResource();
    virtual std::shared_ptr<FilePackageResource> load(FilePackage* filePackage, const std::string& name);
};

class Texture : public FilePackageResource {
    public:
    unsigned int width, height;
    GLuint GLname;
    GLenum minFilter, magFilter;
    SDL_Surface* surface;
    Texture();
    ~Texture();
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackageB, const std::string& name);
    void loadRandom();
    void unloadImage();
    bool uploadTexture(GLenum textureTarget, GLenum format);
    bool uploadNormalMap(float processingValue);
    void unloadTexture();
    void use(GLenum textureTarget, GLuint targetIndex);
};

#endif
