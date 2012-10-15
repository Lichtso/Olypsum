//
//  ParticleSystem.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Texture.h"

#ifndef ParticleSystem_h
#define ParticleSystem_h

struct Particle {
    Vector3 pos, dir;
    float life;
};

class PositionalLight;

class ParticleSystem {
    Particle* particles;
    GLuint particlesVBO[2];
    bool activeVBO;
    unsigned int particlesCount;
    float addParticles;
    public:
    PositionalLight* lightSource;
    Vector3 position, posMin, posMax, force, dirMin, dirMax;
    float lifeMin, lifeMax, systemLife;
    unsigned int maxParticles;
    std::shared_ptr<Texture> texture;
    ParticleSystem(unsigned int maxParticles);
    ~ParticleSystem();
    bool gameTick();
    void draw();
};

class ParticleSystemManager {
    public:
    std::vector<ParticleSystem*> particleSystems;
    ~ParticleSystemManager();
    void clear();
    void gameTick();
    void draw();
};

extern ParticleSystemManager particleSystemManager;

#endif
