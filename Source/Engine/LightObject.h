//
//  LightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LightVolume.h"
#import "DisplayObject.h"

#ifndef LightObject_h
#define LightObject_h

void initLightVolumes();

class LightObject : public DisplayObject {
    protected:
    ColorBuffer* shadowMap;
    void setPhysicsShape(btCollisionShape* shape);
    public:
    Cam shadowCam;
    Color4 color;
    LightObject();
    ~LightObject();
    void remove();
    btTransform getTransformation();
    virtual bool gameTick(bool shadowActive);
    void draw();
    virtual void prepareShaderProgram(bool skeletal) = 0;
    virtual float getPriority(btVector3 position) = 0;
};

class LightPrioritySorter {
    public:
    btVector3 position;
    bool operator()(LightObject* a, LightObject* b);
};

class DirectionalLight : public LightObject {
    public:
    DirectionalLight();
    void setTransformation(const btTransform& transformation);
    void setBounds(float width, float height, float range);
    bool gameTick(bool shadowActive);
    void draw();
    virtual void prepareShaderProgram(bool skeletal);
    virtual float getPriority(btVector3 position);
};

class SpotLight : public LightObject {
    public:
    SpotLight();
    void setTransformation(const btTransform& transformation);
    void setBounds(float cutoff, float range);
    bool gameTick(bool shadowActive);
    void draw();
    virtual void prepareShaderProgram(bool skeletal);
    virtual float getPriority(btVector3 position);
};

class PositionalLight : public LightObject {
    ColorBuffer* shadowMapB;
    public:
    PositionalLight();
    ~PositionalLight();
    void setTransformation(const btTransform& transformation);
    void setBounds(bool omniDirectional, float range);
    bool gameTick(bool shadowActive);
    void draw();
    virtual void prepareShaderProgram(bool skeletal);
    virtual float getPriority(btVector3 position);
};

#endif