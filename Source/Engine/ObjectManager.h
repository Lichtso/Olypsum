//
//  ObjectManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "LightObject.h"

#ifndef ObjectManager_h
#define ObjectManager_h

//! This class manages all objects in the scene
class ObjectManager {
    public:
    bool currentShadowIsParabolid; //!< True if the active light during a shadow map generation is a PositionalLight
    LightObject* currentShadowLight; //!< The active light during a shadow map generation
    Reflective* currentReflective; //!< The active reflector during a reflection of the scene
    
    std::map<RigidObject*, Reflective*> reflectiveAccumulator; //!< Stores the mirrors for deferred rendering
    std::vector<AccumulatedTransparent*> transparentAccumulator; //!< Stores the transparent objects for deferred rendering
    std::vector<LightObject*> lightObjects; //!< All light sources of the scene
    std::set<GraphicObject*> graphicObjects; //!< All GraphicObjects (without ParticlesObjects) in the scene
    std::set<ParticlesObject*> particlesObjects; //! All ParticlesObjects in the scene
    std::set<SimpleObject*> simpleObjects; //! All Cams and SoundObjects in the scene
    btVector3 sceneAmbient; //! Ambient light color
    
    PosixThreadSupport* bulletThreadSupport; //!< The physics multi thread support
    btCollisionConfiguration* collisionConfiguration; //!< The physics collision configuration
    btCollisionDispatcher* collisionDispatcher; //!< The physics collision dispatcher
    btConstraintSolver* constraintSolver; //!< The physics constraint solver
    btSoftBodySolver* softBodySolver; //!< The physics soft body solver
    btBroadphaseInterface* broadphase; //!< The physics broadphase
    std::unique_ptr<btSoftRigidDynamicsWorld> physicsWorld; //!< The physics world
    
    ObjectManager();
    ~ObjectManager();
    //! Initialize
    void init();
    //! Deletes all Objects and the physics world
    void clear();
    /*! Initializes a new game
     @param levelPackage The name of the FilePackage to be used as levelPackage
     */
    void initGame(const std::string& levelPackage);
    //! Calculate a game tick
    void gameTick();
    //! Calculate the physics
    void physicsTick();
    //! Renders the scene for the shadow maps of LightObjects
    void drawShadowCasters();
    //! Renders all LightObjects
    void illuminate();
    /*! Renders a graphics frame
     * @param renderTarget The buffer to be used as render traget
     */
    void drawFrame(GLuint renderTarget);
    //! Updates the scenes renderer settings
    void updateRendererSettings();
};

extern ObjectManager objectManager;

#endif
