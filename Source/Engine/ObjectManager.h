//
//  ObjectManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef ObjectManager_h
#define ObjectManager_h

#include "LightObject.h"

//! This class manages all objects in the scene
class ObjectManager {
    public:
    bool currentShadowIsParabolid; //!< True if the active light during a shadow map generation is a PositionalLight
    LightObject* currentShadowLight; //!< The active light during a shadow map generation
    Reflective* currentReflective; //!< The active reflector during a reflection of the scene
    
    std::unordered_map<RigidObject*, Reflective*> reflectiveAccumulator; //!< Stores the mirrors for deferred rendering
    std::vector<AccumulatedTransparent*> transparentAccumulator; //!< Stores the transparent objects for deferred rendering
    std::vector<LightObject*> lightObjects; //!< All light sources of the scene
    std::unordered_set<MatterObject*> matterObjects; //!< All MatterObjects in the scene
    std::unordered_set<ParticlesObject*> particlesObjects; //! All ParticlesObjects in the scene
    std::unordered_set<SimpleObject*> simpleObjects; //! All SimpleObjects in the scene
    btVector3 sceneAmbient, //! Ambient light color
              sceneFogColor; //! Fog color afloat
    float sceneFogDistance; //! Fog distance afloat
    
    btCollisionConfiguration* collisionConfiguration; //!< The physics collision configuration
    btCollisionDispatcher* collisionDispatcher; //!< The physics collision dispatcher
    btConstraintSolver* constraintSolver; //!< The physics constraint solver
    btSoftBodySolver* softBodySolver; //!< The physics soft body solver
    btSoftBodySolverOutputCLtoGL* softBodyOutput; //!< The physics soft body output
    btBroadphaseInterface* broadphase; //!< The physics broadphase
    std::unique_ptr<btSoftRigidDynamicsWorld> physicsWorld; //!< The physics world
    
    ObjectManager();
    ~ObjectManager();
    //! Initialize OpenCL
    bool initOpenCL();
    //! Initialize
    void init();
    //! Deletes all Objects and the physics world
    void clear();
    //! Initializes a new game
    void initGame();
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
