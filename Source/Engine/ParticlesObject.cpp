//
//  ParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

ParticlesObject::ParticlesObject(unsigned int maxParticlesB) {
    texture = NULL;
    maxParticles = maxParticlesB;
    activeVBO = 0;
    addParticles = 0.0;
    systemLife = -1.0;
    force = btVector3(0, -0.981, 0);
    if(particleCalcTarget == 1) {
        particles = new Particle[maxParticles];
        particlesCount = 0;
    }else if(particleCalcTarget == 2) {
        particlesCount = maxParticles;
        glGenBuffers(1, &particlesVBO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, 7*sizeof(float)*maxParticles, 0, GL_STATIC_DRAW);
        glGenBuffers(1, &particlesVBO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, 7*sizeof(float)*maxParticles, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
    objectManager.particlesObjects.insert(this);
    body = new btCollisionObject();
    body->setCollisionShape(new btSphereShape(3.0));
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    worldManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
}

ParticlesObject::~ParticlesObject() {
    if(body) {
        delete body->getCollisionShape();
        delete body;
    }
    if(particleCalcTarget == 1)
        delete [] (Particle*)particles;
    if(particlesVBO[0]) {
        glDeleteBuffers(1, &particlesVBO[0]);
        glDeleteBuffers(1, &particlesVBO[1]);
    }
}

void ParticlesObject::remove() {
    objectManager.particlesObjects.erase(this);
    delete this;
}

void ParticlesObject::setTransformation(const btTransform& transformation) {
    body->setWorldTransform(transformation);
}

btTransform ParticlesObject::getTransformation() {
    return body->getWorldTransform();
}

void ParticlesObject::gameTick() {
    btVector3 position = getTransformation().getOrigin();
    
    if(particleCalcTarget == 0) systemLife = 0.0;
    if(systemLife > -1.0) {
        systemLife -= worldManager.animationFactor;
        if(systemLife <= 0.0)
            return;
    }
    
    if(particleCalcTarget == 1 && (systemLife == -1.0 || systemLife > lifeMax))
        for(; particlesCount < maxParticles; particlesCount ++) {
            Particle* particle = &particles[particlesCount];
            particle->pos = vec3rand(posMin, posMax)+position;
            particle->dir = vec3rand(dirMin, dirMax);
            particle->life = frand(lifeMin, lifeMax);
        }
    
    if(particleCalcTarget == 1) {
        btVector3 forceAux = force*worldManager.animationFactor;
        for(unsigned int p = 0; p < particlesCount; p ++) {
            particles[p].life -= worldManager.animationFactor;
            if(particles[p].life <= 0.0) {
                memcpy(&particles[p], &particles[p+1], sizeof(Particle)*(particlesCount-p-1));
                particlesCount --;
                p --;
                continue;
            }
            particles[p].pos += particles[p].dir*worldManager.animationFactor;
            particles[p].dir += forceAux;
        }
    }else if(particleCalcTarget == 2) {
        shaderPrograms[particleCalculateSP]->use();
        currentShaderProgram->setUniformF("respawnParticles", (systemLife == -1.0 || systemLife > lifeMax) ? 1.0 : 0.0);
        currentShaderProgram->setUniformF("animationFactor", worldManager.animationFactor);
        currentShaderProgram->setUniformF("lifeMin", lifeMin);
        currentShaderProgram->setUniformF("lifeRange", lifeMax-lifeMin);
        currentShaderProgram->setUniformVec3("posMin", posMin+position);
        currentShaderProgram->setUniformVec3("posRange", posMax-posMin);
        currentShaderProgram->setUniformVec3("dirMin", dirMin);
        currentShaderProgram->setUniformVec3("dirRange", dirMax-dirMin);
        currentShaderProgram->setUniformVec3("velocityAdd", force*worldManager.animationFactor);
        
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[activeVBO]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 7*sizeof(float), 0);
        currentShaderProgram->setAttribute(VELOCITY_ATTRIBUTE, 3, 7*sizeof(float), (GLfloat*)(4*sizeof(float)));
        
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, particlesVBO[!activeVBO]);
        glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, particlesVBO[!activeVBO]);
        glBeginTransformFeedbackEXT(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, particlesCount);
        glEndTransformFeedbackEXT();
        activeVBO = !activeVBO;
    }
    
    return;
}

void ParticlesObject::draw() {
    modelMat = getTransformation();
    if(texture) texture->use(GL_TEXTURE_2D, 0);
    currentShaderProgram->setUniformF("lifeMin", lifeMin);
    
    if(particleCalcTarget == 1) {
        unsigned int index;
        float* vertices = new float[4*particlesCount];
        for(unsigned int p = 0; p < particlesCount; p ++) {
            index = p*4;
            vertices[index] = particles[p].pos.x();
            vertices[index+1] = particles[p].pos.y();
            vertices[index+2] = particles[p].pos.z();
            vertices[index+3] = particles[p].life;
        }
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 4*sizeof(float), vertices);
    }else if(particleCalcTarget == 2) {
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[activeVBO]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 7*sizeof(float), 0);
    }
    
    glDrawArrays(GL_POINTS, 0, particlesCount);
}