//
//  LevelSaver.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//
//

#include "LevelManager.h"

LevelSaver::LevelSaver() :objectCounter(0) {
    
}

LevelSaver::~LevelSaver() {
    
}

void LevelSaver::pushObject(BaseObject* object) {
    for(auto iteratorInObject : object->links) {
        if(iteratorInObject.first == ".." || dynamic_cast<BoneObject*>(iteratorInObject.second->getOther(object)))
            continue; //Don't export BoneObject or parent links
        auto iteratorInSet = linkingMap.find(iteratorInObject.second);
        if(iteratorInSet == linkingMap.end()) {
            LinkSaver* linkSaver = new LinkSaver();
            linkSaver->index[0] = objectCounter;
            linkSaver->name[1] = iteratorInObject.first;
            linkingMap[iteratorInObject.second] = linkSaver;
        }else{
            LinkSaver* linkSaver = iteratorInSet->second;
            linkSaver->index[1] = objectCounter;
            linkSaver->name[0] = iteratorInObject.first;
        }
    }
    objectCounter ++;
}

bool LevelSaver::saveLevel() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* container = doc.allocate_node(rapidxml::node_element);
    container->name("Container");
    doc.append_node(container);
    
    //Write level tag
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("Level");
    container->append_node(node);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("gravity");
    btVector3 gravity = objectManager.physicsWorld->getGravity();
    std::string gravityStr = stringOf(gravity);
    attribute->value(gravityStr.c_str());
    node->append_attribute(attribute);
    
    //Save objects
    node = doc.allocate_node(rapidxml::node_element);
    node->name("Objects");
    container->append_node(node);
    
    for(unsigned int i = 0; i < objectManager.lightObjects.size(); i ++)
        node->append_node(objectManager.lightObjects[i]->write(doc, this));
    
    for(auto particlesObject : objectManager.particlesObjects)
        node->append_node(particlesObject->write(doc, this));
    
    for(auto simpleObject : objectManager.simpleObjects)
        node->append_node(simpleObject->write(doc, this));
    
    for(auto graphicObject : objectManager.graphicObjects)
        node->append_node(graphicObject->write(doc, this));
    
    //Save links
    node = doc.allocate_node(rapidxml::node_element);
    node->name("Links");
    container->append_node(node);
    for(auto iterator : linkingMap) {
        rapidxml::xml_node<xmlUsedCharType>* linkNode = doc.allocate_node(rapidxml::node_element);
        for(char i = 0; i < 2; i ++) {
            rapidxml::xml_node<xmlUsedCharType>* objectNode = doc.allocate_node(rapidxml::node_element);
            objectNode->name("Object");
            linkNode->append_node(objectNode);
            rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
            attribute->name("index");
            attribute->value(doc.allocate_string(stringOf(iterator.second->index[i]).c_str()));
            objectNode->append_attribute(attribute);
            attribute = doc.allocate_attribute();
            attribute->name("name");
            attribute->value(doc.allocate_string(iterator.second->name[i].c_str()));
            objectNode->append_attribute(attribute);
        }
        node->append_node(linkNode);
        iterator.first->write(doc, linkNode);
    }
    
    return writeXmlFile(doc, gameDataDir+"Saves/"+levelManager.saveGameName+"/Containers/"+levelManager.levelId+".xml", true);
}