//
//  LightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "LightVolume.h"
#include "DisplayObject.h"

#ifndef LightObject_h
#define LightObject_h

void initLightVolumes();

class LightObject : public DisplayObject {
    protected:
    ColorBuffer* shadowMap;
    void setPhysicsShape(btCollisionShape* shape);
    LightObject();
    public:
    Cam shadowCam;
    Color4 color;
    ~LightObject();
    void remove();
    btTransform getTransformation();
    virtual bool gameTick(bool shadowActive);
    void draw();
    virtual void deleteShadowMap();
    virtual float getPriority(btVector3 position) = 0;
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

class DirectionalLight : public LightObject {
    public:
    DirectionalLight();
    DirectionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    void setBounds(float width, float height, float range);
    bool gameTick(bool shadowActive);
    void draw();
    float getPriority(btVector3 position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

class SpotLight : public LightObject {
    public:
    SpotLight();
    SpotLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    void setBounds(float cutoff, float range);
    bool gameTick(bool shadowActive);
    void draw();
    float getPriority(btVector3 position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

class PositionalLight : public LightObject {
    ColorBuffer* shadowMapB;
    public:
    PositionalLight();
    PositionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~PositionalLight();
    void setTransformation(const btTransform& transformation);
    void setBounds(bool omniDirectional, float range);
    bool gameTick(bool shadowActive);
    void draw();
    void deleteShadowMap();
    float getPriority(btVector3 position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif