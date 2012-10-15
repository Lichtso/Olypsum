//
//  Object.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Model.h"

#ifndef Object_h
#define Object_h

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

class ModelObject : public GraphicObject {
    public:
    Matrix4 transformation;
    ModelObject(std::shared_ptr<Model> model);
    Matrix4 getTransformation();
};

class RigidObject : public GraphicObject, PhysicObject {
    public:
    btRigidBody* body;
    RigidObject(std::shared_ptr<Model> model, btRigidBody::btRigidBodyConstructionInfo* rBCI);
    ~RigidObject();
    Matrix4 getTransformation();
};

#define MAX_WAVES 4

class WaterObject : public GraphicObject, ZoneObject {
    public:
    float waveSpeed;
    struct WaterObjectWave {
        float length, age, maxAge, ampitude, originX, originY;
    };
    std::vector<WaterObjectWave> waves;
    WaterObject(std::shared_ptr<Model> model, btCollisionShape* shape, const btTransform& transform);
    ~WaterObject();
    void addWave(float maxAge, float ampitude, float length, float originX, float originY);
    void gameTick();
    void prepareShaderProgram(Mesh* mesh);
    bool prepareDraw();
};

#endif