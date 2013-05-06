//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#include "AppMain.h"

BaseObject::~BaseObject() {
    if(!scriptInstance.IsEmpty())
        scriptInstance.Dispose();
}

void BaseObject::removeClean() {
    while(links.size() > 0)
        links.begin()->second->removeClean(this, links.begin());
    delete this;
}

void BaseObject::removeFast() {
    foreach_e(links, iterator)
        iterator->second->removeFast(this, iterator);
    delete this;
}

bool BaseObject::gameTick() {
    for(auto link : links)
        if(link.first != "..") //Don't process parent
            link.second->gameTickFrom(this);
    return true;
}

void BaseObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    setTransformation(readTransformtion(node, levelLoader));
}

void BaseObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptBaseObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
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

std::string BaseObject::getPath() {
    std::string path = "";
    BaseObject* object = this;
    while(true) {
        auto parentIterator = object->links.find(".."); //Find parent
        if(parentIterator == object->links.end() || !dynamic_cast<TransformLink*>(parentIterator->second)) break;
        object = parentIterator->second->getOther(object);
        for(auto iterator : object->links)
            if(iterator.second == parentIterator->second) {
                path = (path.size() == 0) ? iterator.first : iterator.first+'/'+path;
                break;
            }
    }
    return path;
}



rapidxml::xml_node<xmlUsedCharType>* BoneObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("BoneObject");
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("path");
    attribute->value(doc.allocate_string(getPath().c_str()));
    node->append_attribute(attribute);
    return node;
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
        body = NULL;
    }
    BaseObject::removeFast();
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

void PhysicObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptPhysicObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
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