//
//  LightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LightObject_h
#define LightObject_h

#include "LightVolume.h"

void initLightVolumes();

class LightObject : public DisplayObject {
    protected:
    ColorBuffer* shadowMap;
    void setPhysicsShape(btCollisionShape* shape);
    LightObject();
    ~LightObject();
    public:
    CamObject shadowCam;
    Color4 color;
    void removeClean();
    btTransform getTransformation();
    float getRange();
    virtual bool generateShadowMap(bool shadowActive);
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
    void setBounds(btVector3 bounds);
    btVector3 getBounds();
    bool generateShadowMap(bool shadowActive);
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
    float getCutoff();
    bool generateShadowMap(bool shadowActive);
    void draw();
    float getPriority(btVector3 position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

class PositionalLight : public LightObject {
    ColorBuffer* shadowMapB;
    ~PositionalLight();
    public:
    PositionalLight();
    PositionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    void setBounds(bool omniDirectional, float range);
    bool getOmniDirectional();
    bool generateShadowMap(bool shadowActive);
    void draw();
    void deleteShadowMap();
    float getPriority(btVector3 position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif