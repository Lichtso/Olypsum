//
//  BasicObjects.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "LevelManager.h"

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

void BaseObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    setTransformation(readTransformtion(node, levelLoader));
}

btTransform BaseObject::readTransformtion(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    /*if(!node) {
        log(error_log, "Tried to construct BaseObject without \"Transformation\"-nodes.");
        return btTransform::getIdentity();
    }*/
    return levelLoader->transformation * readTransformationXML(node);
}



PhysicObject::~PhysicObject() {
    if(body) {
        if(body->getCollisionShape())
            delete body->getCollisionShape();
        delete body;
    }
}

void PhysicObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    btCollisionShape* collisionShape = readCollisionShape(node->first_node("PhysicsBody"), levelLoader);
    if(!collisionShape) return;
    body = new btCollisionObject();
    body->setCollisionShape(collisionShape);
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Zone, CollisionMask_Object);
}

btCollisionShape* PhysicObject::readCollisionShape(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    if(!node) {
        log(error_log, "Tried to construct PhysicObject without \"PhysicsBody\"-node.");
        return NULL;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("collisionShape");
    if(!attribute) {
        log(error_log, "Tried to construct PhysicObject without \"collisionShape\"-attribute.");
        return NULL;
    }
    btCollisionShape* shape = levelLoader->getCollisionShape(attribute->value());
    if(!shape) {
        log(error_log, std::string("Tried to construct PhysicObject with invalid \"collisionShape\"-attribute: ")+attribute->value()+'.');
        return NULL;
    }
    return shape;
}



BaseLink::BaseLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB) {
    fusion = reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(b));
    a->links[nameInA] = this;
    b->links[nameInB] = this;
}

BaseLink::BaseLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    BaseObject *a, *b;
    init(node, levelLoader, a, b);
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

void BaseLink::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader, BaseObject*& a, BaseObject*& b) {
    node = node->first_node("Object");
    if(!node) {
        log(error_log, "Tried to construct BaseLink without first \"Object\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("id");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without first \"id\"-attribute.");
        return;
    }
    a = levelLoader->getObjectLinking(attribute->value());
    attribute = node->first_attribute("name");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without first \"name\"-attribute.");
        return;
    }
    const char *nameOfA = attribute->value();
    
    node = node->next_sibling("Object");
    if(!node) {
        log(error_log, "Tried to construct BaseLink without first \"Object\"-node.");
        return;
    }
    attribute = node->first_attribute("id");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without first \"id\"-attribute.");
        return;
    }
    b = levelLoader->getObjectLinking(attribute->value());
    attribute = node->first_attribute("name");
    if(!attribute) {
        log(error_log, "Tried to construct BaseLink without first \"name\"-attribute.");
        return;
    }
    
    decltype(a->links)::iterator iterator;
    if(strcmp(attribute->value(), "parent") != 0) {
        iterator = a->links.find(attribute->value());
        if(iterator != a->links.end())
            iterator->second->remove(b, iterator);
    }else if(strcmp(nameOfA, "parent") != 0) {
        iterator = b->links.find(nameOfA);
        if(iterator != b->links.end())
            iterator->second->remove(a, iterator);
    }else{
        log(error_log, "Tried to construct BaseLink with two parent relationships.");
        return;
    }
    a->links[attribute->value()] = this;
    b->links[nameOfA] = this;
    fusion = reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(b));
}



PhysicLink::PhysicLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB, btTypedConstraint* constraintB)
    :BaseLink(a, b, nameInA, nameInB), constraint(constraintB) {
    constraint->setUserConstraintPtr(this);
    objectManager.physicsWorld->addConstraint(constraint);
}

PhysicLink::PhysicLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    rapidxml::xml_node<xmlUsedCharType>* constraintNode = node->first_node("Constraint");
    if(!constraintNode) {
        log(error_log, "Tried to construct PhysicLink without \"Constraint\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = constraintNode->first_attribute("type");
    if(!attribute) {
        log(error_log, "Tried to construct PhysicLink without \"type\"-attribute.");
        return;
    }
    BaseObject *c, *d;
    BaseLink::init(node, levelLoader, c, d);
    RigidObject *a = dynamic_cast<RigidObject*>(c), *b = dynamic_cast<RigidObject*>(d);
    if(!a || !b) {
        log(error_log, "Tried to construct PhysicLink with objects which aren't RigidObjects.");
        return;
    }
    
    if(strcmp(attribute->value(), "point") == 0) {
        rapidxml::xml_node<xmlUsedCharType>* pointNode = constraintNode->first_node("Point");
        if(!pointNode) {
            log(error_log, "Tried to construct Point-PhysicLink without first \"Point\"-node.");
            return;
        }
        XMLValueArray<float> vecData;
        vecData.readString(pointNode->value(), "%f");
        btVector3 pointA = vecData.getVector3();
        
        pointNode = pointNode->next_sibling("Point");
        if(!pointNode) {
            log(error_log, "Tried to construct Point-PhysicLink without second \"Point\"-node.");
            return;
        }
        vecData.readString(pointNode->value(), "%f");
        btVector3 pointB = vecData.getVector3();
        
        constraint = new btPoint2PointConstraint(*a->getBody(), *b->getBody(), pointA, pointB);
    }else if(strcmp(attribute->value(), "gear") == 0) {
        rapidxml::xml_node<xmlUsedCharType>* pointNode = constraintNode->first_node("Axis");
        if(!pointNode) {
            log(error_log, "Tried to construct Gear-PhysicLink without first \"Axis\"-node.");
            return;
        }
        XMLValueArray<float> vecData;
        vecData.readString(pointNode->value(), "%f");
        btVector3 axisA = vecData.getVector3();
        
        pointNode = pointNode->next_sibling("Axis");
        if(!pointNode) {
            log(error_log, "Tried to construct Gear-PhysicLink without second \"Axis\"-node.");
            return;
        }
        vecData.readString(pointNode->value(), "%f");
        btVector3 axisB = vecData.getVector3();
        
        attribute = constraintNode->first_attribute("ratio");
        if(!attribute) {
            log(error_log, "Tried to construct Gear-PhysicLink without \"ratio\"-attribute.");
            return;
        }
        float ratio;
        sscanf(attribute->value(), "%f", &ratio);
        
        constraint = new btGearConstraint(*a->getBody(), *b->getBody(), axisA, axisB, ratio);
    }else if(strcmp(attribute->value(), "hinge") == 0) {
        rapidxml::xml_node<xmlUsedCharType>* axisNode = constraintNode->first_node("Axis");
        if(!axisNode) {
            log(error_log, "Tried to construct Hinge-PhysicLink without first \"Axis\"-node.");
            return;
        }
        XMLValueArray<float> vecData;
        vecData.readString(axisNode->value(), "%f");
        btVector3 axisA = vecData.getVector3();
        attribute = axisNode->first_attribute("origin");
        if(!attribute) {
            log(error_log, "Tried to construct Hinge-PhysicLink without first \"origin\"-attribute.");
            return;
        }
        vecData.readString(attribute->value(), "%f");
        btVector3 originA = vecData.getVector3();
        
        axisNode = axisNode->next_sibling("Axis");
        if(!axisNode) {
            log(error_log, "Tried to construct Hinge-PhysicLink without second \"Axis\"-node.");
            return;
        }
        vecData.readString(axisNode->value(), "%f");
        btVector3 axisB = vecData.getVector3();
        attribute = axisNode->first_attribute("origin");
        if(!attribute) {
            log(error_log, "Tried to construct Hinge-PhysicLink without second \"origin\"-attribute.");
            return;
        }
        vecData.readString(attribute->value(), "%f");
        btVector3 originB = vecData.getVector3();
        
        constraint = new btHingeConstraint(*a->getBody(), *b->getBody(), originA, originB, axisA, axisB);
    }else{
        log(error_log, std::string("Tried to construct PhysicLink with invalid \"type\"-attribute: ")+attribute->value()+'.');
        return;
    }
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

TransformLink::TransformLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    BaseObject *a, *b;
    BaseLink::init(node, levelLoader, a, b);
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
    
    if(boneObject) transform = boneObject->bone->relativeMat*transform;
    child->setTransformation(parent->getTransformation()*transform);
    
    if(boneObject) boneObject->gameTick();
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