//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#include "ObjectManager.h"

#ifndef LevelLoader_h
#define LevelLoader_h

class LevelLoader {
    unsigned int objectLinkingScope, objectLinkingOffset;
    std::vector<BaseObject*> objectLinkingIndex;
    std::set<std::string> containerStack;
    public:
    btTransform transformation;
    LevelLoader();
    btCollisionShape* getCollisionShape(std::string name);
    BaseObject* getObjectLinking(const char* id);
    void pushObject(BaseObject* object);
    bool loadContainer(std::string name);
    bool loadLevel(std::string levelId);
};

#endif