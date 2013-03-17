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

bool LevelSaver::saveLevel() {
    v8::HandleScope handleScope;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* container = doc.allocate_node(rapidxml::node_element);
    container->name("Container");
    doc.append_node(container);
    
    //Write level tag
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("Level");
    container->append_node(node);
    rapidxml::xml_node<xmlUsedCharType>* property = doc.allocate_node(rapidxml::node_element);
    property->name("Gravity");
    btVector3 gravity = objectManager.physicsWorld->getGravity();
    property->value(doc.allocate_string(stringOf(gravity).c_str()));
    node->append_node(property);
    property = doc.allocate_node(rapidxml::node_element);
    property->name("Ambient");
    property->value(doc.allocate_string(stringOf(objectManager.sceneAmbient).c_str()));
    node->append_node(property);
    
    v8::Handle<v8::Value> scritData = scriptManager->callFunctionOfScript(scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName),
                                                                          "saveLocalData", false, { });
    if(!scritData.IsEmpty() && scritData->IsString()) {
        property = doc.allocate_node(rapidxml::node_element);
        property->name("Data");
        v8::String::Utf8Value dataStr(scritData);
        property->value(doc.allocate_string(*dataStr));
        node->append_node(property);
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
    return writeXmlFile(doc, containersPath+levelManager.levelId+".xml", true);
}