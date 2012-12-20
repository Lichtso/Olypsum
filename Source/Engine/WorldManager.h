//
//  WorldManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#import "LevelLoader.h"

#ifndef WorldManager_h
#define WorldManager_h

enum GameStatusName {
    noGame = 0,
    editorMode = 1,
    localGame = 2,
    networkGame = 3
};

class WorldManager {
    public:
    WorldManager();
    ~WorldManager();
    FilePackage* levelPackage;
    std::string levelPackageName, levelId;
    GameStatusName gameStatus = noGame;
    void loadLevel();
    void saveLevel();
    void leaveGame();
    bool loadGame(std::string name);
    bool newGame(std::string packageName, std::string name);
    bool removeGame(std::string name);
};

extern WorldManager worldManager;

#endif
