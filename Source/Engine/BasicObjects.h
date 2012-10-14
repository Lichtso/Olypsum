//
//  BasicObjects.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "ShaderProgram.h"

#ifndef BasicObjects_h
#define BasicObjects_h

class ObjectBase {
    public:
    ObjectBase();
    ~ObjectBase();
    virtual void gameTick();
    virtual Matrix4 getTransformation();
};

class PhysicObject : public ObjectBase {
    public:
    virtual void physicsTick();
};

class RigidObject : public PhysicObject {
    btRigidBody* body;
    public:
    RigidObject(btRigidBody::btRigidBodyConstructionInfo* rBCI);
    ~RigidObject();
    Matrix4 getTransformation();
};

class ZoneObject : public RigidObject {
    public:
    
};

class SkeletonPose;
class Model;

class GraphicObject : public ObjectBase {
    public:
    SkeletonPose* skeletonPose;
    std::shared_ptr<Model> model;
    GraphicObject(Model* model);
    ~GraphicObject();
    virtual void prepareShaderProgram();
    virtual void draw();
};

#endif