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
    rapidxml::xml_node<xmlUsedCharType>* parameterNode;
    if(localData.size() > 0)
        node->append_node(scriptManager->writeCdataXMLNode(doc, localData));
    parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Gravity");
    btVector3 gravity = objectManager.physicsWorld->getGravity();
    parameterNode->value(doc.allocate_string(stringOf(gravity).c_str()));
    node->append_node(parameterNode);
    parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Ambient");
    parameterNode->value(doc.allocate_string(stringOf(objectManager.sceneAmbient).c_str()));
    node->append_node(parameterNode);
    
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
    parameterNode = node->first_node("Data");
    if(parameterNode)
        node->remove_node(parameterNode);
    if(globalData.size() > 0)
        node->append_node(scriptManager->writeCdataXMLNode(doc, globalData));
    return writeXmlFile(doc, statusFilePath, true);
}