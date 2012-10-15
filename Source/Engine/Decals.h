//
//  Decals.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

#ifndef Decals_h
#define Decals_h

class Decal {
    public:
    Matrix4 transformation;
    std::shared_ptr<Texture> diffuse, heightMap;
    float life;
};

class DecalManager {
    GLuint vbo;
    public:
    std::vector<Decal*> decals;
    ~DecalManager();
    void init();
    void clear();
    void gameTick();
    void draw();
};

extern DecalManager decalManager;

#endif