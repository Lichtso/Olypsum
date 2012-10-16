//
//  BasicObjects.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "WorldManager.h"

ObjectBase::ObjectBase() {
    objectManager.objects.push_back(this);
}

ObjectBase::~ObjectBase() {
    for(unsigned int i = 0; i < objectManager.objects.size(); i ++) {
        if(objectManager.objects[i] != this) continue;
        objectManager.objects.erase(objectManager.objects.begin()+i);
        return;
    }
}

void ObjectBase::gameTick() {
    
}

btTransform ObjectBase::getTransformation() {
    log(error_log, "Unreachable function called: ObjectBase::getTransformation();");
    return btTransform::getIdentity();
}



PhysicObject::PhysicObject() {
    
}

void PhysicObject::physicsTick() {
    
}

void PhysicObject::handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) {
    
}



GraphicObject::GraphicObject() {
    
}

void GraphicObject::draw() {
    
}



ModelObject::ModelObject(std::shared_ptr<Model> modelB) :model(modelB) {
    if(model->skeleton) {
        skeletonPose = new SkeletonPose(model->skeleton);
        skeletonPose->calculate();
    }
}

ModelObject::~ModelObject() {
    if(skeletonPose)
        delete skeletonPose;
}

void ModelObject::draw() {
    if(prepareDraw())
        model->draw(this);
}

void ModelObject::prepareShaderProgram(Mesh* mesh) {
    if(!lightManager.currentShadowLight) {
        if(mesh->heightMap) {
            if(skeletonPose) {
                if(mesh->transparent && blendingQuality > 0)
                    shaderPrograms[glassSkeletalBumpGeometrySP]->use();
                else
                    shaderPrograms[skeletalBumpGeometrySP]->use();
            }else{
                if(mesh->transparent && blendingQuality > 0)
                    shaderPrograms[glassBumpGeometrySP]->use();
                else
                    shaderPrograms[solidBumpGeometrySP]->use();
            }
        }else{
            if(skeletonPose) {
                if(mesh->transparent && blendingQuality > 0)
                    shaderPrograms[glassSkeletalGeometrySP]->use();
                else
                    shaderPrograms[skeletalGeometrySP]->use();
            }else{
                if(mesh->transparent && blendingQuality > 0)
                    shaderPrograms[glassGeometrySP]->use();
                else
                    shaderPrograms[solidGeometrySP]->use();
            }
        }
    }
    currentShaderProgram->setUniformF("discardDensity", 1.0);
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose->mats, skeletonPose->skeleton->bones.size());
}

bool ModelObject::prepareDraw() {
    modelMat = getTransformation();
    return true;
}