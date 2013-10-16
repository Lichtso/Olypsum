//
//  LightVolume.h
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LightVolume_h
#define LightVolume_h

#include "ParticlesObject.h"

class LightVolume {
    protected:
    VertexArrayObject vao;
    public:
    void init();
    void drawWireFrameBegin();
    void drawWireFrameEnd();
    virtual std::unique_ptr<float[]> getVertices(unsigned int& verticesCount) = 0;
    virtual std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount) = 0;
    virtual void drawDebug(Color4 color);
    virtual void draw();
};

class LightBoxVolume : public LightVolume {
    public:
    btVector3 halfSize;
    LightBoxVolume(btVector3 halfSize);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

class FrustumVolume : public LightVolume {
    public:
    Ray3 bounds[4];
    float length;
    FrustumVolume(Ray3 bounds[4], float length);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

class LightSphereVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightSphereVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

class LightParabolidVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightParabolidVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    std::unique_ptr<float[]> getVertices(unsigned int& verticesCount);
    std::unique_ptr<unsigned int[]> getIndecies(unsigned int& elementsCount);
};

#endif
