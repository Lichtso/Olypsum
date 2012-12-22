//
//  DisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Model.h"

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
    void setupModelObjectBones(BaseObject* object, Bone* bone);
    void updateSkeletonPose(BaseObject* object, Bone* bone);
    void drawBonePose(BaseObject* object, Bone* bone, float axesSize, float linesSize, float textSize);
    protected:
    ModelObject() :skeletonPose(NULL) { };
    public:
    std::shared_ptr<Model> model;
    bool gameTick();
    void draw();
    //! Draws a single mesh
    void drawAccumulatedMesh(Mesh* mesh);
    //! A debug draw of the skeleton
    void drawSkeletonPose(float axesSize, float linesSize, float textSize);
    //! Called by a Mesh to prepare the shader program to draw this ModelObject
    virtual void prepareShaderProgram(Mesh* mesh);
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
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
    void draw();
};

#define MAX_WAVES 4

//! A ModelObject to display water like effects
/*!
 A ModelObject to display water like effects
 TODO: Not tested yet.
 */
class WaterObject : public ModelObject {
    public:
    float waveSpeed;
    //! A wave in a WaterObject
    struct Wave {
        float length, //! The length of the wave
              age, //! The time in seconds until the wave will be deleted
              duration, //! The life duration in seconds
              ampitude, //! The height of the wave
              originX, //! The s coord of the center
              originY; //! The t coord of the center
    };
    std::vector<Wave> waves; //!< The waves on the surface of this WaterObject
    WaterObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~WaterObject();
    bool gameTick();
    void draw();
    void prepareShaderProgram(Mesh* mesh);
    /*! Adds a wave to the surface
     @param duration The time in seconds until the wave will be deleted
     @param ampitude The height of the wave
     @param length The length of the wave
     @param originX The s coord of the center
     @param originY The t coord of the center
     */
    void addWave(float duration, float ampitude, float length, float originX, float originY);
};

#endif