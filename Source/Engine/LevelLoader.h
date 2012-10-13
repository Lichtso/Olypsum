//
//  LevelLoader.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import "Object.h"
#import "Light.h"
#import "ParticleSystem.h"
#import "Decals.h"
#import "FileManager.h"

#ifndef LevelLoader_h
#define LevelLoader_h

class LevelLoader {
    std::unique_ptr<char[]> collisionShapesData;
    std::map<std::string, rapidxml::xml_node<xmlUsedCharType>*> collisionShapeNodes;
    btCollisionShape* getCollisionShape(std::string name);
    
    public:
    LevelLoader();
    ~LevelLoader();
    bool loadLevel();
};

#endif