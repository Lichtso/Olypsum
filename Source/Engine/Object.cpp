//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ShaderProgram.h"

void Object::draw() {
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

void ObjectManager::draw() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        objects[i]->draw();
}

ModelOnlyObject::ModelOnlyObject(Model* modelB) {
    type = ObjectType_ModelOnly;
    model = modelB;
    transformation.setIdentity();
}

ModelOnlyObject::~ModelOnlyObject() {
    fileManager.releaseModel(model);
}

void ModelOnlyObject::draw() {
    modelMat = transformation;
    model->draw(1.0);
}



ObjectManager objectManager;