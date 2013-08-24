//
//  LevelManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

LevelManager::LevelManager() {
    
}

btCollisionShape* LevelManager::getCollisionShape(const std::string& name) {
    btCollisionShape* shape = levelManager.sharedCollisionShapes[name];
    if(shape) return shape;
    log(error_log, std::string("Couldn't find collision shape with id ")+name+'.');
    return NULL;
}

std::string LevelManager::getCollisionShapeName(btCollisionShape* shape) {
    for(auto iterator : sharedCollisionShapes)
        if(iterator.second == shape)
            return iterator.first;
    return "";
}

void LevelManager::clear() {
    if(scriptManager) {
        ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
        if(script) script->callFunction("onleave", false, { });
    }
    saveGameName = levelContainer = "";
    levelPackage = NULL;
    gameStatus = noGame;
    for(auto iterator: sharedCollisionShapes)
        delete iterator.second;
    sharedCollisionShapes.clear();
    objectManager.clear();
    menu.setMenu(Menu::Name::saveGames);
}

bool LevelManager::newGame() {
    if(!createDir(gameDataDir+"Saves/"+saveGameName+'/'))
        return false;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* rootNode = doc.allocate_node(rapidxml::node_element);
    rootNode->name("Status");
    doc.append_node(rootNode);
    addXMLNode(doc, rootNode, "EngineVersion", VERSION);
    addXMLNode(doc, rootNode, "Package", levelPackage->name.c_str());
    addXMLNode(doc, rootNode, "Level", levelContainer.c_str());
    writeXmlFile(doc, gameDataDir+"Saves/"+saveGameName+"/Status.xml", true);
    LevelLoader levelLoader;
    return levelLoader.loadLevel();
}

LevelManager levelManager;