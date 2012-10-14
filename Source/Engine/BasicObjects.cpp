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
    
}

void ObjectBase::gameTick() {
    
}

Matrix4 ObjectBase::getTransformation() {
    return Matrix4().setIdentity();
}

RigidObject::RigidObject(btRigidBody::btRigidBodyConstructionInfo* rBCI) {
    rBCI->m_collisionShape->calculateLocalInertia(rBCI->m_mass, rBCI->m_localInertia);
    body = new btRigidBody(*rBCI);
}

RigidObject::~RigidObject() {
    if(body) {
        while(body->getNumConstraintRefs() > 0) {
            btTypedConstraint* constraint = body->getConstraintRef(0);
            worldManager.physicsWorld->removeConstraint(constraint);
            delete constraint;
        }
        delete body->getMotionState();
        worldManager.physicsWorld->removeRigidBody(body);
        delete body;
    }
}

Matrix4 RigidObject::getTransformation() {
    btTransform transform;
    body->getMotionState()->getWorldTransform(transform);
    return Matrix4(transform);
}


GraphicObject::GraphicObject(Model* modelB) :model(modelB) {
    
}

GraphicObject::~GraphicObject() {
    if(skeletonPose)
        delete skeletonPose;
}