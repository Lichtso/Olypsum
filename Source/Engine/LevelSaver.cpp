//
//  LevelSaver.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

LevelSaver::LevelSaver() :objectCounter(0) {
    
}

void LevelSaver::pushObject(BaseObject* object) {
    for(auto iteratorInObject : object->links) {
        auto iteratorInSet = linkingMap.find(iteratorInObject);
        if(iteratorInSet == linkingMap.end()) {
            LinkInitializer* linkSaver = new LinkInitializer();
            linkSaver->index[0] = objectCounter;
            linkSaver->object[0] = object;
            linkingMap[iteratorInObject] = linkSaver;
        }else{
            LinkInitializer* linkSaver = iteratorInSet->second;
            linkSaver->index[1] = objectCounter;
            linkSaver->object[1] = object;
            if(linkSaver->object[0] != iteratorInObject->a)
                linkSaver->swap();
        }
    }
    objectCounter ++;
}

bool LevelSaver::saveLevel(v8::Handle<v8::Value> localData, v8::Handle<v8::Value> globalData, v8::Handle<v8::Value> description) {
    ScriptScope();
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* container = doc.allocate_node(rapidxml::node_element);
    container->name("Container");
    doc.append_node(container);
    
    //Write level tag
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("Level");
    container->append_node(node);
    scriptManager->writeCdataXMLNode(doc, node, "Data", localData);
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Gravity");
    btVector3 gravity = objectManager.physicsWorld->getGravity();
    parameterNode->value(doc.allocate_string(stringOf(gravity).c_str()));
    node->append_node(parameterNode);
    parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Ambient");
    parameterNode->value(doc.allocate_string(stringOf(objectManager.sceneAmbient).c_str()));
    node->append_node(parameterNode);
    if(objectManager.sceneFogDistance > 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("Fog");
        node->append_node(parameterNode);
        attribute = doc.allocate_attribute();
        attribute->name("color");
        attribute->value(doc.allocate_string(stringOf(objectManager.sceneFogColor).c_str()));
        parameterNode->append_attribute(attribute);
        attribute = doc.allocate_attribute();
        attribute->name("distance");
        attribute->value(doc.allocate_string(stringOf(objectManager.sceneFogDistance).c_str()));
        parameterNode->append_attribute(attribute);
    }
    
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
    
    for(auto matterObject : objectManager.matterObjects)
        node->append_node(matterObject->write(doc, this));
    
    //Save links
    node = doc.allocate_node(rapidxml::node_element);
    node->name("Links");
    container->append_node(node);
    for(auto iterator : linkingMap) {
        node->append_node(iterator.first->write(doc, iterator.second));
        delete iterator.second;
    }
    linkingMap.clear();

    std::string containersPath = supportPath+"Saves/"+levelManager.saveGameName+"/Containers/";
    createDir(containersPath);
    if(!writeXmlFile(doc, containersPath+levelManager.levelContainer+".xml", true))
        return false;
    
    doc.clear();
    std::string statusFilePath = supportPath+"Saves/"+levelManager.saveGameName+"/Status.xml";
    std::unique_ptr<char[]> fileData = readXmlFile(doc, statusFilePath, true);
    node = doc.first_node("Status");
    node->first_node("EngineVersion")->first_attribute()->value(VERSION);
    node->first_node("Level")->first_attribute()->value(levelManager.levelContainer.c_str());
    scriptManager->writeCdataXMLNode(doc, node, "Data", globalData);
    scriptManager->writeCdataXMLNode(doc, node, "Description", description);
    return writeXmlFile(doc, statusFilePath, true);
}