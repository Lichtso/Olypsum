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
    float life, size;
};

struct ParticleDrawArray {
    Vector3 position;
    Texture* texture;
    unsigned int vertexCount;
    float* vertices;
};

class ParticleSystem {
    float addParticles;
    void setParticleVertex(float* verticesB, unsigned int p, Vector3 corner);
    public:
    std::vector<Particle> particles;
    Vector3 position, posMin, posMax, force, dirMin, dirMax;
    float lifeMin, lifeMax, sizeMin, sizeMax, addMin, addMax, systemLife;
    Texture* texture;
    ParticleSystem();
    ~ParticleSystem();
    ParticleDrawArray* calculate(float animation);
};

class ParticleSystemManager {
    SDL_mutex* mutex;
    std::vector<ParticleDrawArray*>* particleDrawArrays;
    void cleanParticleDrawArrays();
    public:
    std::vector<ParticleSystem*> particleSystems;
    ParticleSystemManager();
    ~ParticleSystemManager();
    void calculate(float animation);
    void draw();
};

extern ParticleSystemManager particleSystemManager;

#endif
