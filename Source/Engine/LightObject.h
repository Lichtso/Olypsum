//
//  LightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef LightObject_h
#define LightObject_h

#include "LightVolume.h"

//! Initializes the static LightVolumes
void initLightVolumes();

//! A VisualObject used as light source
class LightObject : public VisualObject {
    protected:
    ColorBuffer* shadowMap; //!< Optional shadow map if used for shadow casting
    //! Replaces the collision shape and deletes the old one if present
    void setPhysicsShape(btCollisionShape* shape);
    LightObject();
    ~LightObject();
    public:
    CamObject shadowCam; //!< A CamObject used for shadow casting and defining the light volume
    Color4 color; //!< The color of the light
    void removeClean();
    btTransform getTransformation();
    //! Returns the maximal length of a light ray emitted by this light source
    float getRange();
    //! Gernerates or deletes the shadowmap(s) depending on the parameter
    virtual bool updateShadowMap(bool shadowActive);
    void draw();
    //! Deletes the shadowmap(s) if present
    virtual void deleteShadowMap();
    //! Returns the priority of this light source in the shadow casting ranking
    virtual float getPriority(const btVector3& position) = 0;
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A LightObject with a box shape and parallel light
class DirectionalLight : public LightObject {
    public:
    DirectionalLight();
    DirectionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    //! Updates the box shape to the given half extends
    void setBounds(const btVector3& bounds);
    //! Returns the half extends of the box shape
    btVector3 getBounds();
    bool updateShadowMap(bool shadowActive);
    void draw();
    float getPriority(const btVector3& position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A LightObject with a parabolid or sphere / dual parabolid (omni directional) shape and divergent light
class PositionalLight : public LightObject {
    ColorBuffer* shadowMapB; //!< The second shadow map used if omni directional
    ~PositionalLight();
    public:
    PositionalLight();
    PositionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    void setBounds(bool omniDirectional, float range);
    //! Returns true if the shape is a sphere and false if it is a parabolid
    bool getOmniDirectional();
    bool updateShadowMap(bool shadowActive);
    void draw();
    void deleteShadowMap();
    float getPriority(const btVector3& position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A LightObject with a cone shape and divergent light
class SpotLight : public LightObject {
    public:
    SpotLight();
    SpotLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void setTransformation(const btTransform& transformation);
    //! Updates the cone shape
    void setBounds(float cutoff, float range);
    //! Returns the cutoff angle of the cone shape in radians
    float getCutoff();
    bool updateShadowMap(bool shadowActive);
    void draw();
    float getPriority(const btVector3& position);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif