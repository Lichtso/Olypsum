//
//  ParticleSystem.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Audio.h"

#ifndef ParticleSystem_h
#define ParticleSystem_h

struct Particle {
    Vector3 pos, dir;
    float life;
};

class ParticleSystem {
    Particle* particles;
    GLuint particlesVBO[2];
    bool activeVBO;
    unsigned int particlesCount;
    float addParticles;
    public:
    PositionalLight* lightSource;
    Vector3 position, posMin, posMax, force, dirMin, dirMax;
    float lifeMin, lifeMax, addMin, addMax, systemLife;
    unsigned int maxParticles;
    Texture* texture;
    ParticleSystem(unsigned int maxParticles);
    ~ParticleSystem();
    bool calculate();
    void draw();
};

class ParticleSystemManager {
    public:
    std::vector<ParticleSystem*> particleSystems;
    ParticleSystemManager();
    ~ParticleSystemManager();
    void calculate();
    void draw();
};

extern ParticleSystemManager particleSystemManager;

#endif
