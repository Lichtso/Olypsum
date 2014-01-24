//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

BaseClass::~BaseClass() {
    if(!scriptInstance.IsEmpty())
        scriptInstance.Dispose();
}

void BaseClass::initScriptNode(FilePackage* filePackage, rapidxml::xml_node<xmlUsedCharType>* node) {
    rapidxml::xml_node<xmlUsedCharType>* scriptNode = node->first_node("Script");
    if(!scriptNode) return;
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = scriptNode->first_attribute("src");
    if(attribute) {
        scriptFile = fileManager.getResourceByPath<ScriptFile>(filePackage, attribute->value());
        if(scriptFile)
            scriptFile->callFunction("onload", true, { scriptInstance, scriptManager->readCdataXMLNode(node) });
    }else
        log(error_log, "Tried to construct resource without \"src\"-attribute.");
}



void BaseObject::removeClean() {
    while(links.size() > 0) {
        BaseLink* link = *links.begin();
        if(dynamic_cast<TransformLink*>(link) && link->b != this)
            link->b->removeClean();
        else
            link->removeClean();
    }
    delete this;
}

void BaseObject::removeFast() {
    foreach_e(links, iterator)
        (*iterator)->removeFast();
    delete this;
}

bool BaseObject::gameTick() {
    foreach_e(links, iterator)
        if((*iterator)->a == this)
            (*iterator)->gameTick();
    return true;
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
    if(scriptFile) {
        v8::Handle<v8::Value> scritData = scriptFile->callFunction("onsave", true, { scriptInstance });
        scriptManager->writeCdataXMLNode(doc, node, "Data", scritData);
        node->append_node(fileManager.writeResource(doc, "Script", scriptFile->filePackage, scriptFile->name));
    }
    btTransform transform = getTransformation();
    node->append_node(writeTransformationXML(doc, transform));
    levelSaver->pushObject(this);
    return node;
}

std::unordered_set<BaseLink*>::iterator BaseObject::findLinkTo(BaseObject* linked) {
    foreach_e(links, i)
        if((*i)->getOther(this) == linked)
            return i;
    return links.end();
}



SimpleObject::SimpleObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :SimpleObject() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptBaseObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    objectManager.simpleObjects.insert(this);
    BaseObject::init(node, levelLoader);
}

void SimpleObject::removeClean() {
    objectManager.simpleObjects.erase(this);
    BaseObject::removeClean();
}



PhysicObject::PhysicObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    btCollisionShape* collisionShape = readCollisionShape(node->first_node("PhysicsBody"));
    if(!collisionShape) return;
    body = new btCollisionObject();
    body->setCollisionShape(collisionShape);
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Zone, CollisionMask_Object);
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptPhysicObject.functionTemplate->GetFunction()->NewInstance(1, &external);
}

void PhysicObject::removeClean() {
    if(body) {
        if(body->getCollisionShape())
            delete body->getCollisionShape();
        objectManager.physicsWorld->removeCollisionObject(body);
        delete body;
        body = NULL;
    }
    BaseObject::removeClean();
}

void PhysicObject::removeFast() {
    if(body) {
        if(body->getCollisionShape())
            delete body->getCollisionShape();
        delete body;
    }
    BaseObject::removeFast();
}

void PhysicObject::updateTouchingObjects() {
    body->activate();
    
    if(body->getCollisionFlags() & btCollisionObject::CF_NO_CONTACT_RESPONSE) return;
    
    unsigned int numManifolds = objectManager.collisionDispatcher->getNumManifolds();
	for(unsigned int i = 0; i < numManifolds; i ++) {
		btPersistentManifold* contactManifold = objectManager.collisionDispatcher->getManifoldByIndexInternal(i);
        if(contactManifold->getNumContacts() == 0) continue;
        
		const btCollisionObject *objectA = contactManifold->getBody0(),
        *objectB = contactManifold->getBody1();
        if(objectA == body)
            objectB->activate();
        else if(objectB == body)
            objectA->activate();
	}
}

void PhysicObject::handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) {
    if(!scriptFile || !scriptFile->checkFunction("oncollision")) return;
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Array> pointsA = v8::Array::New(contactManifold->getNumContacts()),
                          pointsB = v8::Array::New(contactManifold->getNumContacts()),
                          distances = v8::Array::New(contactManifold->getNumContacts()),
                          impulses = v8::Array::New(contactManifold->getNumContacts());
    
    for(unsigned int i = 0; i < contactManifold->getNumContacts(); i ++) {
        btManifoldPoint point = contactManifold->getContactPoint(i);
        pointsA->Set(i, scriptVector3.newInstance(point.getPositionWorldOnA()));
        pointsB->Set(i, scriptVector3.newInstance(point.getPositionWorldOnB()));
        distances->Set(i, v8::Number::New(point.getDistance()));
        impulses->Set(i, v8::Number::New(point.getAppliedImpulse()));
    }
    
    scriptFile->callFunction("oncollision", true, { scriptInstance, b->scriptInstance, pointsA, pointsB, distances, impulses });
}

btCollisionShape* PhysicObject::readCollisionShape(rapidxml::xml_node<xmlUsedCharType>* node) {
    if(!node) {
        log(error_log, "Tried to construct PhysicObject without \"PhysicsBody\"-node.");
        return NULL;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("collisionShape");
    if(!attribute) {
        log(error_log, "Tried to construct PhysicObject without \"collisionShape\"-attribute.");
        return NULL;
    }
    btCollisionShape* shape = levelManager.getCollisionShape(attribute->value());
    if(!shape) {
        log(error_log, std::string("Tried to construct PhysicObject with invalid \"collisionShape\"-attribute: ")+attribute->value()+'.');
        return NULL;
    }
    return shape;
}

void PhysicObject::readFrictionAndRestitution(rapidxml::xml_node<xmlUsedCharType>* node) {
    float value;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Friction");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("linear");
        if(!attribute) {
            log(error_log, "Tried to construct PhysicObject-Friction without \"linear\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &value);
        body->setFriction(value);
        attribute = parameterNode->first_attribute("angular");
        if(!attribute) {
            log(error_log, "Tried to construct PhysicObject-Friction without \"angular\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &value);
        body->setRollingFriction(value);
    }
    parameterNode = node->first_node("Restitution");
    if(parameterNode) {
        sscanf(parameterNode->value(), "%f", &value);
        body->setRestitution(value);
    }
}

rapidxml::xml_node<xmlUsedCharType>* PhysicObject::writeWithoutCollisionShape(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType> *parameterNode, *node = BaseObject::write(doc, levelSaver);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    if(body->getFriction() != 0.5 || body->getRollingFriction() != 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("Friction");
        node->append_node(parameterNode);
        attribute = doc.allocate_attribute();
        attribute->name("linear");
        attribute->value(doc.allocate_string(stringOf(body->getFriction()).c_str()));
        parameterNode->append_attribute(attribute);
        attribute = doc.allocate_attribute();
        attribute->name("angular");
        attribute->value(doc.allocate_string(stringOf(body->getRollingFriction()).c_str()));
        parameterNode->append_attribute(attribute);
    }
    if(body->getRestitution() != 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("Restitution");
        parameterNode->value(doc.allocate_string(stringOf(body->getRestitution()).c_str()));
        node->append_node(parameterNode);
    }
    return node;
}

rapidxml::xml_node<xmlUsedCharType>* PhysicObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::writeWithoutCollisionShape(doc, levelSaver);
    node->name("PhysicObject");
    rapidxml::xml_node<xmlUsedCharType>* physicsBody = doc.allocate_node(rapidxml::node_element);
    physicsBody->name("PhysicsBody");
    node->append_node(physicsBody);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("collisionShape");
    attribute->value(levelManager.getCollisionShapeName(body->getCollisionShape()));
    physicsBody->append_attribute(attribute);
    return node;
}