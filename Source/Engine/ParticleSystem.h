//
//  ParticleSystem.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ShaderProgram.h"

#ifndef ParticleSystem_h
#define ParticleSystem_h

struct Particle {
    Vector3 pos, dir;
    float life, size;
};

class ParticleSystem {
    unsigned int vertexCount;
    float addParticles, *vertices;
    void setParticleVertex(float* verticesB, unsigned int p, Vector3 corner);
    public:
    std::vector<Particle> particles;
    Vector3 pos, posMin, posMax, force, dirMin, dirMax;
    float lifeMin, lifeMax, sizeMin, sizeMax, addMin, addMax, systemLife;
    Texture* texture;
    ParticleSystem();
    ~ParticleSystem();
    void calculate(float animation);
    void draw();
};

class ParticleSystemManager {
    public:
    SDL_mutex* mutex;
    std::vector<ParticleSystem*> particleSystems;
    ParticleSystemManager();
    ~ParticleSystemManager();
    void calculate(float animation);
    void draw();
};

extern ParticleSystemManager particleSystemManager;

#endif
