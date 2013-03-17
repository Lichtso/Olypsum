//
//  Object.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#include "AppMain.h"

bool BaseObject::gameTick() {
    for(auto link : links)
        if(link.first != "..") //Don't process parent
            link.second->gameTickFrom(this);
    return true;
}

void BaseObject::remove() {
    if(!scriptInstance.IsEmpty())
        scriptInstance.Dispose();
    delete this;
}

BaseObject::~BaseObject() {
    while(links.size() > 0)
        links.begin()->second->remove(this, links.begin());
}

void BaseObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    setTransformation(readTransformtion(node, levelLoader));
}

void BaseObject::initScriptInstance(rapidxml::xml_node<xmlUsedCharType>* node) {
    v8::HandleScope handleScope;
    scriptInstance = v8::Persistent<v8::Object>::New(scriptBaseObject.newInstance(this));
    node = node->first_node("Script");
    if(!node) return;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("package");
    FilePackage* levelPack = levelManager.levelPackage;
    if(attribute)
        levelPack = fileManager.getPackage(attribute->value());
    attribute = node->first_attribute("src");
    if(!attribute) {
        log(error_log, "Tried to construct resource without \"src\"-attribute.");
        return;
    }
    scriptFile = scriptManager->getScriptFile(levelPack, attribute->value());
    scriptManager->callFunctionOfScript(scriptFile, "onload", true, { scriptInstance });
}

btTransform BaseObject::readTransformtion(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    return levelLoader->transformation * readTransformationXML(node);
}

rapidxml::xml_node<xmlUsedCharType>* BaseObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("BaseObject");
    btTransform transform = getTransformation();
    node->append_node(writeTransformationXML(doc, transform));
    if(scriptFile) {
        rapidxml::xml_node<xmlUsedCharType>* scriptNode = doc.allocate_node(rapidxml::node_element);
        scriptNode->name("Script");
        node->append_node(scriptNode);
        rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
        if(scriptFile->filePackage != levelManager.levelPackage) {
            attribute->name("package");
            attribute->value(doc.allocate_string(scriptFile->filePackage->name.c_str()));
            scriptNode->append_attribute(attribute);
            attribute = doc.allocate_attribute();
        }
        attribute->name("src");
        attribute->value(doc.allocate_string(scriptFile->name.c_str()));
        scriptNode->append_attribute(attribute);
    }
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
        objectManager.physicsWorld->removeCollisionObject(body);
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

void PhysicObject::handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) {
    scriptManager->callFunctionOfScript(scriptFile, "oncollision", true, { scriptInstance, b->scriptInstance });
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