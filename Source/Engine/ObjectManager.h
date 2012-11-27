//
//  ObjectManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//
//

#import "DisplayObject.h"
#import "ParticlesObject.h"
#import "Audio.h"
#import "LightObject.h"

#ifndef ObjectManager_h
#define ObjectManager_h

class Decal {
    public:
    btTransform transformation;
    std::shared_ptr<Texture> diffuse, heightMap;
    float life;
};

class ObjectManager {
    public:
    LightObject* currentShadowLight;
    std::vector<LightObject*> lightObjects;
    std::vector<AccumulatedMesh*> transparentAccumulator;
    std::set<Decal*> decals;
    std::set<GraphicObject*> graphicObjects;
    std::set<ParticlesObject*> particlesObjects;
    std::set<SoundSourceObject*> soundSourceObjects;
    
    ObjectManager();
    ~ObjectManager();
    void init();
    void clear();
    void gameTick();
    void physicsTick();
    void drawScene();
    void illuminate();
    void drawFrame();
};

extern ObjectManager objectManager;

#endif
