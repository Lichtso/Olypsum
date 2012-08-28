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

enum ObjectType {
    ObjectType_ModelOnly = 1,
    ObjectType_Normal = 2,
    ObjectType_Animated = 3,
    ObjectType_Water = 4,
    ObjectType_NPC = 5
};

struct TransparentMesh {
    ObjectType type;
    SkeletonPose* skeletonPose;
    Matrix4 transformation;
    Mesh* mesh;
    float discardDensity;
};

class Object {
    public:
    ObjectType type;
    Model* model;
    virtual void draw();
};

class ObjectManager {
    public:
    std::vector<TransparentMesh*> transparentAccumulator;
    std::vector<Object*> objects;
    ObjectManager();
    ~ObjectManager();
    void clear();
    void draw();
};

class ModelOnlyObject : public Object {
    public:
    Matrix4 transformation;
    ModelOnlyObject(Model* model);
    ~ModelOnlyObject();
    void draw();
};

extern ObjectManager objectManager;

#endif