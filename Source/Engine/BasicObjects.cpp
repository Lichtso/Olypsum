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

Matrix4 ObjectBase::getTransformation() {
    log(error_log, "Unreachable function called: ObjectBase::getTransformation();");
    return Matrix4().setIdentity();
}



/*void PhysicObject::physicsTick() {
    
}*/

void PhysicObject::handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) {
    
}



ZoneObject::ZoneObject(btCollisionShape* shape, const btTransform& transform) {
    body = new btCollisionObject();
    body->setCollisionShape(shape);
    body->setWorldTransform(transform);
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(body);
}

ZoneObject::~ZoneObject() {
    worldManager.physicsWorld->removeCollisionObject(body);
    delete body;
}

Matrix4 ZoneObject::getTransformation() {
    return Matrix4(body->getWorldTransform());
}



GraphicObject::GraphicObject(std::shared_ptr<Model> modelB) :model(modelB) {
    if(model->skeleton) {
        skeletonPose = new SkeletonPose(model->skeleton);
        skeletonPose->calculate();
    }
}

GraphicObject::~GraphicObject() {
    if(skeletonPose)
        delete skeletonPose;
}

void GraphicObject::prepareShaderProgram(Mesh* mesh) {
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

bool GraphicObject::prepareDraw() {
    modelMat = getTransformation();
    return true;
}