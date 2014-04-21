//
//  LevelManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
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
    std::unordered_map<std::string, btCollisionShape*> sharedCollisionShapes; //!< All available collision shapes
    FilePackage* levelPackage;
    std::string saveGameName, levelContainer;
    GameStatusName gameStatus = noGame;
    
    LevelManager();
    btCollisionShape* getCollisionShape(const std::string& name);
    const char* getCollisionShapeName(btCollisionShape* shape);
    //! Deletes all shared objects
    void clear();
    bool loadGame(FilePackage* package, const std::string& name, const std::string& container);
    bool newGame(FilePackage* package, const std::string& name);
};

extern LevelManager levelManager;

#endif
