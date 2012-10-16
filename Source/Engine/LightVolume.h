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
    void draw();
};

class LightBoxVolume : public LightVolume {
    public:
    btVector3 halfSize;
    LightBoxVolume(btVector3 halfSize);
    void drawWireFrame(Color4 color);
    void draw();
};

class LightSphereVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightSphereVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    void drawWireFrame(Color4 color);
    void draw();
};

class LightParabolidVolume : public LightVolume {
    public:
    btScalar radius;
    unsigned int accuracyX, accuracyY;
    LightParabolidVolume(btScalar radius, unsigned int accuracyX, unsigned int accuracyY);
    void drawWireFrame(Color4 color);
    void draw();
};

#endif
