//
//  LevelManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#ifndef LevelManager_h
#define LevelManager_h

#include "LevelSaver.h"

enum GameStatusName {
    noGame = 0,
    editorMode = 1,
    localGame = 2,
    networkGame = 3
};

class LevelManager {
    public:
    std::map<std::string, btCollisionShape*> sharedCollisionShapes; //!< All available collision shapes
    FilePackage* levelPackage;
    std::string saveGameName, levelId;
    GameStatusName gameStatus = noGame;
    
    LevelManager();
    void showErrorModal(const std::string& error);
    btCollisionShape* getCollisionShape(const std::string& name);
    std::string getCollisionShapeName(btCollisionShape* shape);
    //! Deletes all shared objects
    void clear();
    bool loadLevel(const std::string& levelPackage, const std::string& levelId);
    bool saveLevel(const std::string& localData, const std::string& globalData);
    bool loadGame(const std::string& name);
    bool newGame(const std::string& packageName, const std::string& name);
    bool removeGame(const std::string& name);
};

extern LevelManager levelManager;

#endif
