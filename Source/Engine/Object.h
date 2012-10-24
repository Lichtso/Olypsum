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

class ZoneObject : public PhysicObject {
    public:
    btCollisionObject* body;
    ZoneObject(btCollisionShape* shape, const btTransform& transform);
    ~ZoneObject();
    btTransform getTransformation();
};

class SoftObject : public GraphicObject, PhysicObject { //Not implemented (TODO)
    public:
    btSoftBody* body;
    SoftObject();
    ~SoftObject();
    btTransform getTransformation();
    void draw();
};

class RigidObject : public ModelObject, PhysicObject {
    public:
    btRigidBody* body;
    RigidObject(std::shared_ptr<Model> model, btRigidBody::btRigidBodyConstructionInfo& rBCI);
    ~RigidObject();
    btTransform getTransformation() override;
    void draw() override;
};

#define MAX_WAVES 4

class WaterObject : public ModelObject, ZoneObject {
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
    void draw();
};

#endif