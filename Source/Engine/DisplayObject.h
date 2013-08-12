//
//  DisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef DisplayObject_h
#define DisplayObject_h

#include "Model.h"

//! A PhysicObject which is visible on screen
/*!
 This is the basic class for all Objects, which are visible on screen.
 @warning Don't use it directly
 */
class DisplayObject : public PhysicObject {
    protected:
    DisplayObject() { }
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

//! A decal used for graphic effects
class Decal {
    public:
    btTransform transformation; //!< The world transformation of this decal
    FileResourcePtr<Texture> diffuse, //!< A color texture
                             heightMap; //!< A height map
    float life; //!< The remaining life time in seconds
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
    float integrity; //! Health <= 0.0: destroyed
    std::set<Decal*> decals; //!< Decals attached to this GraphicObject
    void removeClean();
    void removeFast();
    bool gameTick();
    void draw();
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A GraphicObject with a soft-physics-body
/*!
 TODO: This class is not implemented yet.
 */
class SoftObject : public GraphicObject {
    public:
    SoftObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    void removeFast();
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
class RigidObject : public GraphicObject {
    std::unique_ptr<btTransform> skeletonPose; //!< Poses of the BoneObjects (if model has a skeleton) for OpenGL
    void setupBones(LevelLoader* levelLoader, BaseObject* object, Bone* bone);
    void writeBones(rapidxml::xml_document<char> &doc, LevelSaver* levelSaver, BoneObject *object);
    void updateSkeletonPose(BoneObject* object, Bone* bone);
    public:
    RigidObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    FileResourcePtr<Model> model; //!< The shared model
    std::vector<float> textureAnimationTime; //!< Animation time for each mesh
    void removeClean();
    void removeFast();
    void setTransformation(const btTransform& transformation);
    btTransform getTransformation();
    btRigidBody* getBody() {
        return static_cast<btRigidBody*>(body);
    }
    bool gameTick();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
    //! Returns true if the btCollisionObject::CF_KINEMATIC_OBJECT flag is set
    bool getKinematic();
    //! Sets or removes the btCollisionObject::CF_KINEMATIC_OBJECT flag
    void setKinematic(bool active);
    //! Finds the root bone (if model has a skeleton else returns NULL)
    BoneObject* getRootBone();
    //! Overwrites the model and cleans the textureAnimation
    void setModel(LevelLoader* levelLoader, FileResourcePtr<Model> model);
    //! Draws the entire model with all meshes
    void draw();
    //! Draws a single mesh
    void drawAccumulatedMesh(Mesh* mesh);
    //! Called by a Mesh to prepare the shader program to draw this ModelObject
    virtual void prepareShaderProgram(Mesh* mesh);
};

//! A reflective RigidObject
/*!
 A reflective RigidObject which is stored by the engine to be rendered at the beginning of the graphics frame
 
 @warning Don't use it directly
 */
class Reflective {
    protected:
    Reflective(RigidObject* objectB, Mesh* meshB);
    public:
    virtual ~Reflective();
    ColorBuffer* buffer;
    RigidObject* object;
    Mesh* mesh;
    //! Recalculates the reflection in a tick, returns false if it is out of view
    virtual bool gameTick() = 0;
};

//! A Reflective mirroring at a given plane
class PlaneReflective : public Reflective {
    public:
    btVector3 plane;
    PlaneReflective(RigidObject* object, Mesh* mesh);
    bool gameTick();
};

//! A Reflective mirroring the environment using a cube map
class EnvironmentReflective : public Reflective {
    public:
    EnvironmentReflective(RigidObject* object, Mesh* mesh);
    bool gameTick();
};

#endif