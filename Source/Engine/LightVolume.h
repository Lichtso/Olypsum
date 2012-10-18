//
//  LightVolume.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"
#import "LinearMaths.h"

#ifndef LightVolume_h
#define LightVolume_h

class LightVolume {
    protected:
    GLuint vbo, ibo;
    public:
    LightVolume();
    ~LightVolume();
    void init();
    virtual std::unique_ptr<btScalar[]> getVertices(unsigned int& verticesCount);
    virtual std::unique_ptr<unsigned int[]> getIndecies(unsigned int& trianglesCount);
    void drawWireFrameBegin();
    void drawWireFrameEnd();
    virtual void drawWireFrame(Color4 color);
    virtual void draw();
};

class LightBoxVolume : public LightVolume {
    public:
    btVector3 halfSize;
    LightBoxVolume(btVector3 halfSize);
    std::unique_ptr<btScalar[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& trianglesCount);
    void drawWireFrame(Color4 color);
    void draw();
};

class LightSphereVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightSphereVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<btScalar[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& trianglesCount);
    void drawWireFrame(Color4 color);
    void draw();
};

class LightParabolidVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightParabolidVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<btScalar[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& trianglesCount);
    void drawWireFrame(Color4 color);
    void draw();
};

#endif
