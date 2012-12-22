//
//  LevelManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#import "LevelLoader.h"

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
    ~LevelManager();
    std::map<std::string, btCollisionShape*> sharedCollisionShapes; //!< All available collision shapes
    //std::map<std::string, v8::Handle> sharedScripts; //!< All available scripts
    FilePackage* levelPackage;
    std::string levelPackageName, levelId;
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

#endif
