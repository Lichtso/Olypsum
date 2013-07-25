//
//  ParticlesObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Audio.h"

#ifndef ParticlesObject_h
#define ParticlesObject_h

//! A DisplayObject used as particle system
class ParticlesObject : public DisplayObject {
    //! A Particle of a ParticlesObject
    struct Particle {
        btVector3 pos, //!< Its position
                  dir; //!< Its direction/velocity
        float life, //!< The time until it dies in seconds
              size; //!< The size in world units
    } *particles; //!< The particles of this ParticlesObject
    GLuint particlesVAO[2], particlesVBO[2];
    bool activeVAO;
    ParticlesObject();
    //! Internal initialize arrays and OpenGL buffers
    void init();
    void clean();
    public:
    unsigned int maxParticles; //!< The maximal amount of simultaneously existing particles in the system
    bool transformAligned; //!< If set to false the particles will be orientated to view
    btVector3 posMin, //!< The negative position vector relative to the system for spawning new particles
              posMax, //!< The positive position vector relative to the system for spawning new particles
              dirMin, //!< The negative velocity vector for spawning new particles
              dirMax, //!< The positive velocity vector for spawning new particles
              force; //!< A force to be applied to all particles every frame like gravity
    float lifeMin, //!< The minimal life time of a new particle in seconds
          lifeMax, //!< The maximal life time of a new particle in seconds
          sizeMin, //!< The minimal size of a new particle in world units
          sizeMax, //!< The maximal size of a new particle in world units
          systemLife; //!< The life time of the entire system in seconds until it is deleted or -1.0 if it is infinite
    FileResourcePtr<Texture> texture; //!< The texture of the particles
    /*! Constructs a new particle system
     @param maxParticles The maximal amount of simultaneously existing particles in the system
     */
    ParticlesObject(unsigned int maxParticles, btCollisionShape* collisionShape);
    ParticlesObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    void removeFast();
    void setTransformation(const btTransform& transformation);
    btTransform getTransformation();
    void newScriptInstance();
    bool gameTick();
    void draw();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif
