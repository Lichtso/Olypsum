//
//  LevelLoader.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

LevelLoader::LevelLoader() :transformation(btTransform::getIdentity()), filePackage(levelManager.levelPackage) {
    
}

v8::Handle<v8::Array> LevelLoader::getResultsArray() {
    //ScriptScope();
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(v8::Isolate::GetCurrent());
    for(auto hit : objectLinkingIndex)
        objects->Set(i ++, v8::Handle<v8::Object>(*hit->scriptInstance));
    return objects; //handleScope.Close(objects);
}

BaseObject* LevelLoader::getObjectLinking(unsigned int index) {
    index += objectLinkingOffset;
    if(index < objectLinkingScope || index >= objectLinkingIndex.size()) {
        log(error_log, std::string("Object linking offset out of scope: "+stringOf(index)+'.'));
        return NULL;
    }
    return objectLinkingIndex[index];
}

void LevelLoader::pushObject(BaseObject* object) {
    objectLinkingIndex.push_back(object);
}

bool LevelLoader::loadContainer(std::string name, bool isLevelRoot) {
    ScriptScope();
    std::unique_ptr<char[]> rawData;
    rapidxml::xml_document<xmlUsedCharType> doc;
    
    if(isLevelRoot) {
        rawData = readXmlFile(doc, supportPath+"Saves/"+levelManager.saveGameName+"/Containers/"+name+".xml", false);
        if(!rawData)
            rawData = readXmlFile(doc, filePackage->path+"/Containers/"+name+".xml", false);
    }else if(fileManager.readResourcePath(filePackage, name))
        rawData = readXmlFile(doc, filePackage->path+"/Containers/"+name+".xml", false);
    
    if(!rawData) {
        menu.setModalView("error", fileManager.localizeString("packageError_ContainerMissing")+'\n'+name, nullptr);
        return false;
    }
    
    rapidxml::xml_node<xmlUsedCharType>* containerNode = doc.first_node("Container");
    if(!containerNode) {
        log(error_log, std::string("Could not find \"Container\"-node in container: ")+name+'.');
        return false;
    }
    //Push the container on the stack and avoid self recursion
    if(containerStack.find(name) != containerStack.end()) {
        log(error_log, std::string("Container contains its self: ")+name+'.');
        return false;
    }
    containerStack.insert(name);
    FilePackage* prevFilePackage = filePackage;
    unsigned int objectLinkingScopePrev = objectLinkingIndex.size();
    
    //Check for Level-node
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    rapidxml::xml_node<xmlUsedCharType>* levelNode = containerNode->first_node("Level");
    if(levelNode) {
        if(!isLevelRoot) {
            log(error_log, "Found a \"Level\"-node in a child container.");
            return false;
        }
        rapidxml::xml_node<xmlUsedCharType>* parameterNode = levelNode->first_node("Gravity");
        if(!parameterNode) {
            log(error_log, "Tried to construct Level without \"Gravity\"-node.");
            return false;
        }
        XMLValueArray<float> vecData;
        vecData.readString(parameterNode->value(), "%f");
        objectManager.physicsWorld->setGravity(vecData.getVector3());
        parameterNode = levelNode->first_node("Ambient");
        if(parameterNode) {
            vecData.readString(parameterNode->value(), "%f");
            objectManager.sceneAmbient = vecData.getVector3();
        }
        parameterNode = levelNode->first_node("Fog");
        if(parameterNode) {
            attribute = parameterNode->first_attribute("color");
            if(attribute) {
                vecData.readString(attribute->value(), "%f");
                objectManager.sceneFogColor = vecData.getVector3();
            }
            attribute = parameterNode->first_attribute("distance");
            if(attribute)
                sscanf(attribute->value(), "%f", &objectManager.sceneFogDistance);
        }
    }else if(isLevelRoot) {
        log(error_log, "Root container does not contain a \"Level\"-node.");
        return false;
    }
    
    //Load containers
    rapidxml::xml_node<xmlUsedCharType>* node = containerNode->first_node("Container");
    while(node) {
        attribute = node->first_attribute("src");
        if(!attribute) {
            log(error_log, "Tried to construct Container without \"src\"-attribute.");
            return false;
        }
        btTransform parentTransform = transformation;
        transformation *= readTransformationXML(node);
        if(!loadContainer(attribute->value(), false)) return false;
        transformation = parentTransform;
        filePackage = prevFilePackage;
        node = node->next_sibling("Container");
    }
    
    objectLinkingScope = objectLinkingScopePrev;
    objectLinkingOffset = objectLinkingIndex.size();
    
    //Load objects
    node = containerNode->first_node("Objects");
    if(node) {
        node = node->first_node();
        while(node) {
            BaseObject* object;
            if(strcmp(node->name(), "RigidObject") == 0) {
                object = new RigidObject(node, this);
            }else if(strcmp(node->name(), "PhysicObject") == 0) {
                object = new PhysicObject(node, this);
            }else if(strcmp(node->name(), "LightObject") == 0) {
                attribute = node->first_attribute("type");
                if(!attribute) {
                    log(error_log, "Tried to construct LightObject without \"type\"-attribute.");
                    return false;
                }
                if(strcmp(attribute->value(), "positional") == 0)
                    object = new PositionalLight(node, this);
                else if(strcmp(attribute->value(), "spot") == 0)
                    object = new SpotLight(node, this);
                else if(strcmp(attribute->value(), "directional") == 0)
                    object = new DirectionalLight(node, this);
            }else if(strcmp(node->name(), "ParticlesObject") == 0) {
                object = new ParticlesObject(node, this);
            }else if(strcmp(node->name(), "SoundObject") == 0) {
                object = new SoundObject(node, this);
            }else if(strcmp(node->name(), "SimpleObject") == 0) {
                object = new SimpleObject(node, this);
            }else if(strcmp(node->name(), "CamObject") == 0) {
                object = new CamObject(node, this);
            }else if(strcmp(node->name(), "TerrainObject") == 0) {
                object = new TerrainObject(node, this);
            }else{
                log(error_log, std::string("Tried to construct invalid Object: ")+node->name()+'.');
                return false;
            }
            
            object->initScriptNode(filePackage, node);
            node = node->next_sibling();
        }
    }
    
    //Load links
    node = containerNode->first_node("Links");
    if(node) {
        node = node->first_node();
        while(node) {
            BaseLink* link;
            if(strcmp(node->name(), "BaseLink") == 0) {
                link = new BaseLink();
            }else if(strcmp(node->name(), "PhysicLink") == 0) {
                link = new PhysicLink();
            }else if(strcmp(node->name(), "TransformLink") == 0) {
                link = new TransformLink();
            }else if(strcmp(node->name(), "BoneLink") == 0) {
                link = new BoneLink();
            }else{
                log(error_log, std::string("Tried to construct invalid Link: ")+node->name()+'.');
                return false;
            }
            
            link->init(node, this);
            link->initScriptNode(filePackage, node);
            node = node->next_sibling();
        }
    }
    
    //Pop the container from the stack
    containerStack.erase(name);
    
    //Update status file
    if(levelNode) {
        v8::Handle<v8::Value> localData = scriptManager->readCdataXMLNode(levelNode);
        doc.clear();
        std::string statusFilePath = supportPath+"Saves/"+levelManager.saveGameName+"/Status.xml";
        std::unique_ptr<char[]> fileData = readXmlFile(doc, statusFilePath, true);
        node = doc.first_node("Status");
        node->first_node("Level")->first_attribute()->value(name.c_str());
        v8::Handle<v8::Value> globalData = scriptManager->readCdataXMLNode(node);
        if(!writeXmlFile(doc, statusFilePath, true))
            return false;
        v8::Handle<v8::Value> argv[] = { v8::Handle<v8::Value>(*localData), v8::Handle<v8::Value>(*globalData) };
        levelManager.mainScript->callFunction("onload", false, 2, argv);
        if(!mainCam) {
            log(error_log, "No CamObject was set as mainCam.");
            return false;
        }
    }
    return true;
}

bool LevelLoader::loadLevel() {
    objectManager.initGame();
    
    //Load root conatiner
    if(!loadContainer(levelManager.levelContainer, true)) {
        levelManager.clear();
        menu.setSaveGamesMenu();
        if(!menu.screenView->modalView)
            menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
        return false;
    }
    
    //Start the game
    objectManager.updateRendererSettings();
    menu.setInGameMenu();
    return true;
}