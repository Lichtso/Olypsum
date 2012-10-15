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
    //virtual void physicsTick();
    virtual void handleCollision(btPersistentManifold* contactManifold, PhysicObject* b);
};

class ZoneObject : public PhysicObject {
    public:
    btCollisionObject* body;
    ZoneObject(btCollisionShape* shape, const btTransform& transform);
    ~ZoneObject();
    Matrix4 getTransformation();
};

class SkeletonPose;
class Mesh;
class Model;

class GraphicObject : public ObjectBase {
    public:
    SkeletonPose* skeletonPose;
    std::shared_ptr<Model> model;
    GraphicObject(std::shared_ptr<Model> model);
    ~GraphicObject();
    virtual void prepareShaderProgram(Mesh* mesh);
    virtual bool prepareDraw();
};

#endif