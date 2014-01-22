//
//  VisualObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef VisualObject_h
#define VisualObject_h

#include "Model.h"

//! A PhysicObject which is visible on screen
/*!
 This is the basic class for all Objects, which are visible on screen.
 @warning Don't use it directly
 */
class VisualObject : public PhysicObject {
    protected:
    VisualObject() { }
    public:
    bool inFrustum; //!< This flag is set by the engine when the VisualObject was in the frustum at the last graphics frame
    //! Called by the engine to render this VisualObject
    virtual void draw() = 0;
};

//! A transparent VisualObject
/*!
 A transparent VisualObject which is accumulated by the engine to be rendered at the end of the graphics frame
 
 @warning Don't use it directly
 */
struct AccumulatedTransparent {
    VisualObject* object;
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

//! A VisualObject which is managed by the ObjectManager
/*!
 This is the basic class for all Objects, which are managed by the ObjectManager.
 @warning Don't use it directly
 */
class MatterObject : public VisualObject {
    protected:
    MatterObject();
    public:
    float integrity; //! Health <= 0.0: destroyed
    std::unordered_set<Decal*> decals; //!< Decals attached to this MatterObject
    void removeClean();
    void removeFast();
    bool gameTick();
    void draw();
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

//! A MatterObject with a soft-physics-body
/*!
 TODO: This class is not implemented yet.
 */
class SoftObject : public MatterObject {
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

//! A MatterObject with a rigid-physics-body
/*!
 This is the basic class for all Objects with a rigid-physics-body.
 */
class RigidObject : public MatterObject {
    public:
    std::unique_ptr<btTransform> skeletonPose; //!< Pose of the skeleton if the model has one
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
    //! Removes the model, textureAnimation, skeletonPose and all childs connected via BoneLinks
    void removeModel();
    //! Overwrites the model
    void setModel(LevelLoader* levelLoader, FileResourcePtr<Model> model);
    //! Draws the entire model with all meshes
    void draw();
    //! Draws a single mesh
    void drawAccumulatedMesh(Mesh* mesh);
    //! Called by a Mesh to prepare the shader program to draw this ModelObject
    void prepareShaderProgram(Mesh* mesh);
    /*! Returns the BoneLink of a given bone name
     @param name The name of the bone in the COLLADA model
     @return BoneLink if present or NULL if not
     */
    BoneLink* findBoneLinkOfName(const char* name);
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
    ColorBuffer* buffer; //!< The ColorBuffer used to store the reflection
    RigidObject* object; //!< The mirroring RigidObject
    Mesh* mesh; //!< The mirroring Mesh of the object
    //! Recalculates the reflection in a tick, returns false if it is out of view
    virtual bool gameTick() = 0;
};

//! A Reflective mirroring at a given plane
class PlaneReflective : public Reflective {
    public:
    btVector3 plane; //!< The at the origin reflected plane of the mirror
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