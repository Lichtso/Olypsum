//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import "ObjectManager.h"

#ifndef LevelLoader_h
#define LevelLoader_h

class LevelLoader {
    unsigned int objectLinkingScope, objectLinkingOffset;
    std::vector<BaseObject*> objectLinkingIndex;
    std::set<std::string> containerStack;
    std::unique_ptr<char[]> collisionShapesData;
    std::map<std::string, rapidxml::xml_node<xmlUsedCharType>*> collisionShapeNodes;
    void deleteCollisionShapeNode(std::string name);
    public:
    btTransform transformation;
    LevelLoader();
    ~LevelLoader();
    btCollisionShape* getCollisionShape(std::string name);
    BaseObject* getObjectLinking(const char* id);
    void pushObject(BaseObject* object);
    bool loadContainer(std::string name);
    bool loadLevel();
};

#endif