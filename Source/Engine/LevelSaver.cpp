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
    for(auto iterator : linkingMap)
        delete iterator.second;
}

void LevelSaver::pushObject(BaseObject* object) {
    for(auto iteratorInObject : object->links) {
        if(iteratorInObject.first == ".." || dynamic_cast<BoneObject*>(iteratorInObject.second->getOther(object)))
            continue; //Don't export BoneObject or parent links
        auto iteratorInSet = linkingMap.find(iteratorInObject.second);
        if(iteratorInSet == linkingMap.end()) {
            LinkInitializer* linkSaver = new LinkInitializer();
            linkSaver->index[0] = objectCounter;
            linkSaver->object[0] = object;
            linkSaver->name[1] = iteratorInObject.first;
            linkingMap[iteratorInObject.second] = linkSaver;
        }else{
            LinkInitializer* linkSaver = iteratorInSet->second;
            linkSaver->index[1] = objectCounter;
            linkSaver->object[1] = object;
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
    for(auto iterator : linkingMap)
        node->append_node(iterator.first->write(doc, iterator.second));
    
    return writeXmlFile(doc, gameDataDir+"Saves/"+levelManager.saveGameName+"/Containers/"+levelManager.levelId+".xml", true);
}