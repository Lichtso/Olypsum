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
    ObjectManager();
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
    ModelObject(Model* model);
    ~ModelObject();
    float getDiscardDensity();
    Matrix4 getTransformation();
    void draw();
};

class NormalObject : public GraphicObject, RigidObject {
    public:
    NormalObject(Model* model);
    ~NormalObject();
    void gameTick();
    float getDiscardDensity();
    Matrix4 getTransformation();
    void draw();
};

#define MAX_WAVES 4

class WaterObject : public ModelObject, RigidObject {
    public:
    Matrix4 transformation;
    Model* model;
    float waveSpeed;
    struct WaterObjectWave {
        float length, age, maxAge, ampitude, originX, originY;
    };
    std::vector<WaterObjectWave> waves;
    WaterObject(Model* model);
    ~WaterObject();
    void addWave(float maxAge, float ampitude, float length, float originX, float originY);
    void calculate();
    float getDiscardDensity();
    Matrix4 getTransformation();
    void draw();
};

#endif