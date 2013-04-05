//
//  DisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Model.h"

#ifndef DisplayObject_h
#define DisplayObject_h

//! A PhysicObject which is visible on screen
/*!
 This is the basic class for all Objects, which are visible on screen.
 @warning Don't use it directly
 */
class DisplayObject : public PhysicObject {
    protected:
    DisplayObject() { };
    public:
    bool inFrustum; //!< This flag is set by the engine when the DisplayObject was in the frustum at the last graphics frame
    //! Called by the engine to render this DisplayObject
    virtual void draw() = 0;
};

//! A transparent DisplayObject
/*!
 A transparent DisplayObject which is accumulated by the engine to be rendered at the end of the graphics frame
 
 @warning Don't use it directly
 */
struct AccumulatedTransparent {
    DisplayObject* object;
    Mesh* mesh;
};

//! A DisplayObject which is managed by the ObjectManager
/*!
 This is the basic class for all Objects, which are managed by the ObjectManager.
 @warning Don't use it directly
 */
class GraphicObject : public DisplayObject {
    protected:
    GraphicObject();
    public:
    void remove();
};

//! A GraphicObject with a Model
/*!
 This is the basic class for all Objects with a Model.
 @warning Don't use it directly
 */
class ModelObject : public GraphicObject {
    btTransform* skeletonPose;
    void setupBones(BaseObject* object, Bone* bone);
    void writeBones(rapidxml::xml_document<char> &doc, LevelSaver* levelSaver,
                    rapidxml::xml_node<xmlUsedCharType>* node, BoneObject *object);
    void updateSkeletonPose(BaseObject* object, Bone* bone);
    protected:
    ModelObject() :skeletonPose(NULL), integrity(1.0) { };
    public:
    ~ModelObject();
    void newScriptInstance();
    bool gameTick();
    float integrity; //! Health <= 0.0: destroyed
    std::vector<float> textureAnimation; //!< Animation time for each mesh;
    std::shared_ptr<Model> model;
    //! Overwrites the model and cleans the textureAnimation
    void setModel(std::shared_ptr<Model> model);
    //! Draws the entire model with all meshes
    void draw();
    //! Draws a single mesh
    void drawAccumulatedMesh(Mesh* mesh);
    //! Called by a Mesh to prepare the shader program to draw this ModelObject
    virtual void prepareShaderProgram(Mesh* mesh);
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A reflective ModelObject
/*!
 A reflective ModelObject which is stored by the engine to be rendered at the beginning of the graphics frame
 
 @warning Don't use it directly
 */
class Reflective {
    protected:
    Reflective(ModelObject* objectB, Mesh* meshB);
    public:
    virtual ~Reflective();
    ColorBuffer* buffer;
    ModelObject* object;
    Mesh* mesh;
    //! Recalculates the reflection in a tick, returns false if it is out of view
    virtual bool gameTick() = 0;
};

//! A Reflective mirroring at a given plane
class PlaneReflective : public Reflective {
    public:
    btVector3 plane;
    PlaneReflective(ModelObject* object, Mesh* mesh);
    bool gameTick();
};

//! A Reflective mirroring the environment using a cube map
class EnvironmentReflective : public Reflective {
    public:
    EnvironmentReflective(ModelObject* object, Mesh* mesh);
    bool gameTick();
};

//! A GraphicObject with a soft-physics-body
/*!
 TODO: This class is not implemented yet.
 */
class SoftObject : public GraphicObject {
    public:
    SoftObject();
    ~SoftObject();
    
    btSoftBody* getBody() {
        return static_cast<btSoftBody*>(body);
    }
};

//! A simple implementation of btMotionState
class simpleMotionState : public btMotionState {
    public:
    btTransform transformation; //!< The transformation used for graphics
    simpleMotionState(const btTransform& trans) :transformation(trans) { }
    virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
    virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans);
};

//! A simpleMotionState for a RigidObject with a shifted center of mass
class comMotionState : public simpleMotionState {
    btTransform centerOfMassOffset; //!< The center of mass used for shifting between physics and graphics
    public:
    comMotionState(const btTransform& trans, const btTransform& COMO = btTransform::getIdentity())
    :simpleMotionState(trans), centerOfMassOffset(COMO) { }
    virtual void getWorldTransform(btTransform& centerOfMassWorldTrans) const;
    virtual void setWorldTransform(const btTransform& centerOfMassWorldTrans);
};

//! A ModelObject with a rigid-physics-body
/*!
 This is the basic class for all Objects with a rigid-physics-body.
 TODO: Skeletal animation is not implemented yet.
 */
class RigidObject : public ModelObject {
    public:
    RigidObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~RigidObject();
    void setTransformation(const btTransform& transformation);
    btTransform getTransformation();
    btRigidBody* getBody() {
        return static_cast<btRigidBody*>(body);
    }
    void newScriptInstance();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif