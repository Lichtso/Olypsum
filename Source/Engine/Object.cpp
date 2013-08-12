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

void BaseClass::initScriptNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    rapidxml::xml_node<xmlUsedCharType>* scriptNode = node->first_node("Script");
    if(!scriptNode) return;
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = scriptNode->first_attribute("src");
    if(attribute) {
        FilePackage* filePackage;
        std::string name;
        if(fileManager.readResourcePath(attribute->value(), filePackage, name)) {
            scriptFile = scriptManager->getScriptFile(filePackage, name);
            if(scriptFile)
                scriptFile->callFunction("onload", true, { scriptInstance, scriptManager->readCdataXMLNode(node) });
        }
    }else
        log(error_log, "Tried to construct resource without \"src\"-attribute.");
}



void BaseObject::removeClean() {
    while(links.size() > 0)
        (*links.begin())->removeClean(this);
    delete this;
}

void BaseObject::removeFast() {
    foreach_e(links, iterator)
        (*iterator)->removeFast(this);
    delete this;
}

bool BaseObject::gameTick() {
    foreach_e(links, iterator)
        if((*iterator)->b != this || !dynamic_cast<TransformLink*>(*iterator)) //Don't process parent
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
        if(!scritData.IsEmpty() && scritData->IsString())
            scriptManager->writeCdataXMLNode(doc, stdStrOfV8(scritData));
        node->append_node(fileManager.writeResource(doc, "Script", scriptFile->filePackage, scriptFile->name));
    }
    btTransform transform = getTransformation();
    node->append_node(writeTransformationXML(doc, transform));
    levelSaver->pushObject(this);
    return node;
}

std::set<BaseLink*>::iterator BaseObject::findParentLink() {
    for(std::set<BaseLink*>::iterator i = links.begin(); i != links.end(); i ++)
        if(dynamic_cast<TransformLink*>(*i) && (*i)->b == this)
            return i;
    return links.end();
}

std::set<BaseLink*>::iterator BaseObject::findLink(BaseObject* linked) {
    for(std::set<BaseLink*>::iterator i = links.begin(); i != links.end(); i ++)
        if((*i)->getOther(this) == linked)
            return i;
    return links.end();
}



BoneObject::BoneObject(Bone* _bone, BaseObject* parentObject) : bone(_bone) {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptBoneObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    
    LinkInitializer initializer;
    initializer.object[0] = parentObject;
    initializer.object[1] = this;
    (new TransformLink())->init(initializer);
}



PhysicObject::PhysicObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    btCollisionShape* collisionShape = readCollisionShape(node->first_node("PhysicsBody"), levelLoader);
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
    
    body->activate();
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
    btCollisionShape* shape = levelManager.getCollisionShape(attribute->value());
    if(!shape) {
        log(error_log, std::string("Tried to construct PhysicObject with invalid \"collisionShape\"-attribute: ")+attribute->value()+'.');
        return NULL;
    }
    return shape;
}

rapidxml::xml_node<xmlUsedCharType>* PhysicObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("PhysicObject");
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