//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef LevelLoader_h
#define LevelLoader_h

#include "GUI/GUIImage.h"

class LevelLoader {
    unsigned int objectLinkingScope, objectLinkingOffset;
    std::vector<BaseObject*> objectLinkingIndex;
    std::set<std::string> containerStack;
    public:
    FilePackage* filePackage;
    btTransform transformation;
    LevelLoader();
    v8::Handle<v8::Array> getResultsArray();
    BaseObject* getObjectLinking(unsigned int index);
    void pushObject(BaseObject* object);
    bool loadContainer(std::string name, bool isLevelRoot);
    bool loadLevel();
};

#endif