//
//  LevelSaver.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LevelSaver_h
#define LevelSaver_h

#include "LevelLoader.h"

class LevelSaver {
    int objectCounter;
    std::map<BaseLink*, LinkInitializer*> linkingMap;
    public:
    LevelSaver();
    ~LevelSaver();
    void pushObject(BaseObject* object);
    bool saveLevel(const std::string& localData, const std::string& globalData);
};

#endif