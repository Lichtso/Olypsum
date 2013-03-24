//
//  LevelSaver.h
//  Olypsum
//
//  Created by Alexander Meißner on 30.12.12.
//
//

#include "ObjectManager.h"

#ifndef LevelSaver_h
#define LevelSaver_h

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