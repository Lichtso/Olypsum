//
//  ParticleSystem.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ParticleSystem.h"
#import "FileManager.h"


ParticleSystem::ParticleSystem() {
    vertices = NULL;
    texture = NULL;
    vertexCount = 0;
    addParticles = 0.0;
    systemLife = -1.0;
    force = Vector3(0, -0.981, 0);
    particleSystemManager.particleSystems.push_back(this);
}

ParticleSystem::~ParticleSystem() {
    if(vertices) delete [] vertices;
    if(texture) fileManager.releaseTexture(texture);
    for(unsigned int p = 0; p < particleSystemManager.particleSystems.size(); p ++)
        if(particleSystemManager.particleSystems[p] == this) {
            particleSystemManager.particleSystems.erase(particleSystemManager.particleSystems.begin()+p);
            return;
        }
}

void ParticleSystem::setParticleVertex(float* verticesB, unsigned int p, Vector3 corner) {
    float size = particles[p].size;
    Vector3 pos = particles[p].pos;
    verticesB[0] = pos.x;
    verticesB[1] = pos.y;
    verticesB[2] = pos.z;
    verticesB[3] = 0.5+corner.x*0.5;
    verticesB[4] = 0.5+corner.y*0.5;
    verticesB[5] = 1.0-fmin(particles[p].life/lifeMin*5.0, 1.0);
    verticesB[6] = corner.x*size;
    verticesB[7] = corner.y*size;
}

bool ParticleSystem::calculate(float animation) {
    if(systemLife > -1.0) {
        systemLife -= animation;
        if(systemLife <= 0.0) {
            delete this;
            return true;
        }
    }
    
    if(systemLife == -1.0 || systemLife > lifeMax) {
        Particle particle;
        for(addParticles += animation*frand(addMin, addMax); addParticles >= 1.0; addParticles --) {
            particle.pos = vec3rand(posMin, posMax)+position;
            particle.dir = vec3rand(dirMin, dirMax);
            particle.life = frand(lifeMin, lifeMax);
            particle.size = frand(sizeMin, sizeMax);
            particles.push_back(particle);
        }
    }
    
    unsigned int index;
    Vector3 forceAux = force*animation;
    float* verticesB = new float[48*particles.size()];
    for(unsigned int p = 0; p < particles.size(); p ++) {
        particles[p].life -= animation;
        if(particles[p].life <= 0.0) {
            particles.erase(particles.begin()+p);
            p --;
            continue;
        }
        particles[p].dir += forceAux;
        particles[p].pos += particles[p].dir*animation;
        index = p*48;
        setParticleVertex(&verticesB[index   ], p, Vector3(-1.0, -1.0, 0.0));
        setParticleVertex(&verticesB[index+8 ], p, Vector3( 1.0, -1.0, 0.0));
        setParticleVertex(&verticesB[index+16], p, Vector3(-1.0,  1.0, 0.0));
        setParticleVertex(&verticesB[index+24], p, Vector3( 1.0, -1.0, 0.0));
        setParticleVertex(&verticesB[index+32], p, Vector3( 1.0,  1.0, 0.0));
        setParticleVertex(&verticesB[index+40], p, Vector3(-1.0,  1.0, 0.0));
    }
    
    SDL_mutexP(particleSystemManager.mutex);
    if(vertices) delete [] vertices;
    vertices = verticesB;
    vertexCount = 6*particles.size();
    SDL_mutexV(particleSystemManager.mutex);
    return false;
}

void ParticleSystem::draw() {
    if(renderingState != RenderingScreen) return;
    if(texture) texture->use(0);
    lightManager.setLights(position);
    SDL_mutexP(particleSystemManager.mutex);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 8*sizeof(float), vertices);
    currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 3, 8*sizeof(float), &vertices[3]);
    currentShaderProgram->setAttribute(TANGENT_ATTRIBUTE, 2, 8*sizeof(float), &vertices[6]);
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    SDL_mutexV(particleSystemManager.mutex);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(TANGENT_ATTRIBUTE);
}

ParticleSystemManager::ParticleSystemManager() {
    mutex = SDL_CreateMutex();
}

ParticleSystemManager::~ParticleSystemManager() {
    SDL_DestroyMutex(mutex);
    for(unsigned int p = 0; p < particleSystems.size(); p ++)
        delete particleSystems[p];
}

void ParticleSystemManager::calculate(float animation) {
    for(unsigned int p = 0; p < particleSystems.size(); p ++)
        if(particleSystems[p]->calculate(animation))
            p --;
}

void ParticleSystemManager::draw() {
    modelMat.setIdentity();
    spriteShaderProgram->use();
    spriteShaderProgram->setUniformVec3("color", Vector3(-1, -1, -1));
    glDepthMask(0);
    for(unsigned int p = 0; p < particleSystems.size(); p ++)
        particleSystems[p]->draw();
    glDepthMask(1);
}

ParticleSystemManager particleSystemManager;