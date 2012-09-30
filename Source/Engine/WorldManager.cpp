//
//  WorldManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#include "WorldManager.h"

void WorldManager::clear() {
    soundSourcesManager.clear();
    objectManager.clear();
    particleSystemManager.clear();
    decalManager.clear();
    lightManager.clear();
}

void WorldManager::loadLevel() {
    gameStatus = localGame;
    setMenu(inGameMenu);
}

void WorldManager::saveLevel() {
    unsigned int fileSize;
    rapidxml::xml_document<xmlUsedCharType> doc;
    char* fileData = readXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", fileSize, false);
    char mapStr[8];
    sprintf(mapStr, "%d", levelId);
    doc.first_node("level")->first_attribute("value")->value(mapStr);
    writeXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml");
    delete [] fileData;
}

void WorldManager::leaveGame() {
    levelId = -1;
    gameName = "";
    gameStatus = noGame;
    clear();
    fileManager.clear();
    setMenu(mainMenu);
}

bool WorldManager::loadGame(std::string name) {
    std::string path = gameDataDir+"Saves/"+name+'/';
    if(!checkDir(path)) {
        log(error_log, "Could not find a saved game by this name.");
        return false;
    }
    
    unsigned int fileSize;
    rapidxml::xml_document<xmlUsedCharType> doc;
    char* fileData = readXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", fileSize, false);
    if(!fileData || strcmp(doc.first_node("version")->first_attribute("value")->value(), VERSION) != 0) {
        log(error_log, "Could not load saved game, because its version is outdated.");
        delete [] fileData;
        return false;
    }
    fileManager.clear();
    fileManager.loadPackage(doc.first_node("package")->first_attribute("value")->value());
    sscanf(doc.first_node("level")->first_attribute("value")->value(), "%d", &levelId);
    delete [] fileData;
    
    gameName = name;
    loadLevel();
    
    return true;
}

bool WorldManager::newGame(std::string packageName, std::string name) {
    if(!createDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not create new game, because the name already exists.");
        return false;
    }
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    addXMLNode(doc, &doc, "version", VERSION);
    addXMLNode(doc, &doc, "package", packageName.c_str());
    addXMLNode(doc, &doc, "level", "0");
    writeXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml");
    return loadGame(name);
}

bool WorldManager::removeGame(std::string name) {
    if(!removeDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not remove a saved game by this name.");
        return false;
    }
    return true;
}

WorldManager worldManager;