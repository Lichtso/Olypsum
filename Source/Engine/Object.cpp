//
//  BasicObjects.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "WorldManager.h"

bool BaseObject::gameTick() {
    for(auto link : links)
        if(link.first != "parent")
            link.second->gameTickFrom(this);
    return true;
}

void BaseObject::remove() {
    while(links.size() > 0)
        links.begin()->second->remove(this, links.begin());
    delete this;
}



PhysicObject::PhysicObject(btCollisionShape* shape) :body(new btCollisionObject()) {
    body->setCollisionShape(shape);
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Zone, CollisionMask_Object);
}

PhysicObject::~PhysicObject() {
    if(body) {
        if(body->getCollisionShape())
            delete body->getCollisionShape();
        delete body;
    }
}



BaseLink::BaseLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB) {
    fusion = reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(b));
    a->links[nameInA] = this;
    b->links[nameInB] = this;
}

void BaseLink::remove(BaseObject* a, const std::map<std::string, BaseLink*>::iterator& iteratorInA) {
    a->links.erase(iteratorInA);
    BaseObject* b = getOther(a);
    for(auto iterator : b->links)
        if(iterator.second == this) {
            b->links.erase(iterator.first);
            break;
        }
    delete this;
}



PhysicLink::PhysicLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB, btTypedConstraint* constraintB)
    :BaseLink(a, b, nameInA, nameInB), constraint(constraintB) {
    constraint->setUserConstraintPtr(this);
    objectManager.physicsWorld->addConstraint(constraint);
}

PhysicLink::~PhysicLink() {
    objectManager.physicsWorld->removeConstraint(constraint);
}



TransformLink::AnimationEntry::Frame::Frame(float accBeginB, float accEndB, float durationB, btQuaternion rotationB, btVector3 positionB)
    : accBegin(accBeginB), accEnd(accEndB), duration(durationB), rotation(rotationB), position(positionB) {}

btTransform TransformLink::AnimationEntry::Frame::getTransform() {
    btTransform transform;
    transform.setRotation(rotation);
    transform.setOrigin(position);
    return transform;
}

btTransform TransformLink::AnimationEntry::Frame::interpolateTo(Frame* next, float t) {
    t /= duration;
    float t2 = t*t, t3 = t2*t;
    t = 3.0*t2-2.0*t3+(t3-2.0*t2+t)*accBegin+(t3-t2)*accEnd;
    
    btTransform transform;
    transform.setRotation(rotation.slerp(next->rotation, t));
    transform.setOrigin(position+(next->position-position)*t);
    return transform;
}

TransformLink::AnimationEntry::~AnimationEntry() {
    for(auto frame : frames)
        delete frame;
}

btTransform TransformLink::AnimationEntry::gameTick() {
    if(frames.size() == 0) return btTransform::getIdentity();
    if(frames.size() == 1) return frames[0]->getTransform();
    btTransform transform = frames[0]->interpolateTo(frames[1], animationTime);
    animationTime += animationFactor;
    if(animationTime > frames[0]->duration) {
        animationTime -= frames[0]->duration;
        frames.erase(frames.begin());
    }
    return transform;
}

TransformLink::TransformLink(BaseObject* parent, BaseObject* child, std::string childName)
    :BaseLink(parent, child, childName, "parent") {
    
}

TransformLink::~TransformLink() {
    for(auto transform : transforms)
        delete transform;
}

void TransformLink::gameTickFrom(BaseObject* parent) {
    BaseObject* child = getOther(parent);
    BoneObject* boneObject = dynamic_cast<BoneObject*>(child);
    btTransform transform = btTransform::getIdentity();
    
    if(transforms.size() > 0) {
        transform = transforms[0]->gameTick();
        for(unsigned int i = 1; i < transforms.size(); i ++)
            transform = transforms[i]->gameTick()*transform;
    }
    
    if(boneObject) {
        boneObject->gameTick();
        transform = boneObject->bone->relativeMat*transform;
    }
    
    child->setTransformation(parent->getTransformation()*transform);
}

void TransformLink::remove(BaseObject* a, const std::map<std::string, BaseLink*>::iterator& iteratorInA) {
    a->links.erase(iteratorInA);
    BaseObject* b = getOther(a);
    for(auto iterator : b->links)
        if(iterator.second == this) {
            b->links.erase(iterator.first);
            break;
        }
    if(iteratorInA->first != "parent")
        b->remove();
    delete this;
}