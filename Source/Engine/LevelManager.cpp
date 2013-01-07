//
//  LevelManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#import "Menu.h"
#import "LevelManager.h"

LevelManager::LevelManager() {
    
}

LevelManager::~LevelManager() {
    clear();
}

void LevelManager::clear() {
    for(auto iterator: sharedCollisionShapes)
        delete iterator.second;
    sharedCollisionShapes.clear();
}

void LevelManager::loadLevel(std::string nextLevelId) {
    levelId = nextLevelId;
    LevelLoader levelLoader;
    levelLoader.loadLevel();
}

void LevelManager::saveLevel() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+saveGameName+'/'+"Status.xml", false);
    doc.first_node("Status")->first_node("Level")->first_attribute("value")->value(levelId.c_str());
    writeXmlFile(doc, gameDataDir+"Saves/"+saveGameName+'/'+"Status.xml", true);
    
    LevelSaver levelSaver;
    levelSaver.saveLevel();
}

void LevelManager::leaveGame() {
    saveLevel();
    
    levelId = "";
    saveGameName = "";
    levelPackage = NULL;
    gameStatus = noGame;
    objectManager.clear();
    clear();
    fileManager.clear();
    fileManager.getPackage("Default");
    setMenu(mainMenu);
}

bool LevelManager::loadGame(std::string name) {
    std::string path = gameDataDir+"Saves/"+name+'/';
    if(!checkDir(path)) {
        log(error_log, "Could not find a saved game by this name.");
        return false;
    }
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", false);
    if(!fileData) {
        log(error_log, "Could not load saved game, because Status.xml is missing.");
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.first_node("Status");
    if(strcmp(statusNode->first_node("Version")->first_attribute("value")->value(), VERSION) != 0) {
        log(error_log, "Could not load saved game, because its version is outdated.");
        return false;
    }
    saveGameName = name;
    levelPackage = fileManager.getPackage(statusNode->first_node("Package")->first_attribute("value")->value());
    loadLevel(statusNode->first_node("Level")->first_attribute("value")->value());
    return true;
}

bool LevelManager::newGame(std::string packageName, std::string name) {
    if(!createDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not create new game, because the name already exists.");
        return false;
    }
    createDir(gameDataDir+"Saves/"+name+"/Containers/");
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.allocate_node(rapidxml::node_element);
    statusNode->name("Status");
    doc.append_node(statusNode);
    addXMLNode(doc, statusNode, "Version", VERSION);
    addXMLNode(doc, statusNode, "Package", packageName.c_str());
    addXMLNode(doc, statusNode, "Level", "start");
    writeXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", true);
    return loadGame(name);
}

bool LevelManager::removeGame(std::string name) {
    if(!removeDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not remove a saved game by this name.");
        return false;
    }
    return true;
}

LevelManager levelManager;