//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

ObjectManager::ObjectManager() {
    
}

ObjectManager::~ObjectManager() {
    clear();
}

void ObjectManager::clear() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        delete objects[i];
    objects.clear();
}

void ObjectManager::gameTick() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        objects[i]->gameTick();
}

void ObjectManager::physicsTick() {
    for(unsigned int i = 0; i < objects.size(); i ++) {
        PhysicObject* object = dynamic_cast<PhysicObject*>(objects[i]);
        if(!object) continue;
        object->physicsTick();
    }
}

void ObjectManager::draw() {
    for(unsigned int i = 0; i < objects.size(); i ++) {
        GraphicObject* object = dynamic_cast<GraphicObject*>(objects[i]);
        if(!object) continue;
        object->draw();
    }
}

ObjectManager objectManager;



WaterObject::WaterObject(Model* modelB) {
    waveSpeed = 0.05;
    model = modelB;
    transformation.setIdentity();
}

WaterObject::~WaterObject() {
    
}

void WaterObject::addWave(float maxAge, float ampitude, float length, float originX, float originY) {
    if(waves.size() >= MAX_WAVES) return;
    WaterObjectWave wave;
    wave.age = 0.0;
    wave.maxAge = maxAge;
    wave.ampitude = ampitude;
    wave.length = length;
    wave.originX = originX;
    wave.originY = originY;
    waves.push_back(wave);
}

void WaterObject::calculate() {
    for(unsigned int i = 0; i < waves.size(); i ++) {
        waves[i].age += worldManager.animationFactor;
        if(waves[i].age >= waves[i].maxAge) {
            waves.erase(waves.begin()+i);
            i --;
        }
    }
}

float WaterObject::getDiscardDensity() {
    return 1.0;
}

Matrix4 WaterObject::getTransformation() {
    return transformation;
}

void WaterObject::draw() {
    if(lightManager.currentShadowLight) return;
    modelMat = transformation;
    model->draw(this);
}