//
//  ObjectManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//
//

#include "DisplayObject.h"
#include "ParticlesObject.h"
#include "Audio.h"
#include "LightObject.h"

#ifndef ObjectManager_h
#define ObjectManager_h

//! A decal used for graphic effects
class Decal {
    public:
    btTransform transformation; //!< The world transformation of this decal
    std::shared_ptr<Texture> diffuse, //!< A color texture
                             heightMap; //!< A height map
    float life; //!< The remaining life time in seconds
};

//! This class manages all objects in the scene
class ObjectManager {
    public:
    LightObject* currentShadowLight; //!< The active light during a shadow map generation
    std::vector<LightObject*> lightObjects; //!< All light sources of the scene
    std::vector<AccumulatedTransparent*> transparentAccumulator; //!< Stores the transparent objects for deferred rendering
    std::set<Decal*> decals; //!< All Decals in the scene
    std::set<GraphicObject*> graphicObjects; //!< All GraphicObjects (without ParticlesObjects) in the scene
    std::set<ParticlesObject*> particlesObjects; //! All ParticlesObjects in the scene
    std::set<SimpleObject*> simpleObjects; //! All Cams and SoundSourceObjects in the scene
    
    btDefaultCollisionConfiguration* collisionConfiguration; //!< The physics collision configuration
    btCollisionDispatcher* collisionDispatcher; //!< The physics collision dispatcher
    btSequentialImpulseConstraintSolver* constraintSolver; //!< The physics constraint solver
    btBroadphaseInterface* broadphase; //!< The physics broadphase
    btDiscreteDynamicsWorld* physicsWorld; //!< The physics world
    
    ObjectManager();
    ~ObjectManager();
    //! Initialize
    void init();
    //! Deletes all Objects and the physics world
    void clear();
    //! Initializes the physics world
    void initPhysics();
    //! Calculate a game tick
    void gameTick();
    //! Calculate the physics
    void physicsTick();
    //! Renders the scene without LightObjects
    void drawScene();
    //! Renders all LightObjects
    void illuminate();
    //! Renders a graphics frame
    void drawFrame();
};

extern float animationFactor;
extern ObjectManager objectManager;

#endif
