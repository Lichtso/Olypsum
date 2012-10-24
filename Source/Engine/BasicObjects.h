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
    virtual ~ObjectBase();
    virtual void gameTick() { };
    virtual btTransform getTransformation() = 0;
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
class TransparentMesh;
class Model;

class GraphicObject : public ObjectBase {
    protected:
    GraphicObject();
    bool isInFrustum(btCollisionObject* body);
    public:
    virtual void draw() = 0;
};

class ModelObject : public GraphicObject {
    protected:
    ModelObject(std::shared_ptr<Model> model);
    public:
    SkeletonPose* skeletonPose;
    std::shared_ptr<Model> model;
    ~ModelObject();
    void draw();
    virtual void drawMesh(Mesh* mesh);
    virtual void prepareShaderProgram(Mesh* mesh);
};

class ObjectManager {
    public:
    std::vector<TransparentMesh*> transparentAccumulator;
    std::vector<ObjectBase*> objects;
    ~ObjectManager();
    void clear();
    void gameTick();
    void physicsTick();
    void draw();
};

extern ObjectManager objectManager;

#endif