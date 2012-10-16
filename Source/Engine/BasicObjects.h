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
    protected:
    ObjectBase();
    public:
    ~ObjectBase();
    virtual void gameTick();
    virtual btTransform getTransformation();
};

class PhysicObject : public ObjectBase {
    protected:
    PhysicObject();
    public:
    virtual void physicsTick();
    virtual void handleCollision(btPersistentManifold* contactManifold, PhysicObject* b);
};

class SkeletonPose;
class Mesh;
class Model;

class GraphicObject : public ObjectBase {
    protected:
    GraphicObject();
    public:
    virtual void draw();
};

class ModelObject : public GraphicObject {
    protected:
    ModelObject(std::shared_ptr<Model> model);
    public:
    SkeletonPose* skeletonPose;
    std::shared_ptr<Model> model;
    ~ModelObject();
    void draw();
    virtual void prepareShaderProgram(Mesh* mesh);
    virtual bool prepareDraw();
};

#endif