//
//  LevelSaver.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//
//

#include "AppMain.h"

LevelSaver::LevelSaver() :objectCounter(0) {
    
}

LevelSaver::~LevelSaver() {
    for(auto iterator : linkingMap)
        delete iterator.second;
}

void LevelSaver::pushObject(BaseObject* object) {
    for(auto iteratorInObject : object->links) {
        auto iteratorInSet = linkingMap.find(iteratorInObject.second);
        if(iteratorInSet == linkingMap.end()) {
            if(dynamic_cast<BoneObject*>(object) && iteratorInObject.first == "..")
                continue; //Object 0 is child and BoneObject
            if(dynamic_cast<BoneObject*>(iteratorInObject.second->getOther(object)) && iteratorInObject.first != "..")
                continue; //Object 1 is child and BoneObject
            
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

bool LevelSaver::saveLevel(const std::string& localData, const std::string& globalData) {
    v8::HandleScope handleScope;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* container = doc.allocate_node(rapidxml::node_element);
    container->name("Container");
    doc.append_node(container);
    
    //Write level tag
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("Level");
    container->append_node(node);
    rapidxml::xml_node<xmlUsedCharType>* property;
    if(localData.size() > 0) {
        property = doc.allocate_node(rapidxml::node_cdata);
        node->append_node(property);
        property->value(doc.allocate_string(localData.c_str()));
    }
    property = doc.allocate_node(rapidxml::node_element);
    property->name("Gravity");
    btVector3 gravity = objectManager.physicsWorld->getGravity();
    property->value(doc.allocate_string(stringOf(gravity).c_str()));
    node->append_node(property);
    property = doc.allocate_node(rapidxml::node_element);
    property->name("Ambient");
    property->value(doc.allocate_string(stringOf(objectManager.sceneAmbient).c_str()));
    node->append_node(property);
    
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
    
    std::string containersPath = gameDataDir+"Saves/"+levelManager.saveGameName+"/Containers/";
    createDir(containersPath);
    if(!writeXmlFile(doc, containersPath+levelManager.levelId+".xml", true))
        return false;
    
    doc.clear();
    std::string statusFilePath = gameDataDir+"Saves/"+levelManager.saveGameName+"/Status.xml";
    std::unique_ptr<char[]> fileData = readXmlFile(doc, statusFilePath, true);
    node = doc.first_node("Status");
    node->first_node("Level")->first_attribute("value")->value(levelManager.levelId.c_str());
    property = node->first_node();
    if(property->type() != rapidxml::node_cdata)
        property = NULL;
    if(globalData.size() > 0) {
        if(!property) {
            property = doc.allocate_node(rapidxml::node_cdata);
            node->prepend_node(property);
        }
        property->value(doc.allocate_string(globalData.c_str()));
    }else if(property)
        node->remove_node(property);
    return writeXmlFile(doc, statusFilePath, true);
}