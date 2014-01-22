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
    std::unordered_map<BaseLink*, LinkInitializer*> linkingMap;
    public:
    LevelSaver();
    void pushObject(BaseObject* object);
    bool saveLevel(v8::Handle<v8::Value> localData, v8::Handle<v8::Value> globalData, v8::Handle<v8::Value> description);
};

#endif