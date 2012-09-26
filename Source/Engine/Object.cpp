//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ShaderProgram.h"

void ObjectBase::calculate() {
    
}

float ObjectBase::getDiscardDensity() {
    return 0.0;
}

Matrix4 ObjectBase::getTransformation() {
    return Matrix4();
}

void ObjectBase::draw() {
    log(error_log, "draw() called in unspecified Object.");
}

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

void ObjectManager::calculate() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        objects[i]->calculate();
}

void ObjectManager::draw() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        objects[i]->draw();
}

ObjectManager objectManager;

ModelOnlyObject::ModelOnlyObject(Model* modelB) {
    type = ObjectInstance_ModelOnly;
    model = modelB;
    transformation.setIdentity();
}

ModelOnlyObject::~ModelOnlyObject() {
    fileManager.releaseModel(model);
}

void ModelOnlyObject::calculate() {
    
}

float ModelOnlyObject::getDiscardDensity() {
    return 1.0;
}

Matrix4 ModelOnlyObject::getTransformation() {
    return transformation;
}

void ModelOnlyObject::draw() {
    modelMat = transformation;
    model->draw(this);
}



WaterObject::WaterObject(Model* modelB) {
    type = ObjectInstance_Water;
    waveSpeed = 0.05;
    model = modelB;
    transformation.setIdentity();
}

WaterObject::~WaterObject() {
    fileManager.releaseModel(model);
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
        waves[i].age += animationFactor;
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