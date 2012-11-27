//
//  ParticlesObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "DisplayObject.h"

#ifndef ParticlesObject_h
#define ParticlesObject_h

//! A DisplayObject used as particle system
class ParticlesObject : public DisplayObject {
    //! A Particle of a ParticlesObject
    struct Particle {
        btVector3 pos, //!< Its position
                  dir; //!< Its direction/velocity
        float life; //!< The time until it dies in seconds
    } *particles; //!< The particles of this ParticlesObject
    GLuint particlesVBO[2];
    bool activeVBO;
    unsigned int particlesCount;
    float addParticles;
    public:
    btVector3 posMin, //!< The negative position vector relative to the system for spawning new particles
              posMax, //!< The positive position vector relative to the system for spawning new particles
              force, //!< A force to be applied to all particles every frame like gravity
              dirMin, //!< The negative velocity vector for spawning new particles
              dirMax; //!< The positive velocity vector for spawning new particles
    float lifeMin, //!< The minimal life time of a new particle in seconds
          lifeMax, //!< The maximal life time of a new particle in seconds
          systemLife; //!< The life time of the entire system in seconds until it is deleted or -1.0 if it is infinite
    unsigned int maxParticles; //!< The maximal amount of simultaneously existing particles in the system
    std::shared_ptr<Texture> texture; //!< The texture of the particles
    /*! Constructs a new particle system
     @param maxParticles The maximal amount of simultaneously existing particles in the system
     */
    ParticlesObject(unsigned int maxParticles);
    ~ParticlesObject();
    void remove();
    void setTransformation(const btTransform& transformation);
    btTransform getTransformation();
    void gameTick();
    void draw();
};

#endif
