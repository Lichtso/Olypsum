//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import "ObjectManager.h"
#import "FileManager.h"
#import "TextFont.h"

#ifndef LevelLoader_h
#define LevelLoader_h

class LevelLoader {
    unsigned int objectLinkingScope, objectLinkingOffset;
    std::vector<BaseObject*> objectLinkingIndex;
    std::set<std::string> containerStack;
    std::unique_ptr<char[]> collisionShapesData;
    std::map<std::string, BaseObject*> namedObjects;
    std::map<std::string, rapidxml::xml_node<xmlUsedCharType>*> collisionShapeNodes;
    void deleteCollisionShapeNode(std::string name);
    public:
    btTransform transformation;
    LevelLoader();
    ~LevelLoader();
    btCollisionShape* getCollisionShape(std::string name);
    BaseObject* getObjectLinking(const char* id);
    bool loadContainer(std::string name);
    bool loadLevel();
};

#endif