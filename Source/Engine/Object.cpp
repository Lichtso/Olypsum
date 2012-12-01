//
//  BasicObjects.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "WorldManager.h"

BaseObject::~BaseObject() {
    for(auto iterator : links)
        iterator.second->remove(this, iterator.first);
}



PhysicObject::PhysicObject(btCollisionShape* shape) :body(new btCollisionObject()) {
    body->setCollisionShape(shape);
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(body, CollisionMask_Zone, CollisionMask_Object);
}

PhysicObject::~PhysicObject() {
    if(body) {
        if(body->getCollisionShape())
            delete body->getCollisionShape();
        delete body;
    }
}


PhysicLink::PhysicLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB, btTypedConstraint* constraintB)
    :BaseLink(a, b, nameInA, nameInB), constraint(constraintB) {
    constraint->setUserConstraintPtr(this);
    worldManager.physicsWorld->addConstraint(constraint);
}

PhysicLink::~PhysicLink() {
    worldManager.physicsWorld->removeConstraint(constraint);
}