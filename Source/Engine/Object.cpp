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
        if(link.first != "..") //Don't process parent
            link.second->gameTickFrom(this);
    return true;
}

void BaseObject::remove() {
    while(links.size() > 0)
        links.begin()->second->remove(this, links.begin());
    delete this;
}

void BaseObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    setTransformation(readTransformtion(node, levelLoader));
}

btTransform BaseObject::readTransformtion(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    return levelLoader->transformation * readTransformationXML(node);
}

rapidxml::xml_node<xmlUsedCharType>* BaseObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("BaseObject");
    btTransform transform = getTransformation();
    node->append_node(writeTransformationXML(doc, transform));
    levelSaver->pushObject(this);
    return node;
}

BaseObject* BaseObject::findObjectByPath(std::string path) {
    std::vector<char*> objectNames;
    char buffer[path.size()+1], *str = buffer;
    strcpy(buffer, path.c_str());
    
    objectNames.push_back(buffer);
    while(*str != 0) {
        if(*str != '/') {
            str ++;
            continue;
        }
        *str = 0;
        str ++;
        objectNames.push_back(str);
    }
    
    BaseObject* object = this;
    for(char* objectName : objectNames) {
        auto iterator = object->links.find(objectName);
        if(iterator == object->links.end()) {
            log(error_log, std::string("Couldn't find object (")+objectName+") in path: "+path+'.');
            return NULL;
        }
        object = iterator->second->getOther(object);
    }
    
    return object;
}



rapidxml::xml_node<xmlUsedCharType>* BoneObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("BoneObject");
    
    std::string path = "";
    BaseObject* object = this;
    auto parentIterator = object->links.find(".."); //Find parent
    while(parentIterator != object->links.end()) {
        object = parentIterator->second->getOther(object);
        for(auto iterator : object->links)
            if(iterator.second == parentIterator->second) {
                path = (path.size() == 0) ? iterator.first : iterator.first+'/'+path;
                break;
            }
        parentIterator = object->links.find(".."); //Find parent
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("path");
    attribute->value(doc.allocate_string(path.c_str()));
    node->append_attribute(attribute);
    return node;
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

rapidxml::xml_node<xmlUsedCharType>* PhysicObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    rapidxml::xml_node<xmlUsedCharType>* physicsBody = doc.allocate_node(rapidxml::node_element);
    physicsBody->name("PhysicsBody");
    node->append_node(physicsBody);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("collisionShape");
    physicsBody->append_attribute(attribute);
    for(auto iterator : levelManager.sharedCollisionShapes)
        if(iterator.second == body->getCollisionShape()) {
            attribute->value(doc.allocate_string(iterator.first.c_str()));
            return node;
        }
    log(error_log, "Couldn't find collision shape of PhysicObject.");
    return node;
}



BaseLink::BaseLink(LinkInitializer& initializer) {
    init(initializer);
}

BaseLink::BaseLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer = readInitializer(node, levelLoader);
    init(initializer);
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

LinkInitializer BaseLink::readInitializer(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer;
    node = node->first_node("Object");
    for(char i = 0; i < 2; i ++) {
        if(!node) {
            log(error_log, "Tried to construct BaseLink without \"Object\"-node.");
            return initializer;
        }
        rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("index");
        if(!attribute) {
            log(error_log, "Tried to construct BaseLink without \"index\"-attribute.");
            return initializer;
        }
        initializer.object[i] = levelLoader->getObjectLinking(attribute->value());
        attribute = node->first_attribute("name");
        if(!attribute) {
            log(error_log, "Tried to construct BaseLink without \"name\"-attribute.");
            return initializer;
        }
        initializer.name[i] = attribute->value();
        node = node->next_sibling("Object");
    }
    return initializer;
}

void BaseLink::init(LinkInitializer& initializer) {
    initializer.object[0]->links[initializer.name[1]] = this;
    initializer.object[1]->links[initializer.name[0]] = this;
    fusion = reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(initializer.object[0]) ^ reinterpret_cast<unsigned long>(initializer.object[1]));
}

rapidxml::xml_node<xmlUsedCharType>* BaseLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("BaseLink");
    for(char i = 0; i < 2; i ++) {
        rapidxml::xml_node<xmlUsedCharType>* objectNode = doc.allocate_node(rapidxml::node_element);
        objectNode->name("Object");
        node->append_node(objectNode);
        rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
        attribute->name("index");
        attribute->value(doc.allocate_string(stringOf(linkSaver->index[i]).c_str()));
        objectNode->append_attribute(attribute);
        attribute = doc.allocate_attribute();
        attribute->name("name");
        attribute->value(doc.allocate_string(linkSaver->name[i].c_str()));
        objectNode->append_attribute(attribute);
    }
    return node;
}



PhysicLink::PhysicLink(LinkInitializer& initializer, btTypedConstraint* constraintB)
    :BaseLink(initializer), constraint(constraintB) {
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
    
    LinkInitializer initializer = BaseLink::readInitializer(node, levelLoader);
    RigidObject *a = dynamic_cast<RigidObject*>(initializer.object[0]), *b = dynamic_cast<RigidObject*>(initializer.object[1]);
    if(!a || !b) {
        log(error_log, "Tried to construct PhysicLink with objects which aren't RigidObjects.");
        return;
    }
    BaseLink::init(initializer);
    
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
        rapidxml::xml_node<xmlUsedCharType>* frameNode = constraintNode->first_node("Frame");
        if(!frameNode) {
            log(error_log, "Tried to construct Hinge-PhysicLink without first \"Frame\"-node.");
            return;
        }
        btTransform frameA = readTransformationXML(frameNode);
        frameNode = frameNode->next_sibling("Frame");
        if(!frameNode) {
            log(error_log, "Tried to construct Hinge-PhysicLink without second \"Axis\"-node.");
            return;
        }
        btTransform frameB = readTransformationXML(frameNode);
        constraint = new btHingeConstraint(*a->getBody(), *b->getBody(), frameA, frameB);
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

rapidxml::xml_node<xmlUsedCharType>* PhysicLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseLink::write(doc, linkSaver);
    node->name("PhysicLink");
    rapidxml::xml_node<xmlUsedCharType>* constraintNode = doc.allocate_node(rapidxml::node_element);
    constraintNode->name("Constraint");
    node->append_node(constraintNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("type");
    constraintNode->append_attribute(attribute);
    
    if(&constraint->getRigidBodyA() == static_cast<RigidObject*>(linkSaver->object[1])->getBody()) {
        linkSaver->swap();
        printf("linkSaver->swap()\n");
    }
    
    switch(constraint->getConstraintType()) {
        case POINT2POINT_CONSTRAINT_TYPE: {
            attribute->value("point");
            btPoint2PointConstraint* pointConstraint = static_cast<btPoint2PointConstraint*>(constraint);
            rapidxml::xml_node<xmlUsedCharType>* pointNode = doc.allocate_node(rapidxml::node_element);
            pointNode->name("Point");
            btVector3 point = pointConstraint->getPivotInA();
            pointNode->value(doc.allocate_string(stringOf(point).c_str()));
            node->append_node(pointNode);
            pointNode = doc.allocate_node(rapidxml::node_element);
            pointNode->name("Point");
            point = pointConstraint->getPivotInB();
            pointNode->value(doc.allocate_string(stringOf(point).c_str()));
            node->append_node(pointNode);
        } break;
        case GEAR_CONSTRAINT_TYPE: {
            attribute->value("gear");
            btGearConstraint* gearConstraint = static_cast<btGearConstraint*>(constraint);
            rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
            attribute->name("ratio");
            attribute->value(doc.allocate_string(stringOf(gearConstraint->getRatio()).c_str()));
            node->append_attribute(attribute);
            rapidxml::xml_node<xmlUsedCharType>* axisNode = doc.allocate_node(rapidxml::node_element);
            axisNode->name("Axis");
            btVector3 axis = gearConstraint->getAxisA();
            axisNode->value(doc.allocate_string(stringOf(axis).c_str()));
            node->append_node(axisNode);
            axisNode = doc.allocate_node(rapidxml::node_element);
            axisNode->name("Axis");
            axis = gearConstraint->getAxisB();
            axisNode->value(doc.allocate_string(stringOf(axis).c_str()));
            node->append_node(axisNode);
        } break;
        case HINGE_CONSTRAINT_TYPE: {
            attribute->value("hinge");
            btHingeConstraint* hingeConstraint = static_cast<btHingeConstraint*>(constraint);
            rapidxml::xml_node<xmlUsedCharType>* frameNode = doc.allocate_node(rapidxml::node_element);
            frameNode->name("Frame");
            btTransform transform = hingeConstraint->getFrameOffsetA();
            frameNode->append_node(writeTransformationXML(doc, transform));
            node->append_node(frameNode);
            frameNode = doc.allocate_node(rapidxml::node_element);
            frameNode->name("Frame");
            transform = hingeConstraint->getFrameOffsetB();
            frameNode->append_node(writeTransformationXML(doc, transform));
            node->append_node(frameNode);
        } break;
        case CONETWIST_CONSTRAINT_TYPE:
            attribute->value("coneTwist");
            
            break;
        case SLIDER_CONSTRAINT_TYPE:
            attribute->value("slider");
            break;
        case D6_CONSTRAINT_TYPE:
            attribute->value("dof6");
            break;
        case D6_SPRING_CONSTRAINT_TYPE:
            attribute->value("dof6Spring");
            break;
        default:
            log(error_log, "Tried to save PhysicLink with invalid constraint type.");
            break;
    }
    return node;
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

TransformLink::TransformLink(LinkInitializer& initializer) {
    init(initializer);
}

TransformLink::TransformLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    LinkInitializer initializer = BaseLink::readInitializer(node, levelLoader);
    init(initializer);
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
    if(iteratorInA->first != "..") //Only remove child if called by parent
        b->remove();
    delete this;
}

void TransformLink::init(LinkInitializer &initializer) {
    //Make sure that a is the parent and b the child node
    if(initializer.name[1] == "..") {
        if(initializer.name[0] == "..") {
            log(error_log, "Tried to construct TransformLink with two parent nodes.");
            return;
        }
        initializer.swap();
    }else if(initializer.name[0] != "..") {
        log(error_log, "Tried to construct TransformLink without parent nodes.");
        return;
    }
    
    //Replace link and child object if there is already one
    auto iterator = initializer.object[0]->links.find(initializer.name[1]);
    if(iterator != initializer.object[0]->links.end()) {
        iterator->second->remove(initializer.object[0], iterator);
    }else{
        iterator = initializer.object[1]->links.find("..");
        if(iterator != initializer.object[1]->links.end()) {
            log(warning_log, "Constructed TransformLink with a child node which was already bound.");
            iterator->second->remove(initializer.object[1], iterator);
        }
    }
    
    BaseLink::init(initializer);
}

rapidxml::xml_node<xmlUsedCharType>* TransformLink::write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseLink::write(doc, linkSaver);
    node->name("TransformLink");
    return node;
}