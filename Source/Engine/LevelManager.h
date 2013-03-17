//
//  LevelManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#include "LevelLoader.h"
#include "LevelSaver.h"
#include "FileManager.h"

#ifndef LevelManager_h
#define LevelManager_h

enum GameStatusName {
    noGame = 0,
    editorMode = 1,
    localGame = 2,
    networkGame = 3
};

class LevelManager {
    public:
    LevelManager();
    void showErrorModal(const std::string& error);
    std::map<std::string, btCollisionShape*> sharedCollisionShapes; //!< All available collision shapes
    //std::map<std::string, v8::Handle> sharedScripts; //!< All available scripts
    FilePackage* levelPackage;
    std::string saveGameName, levelId;
    GameStatusName gameStatus = noGame;
    //! Deletes all shared objects
    void clear();
    void loadLevel(std::string levelId);
    void saveLevel();
    void leaveGame();
    bool loadGame(std::string name);
    bool newGame(std::string packageName, std::string name);
    bool removeGame(std::string name);
};

extern LevelManager levelManager;

template <class T>
std::shared_ptr<T> FileManager::initResource(rapidxml::xml_node<xmlUsedCharType>* node) {
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("package");
    FilePackage* levelPack = levelManager.levelPackage;
    if(attribute)
        levelPack = getPackage(attribute->value());
    attribute = node->first_attribute("src");
    if(!attribute) {
        log(error_log, "Tried to construct resource without \"src\"-attribute.");
        return NULL;
    }
    return levelPack->getResource<T>(attribute->value());
}

template <class T>
rapidxml::xml_node<xmlUsedCharType>* FileManager::writeResource(rapidxml::xml_document<xmlUsedCharType>& doc, const char* nodeName, std::shared_ptr<T>& resource) {
    for(auto iterator : filePackages) {
        std::string name = iterator.second->getNameOfResource(resource);
        if(name.size() > 0) {
            rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
            node->name(nodeName);
            rapidxml::xml_attribute<xmlUsedCharType>* attribute;
            if(iterator.second != levelManager.levelPackage) {
                attribute = doc.allocate_attribute();
                attribute->name("package");
                node->append_attribute(attribute);
                attribute->value(doc.allocate_string(iterator.second->name.c_str()));
            }
            attribute = doc.allocate_attribute();
            attribute->name("src");
            node->append_attribute(attribute);
            attribute->value(doc.allocate_string(name.c_str()));
            return node;
        }
    }
    return NULL;
}

#endif
