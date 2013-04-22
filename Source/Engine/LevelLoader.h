//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#ifndef LevelLoader_h
#define LevelLoader_h

#include "GUIImage.h"

class LevelLoader {
    unsigned int objectLinkingScope, objectLinkingOffset;
    std::vector<BaseObject*> objectLinkingIndex;
    std::set<std::string> containerStack;
    public:
    btTransform transformation;
    LevelLoader();
    v8::Handle<v8::Array> getResultsArray();
    BaseObject* getObjectLinking(const char* id);
    void pushObject(BaseObject* object);
    bool loadContainer(std::string name, bool isLevelRoot);
    bool loadLevel(const std::string& levelPackage, const std::string& levelId);
};

#endif