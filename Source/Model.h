//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "InputController.h"

#ifndef Model_h
#define Model_h

class Mesh {
    public:
    GLuint vboF, vboI, ibo;
    unsigned int elementsCount;
    int postions, texcoords, normals, tangents, bitangents;
    Texture *diffuse, *normalMap;
    Mesh();
    ~Mesh();
    void draw();
};

class Model {
    public:
    unsigned int useCounter;
    std::vector<Mesh*> meshes;
    Model();
    ~Model();
    bool loadCollada(const char* filePath);
    void draw();
};

#endif