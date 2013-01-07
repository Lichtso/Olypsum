//
//  ParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LevelManager.h"

static bool readBoundsNode(rapidxml::xml_node<xmlUsedCharType>* node, const char* name, btVector3& min, btVector3& max) {
    XMLValueArray<float> vecData;
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node(name);
    if(!parameterNode) {
        log(error_log, std::string("Tried to construct ParticlesObject without \"")+name+"\"-node.");
        return false;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("min");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"min\"-attribute.");
        return false;
    }
    vecData.readString(attribute->value(), "%f");
    min = vecData.getVector3();
    attribute = parameterNode->first_attribute("max");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"max\"-attribute.");
        return false;
    }
    vecData.readString(attribute->value(), "%f");
    max = vecData.getVector3();
    return true;
}

static bool readRangeNode(rapidxml::xml_node<xmlUsedCharType>* node, const char* name, float& min, float& max) {
    XMLValueArray<float> vecData;
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node(name);
    if(!parameterNode) {
        log(error_log, std::string("Tried to construct ParticlesObject without \"")+name+"\"-node.");
        return false;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("min");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"min\"-attribute.");
        return false;
    }
    sscanf(attribute->value(), "%f", &min);
    attribute = parameterNode->first_attribute("max");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"max\"-attribute.");
        return false;
    }
    sscanf(attribute->value(), "%f", &max);
    return true;
}

template<class T>
rapidxml::xml_node<xmlUsedCharType>* writeBoundsNode(rapidxml::xml_document<xmlUsedCharType>& doc, const char* name, T min, T max) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name(name);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("min");
    attribute->value(doc.allocate_string(stringOf(min).c_str()));
    node->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("max");
    attribute->value(doc.allocate_string(stringOf(max).c_str()));
    node->append_attribute(attribute);
    return node;
}

ParticlesObject::ParticlesObject() :activeVBO(0), addParticles(0.0), systemLife(-1.0), force(btVector3(0, -9.81, 0)), transformAligned(false) {
    objectManager.particlesObjects.insert(this);
    body = new btCollisionObject();
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void ParticlesObject::init() {
    if(particleCalcTarget == 1) {
        particles = new Particle[maxParticles];
        particlesCount = 0;
    }else if(particleCalcTarget == 2) {
        particlesCount = maxParticles;
        glGenBuffers(1, &particlesVBO[0]);
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[0]);
        glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*maxParticles, 0, GL_STATIC_DRAW);
        glGenBuffers(1, &particlesVBO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*maxParticles, 0, GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }
}

ParticlesObject::ParticlesObject(unsigned int maxParticlesB, btCollisionShape* collisionShape) :ParticlesObject() {
    //texture = NULL;
    maxParticles = maxParticlesB;
    init();
    body->setCollisionShape(collisionShape);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
}

ParticlesObject::ParticlesObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :ParticlesObject() {
    levelLoader->pushObject(this);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("particles");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"particles\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%d", &maxParticles);
    init();
    
    attribute = node->first_attribute("force");
    if(!attribute) {
        log(error_log, "Tried to construct ParticlesObject without \"force\"-attribute.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(attribute->value(), "%f");
    force = vecData.getVector3();
    
    attribute = node->first_attribute("transformAligned");
    if(attribute)
        if(strcmp(attribute->value(), "true") == 0)
            transformAligned = true;
    
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setCollisionShape(PhysicObject::readCollisionShape(node->first_node("PhysicsBody"), levelLoader));
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
    if(!readRangeNode(node, "Life", lifeMin, lifeMax)) return;
    if(!readRangeNode(node, "Size", sizeMin, sizeMax)) return;
    if(!readBoundsNode(node, "SpawnBox", posMin, posMax)) return;
    if(!readBoundsNode(node, "Velocity", dirMin, dirMax)) return;
    
    texture = fileManager.initResource<Texture>(node->first_node("Texture"));
    texture->uploadTexture(GL_TEXTURE_2D_ARRAY_EXT, GL_COMPRESSED_RGB);
}

ParticlesObject::~ParticlesObject() {
    if(body) {
        delete body;
        body = NULL;
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
    BaseObject::remove();
}

void ParticlesObject::setTransformation(const btTransform& transformation) {
    body->setWorldTransform(transformation);
}

btTransform ParticlesObject::getTransformation() {
    return body->getWorldTransform();
}

bool ParticlesObject::gameTick() {
    btVector3 position = getTransformation().getOrigin();
    
    if(particleCalcTarget == 0) systemLife = 0.0;
    if(systemLife > -1.0) {
        systemLife -= animationFactor;
        if(systemLife <= 0.0) {
            delete this;
            return false;
        }
    }
    
    if(particleCalcTarget == 1) {
        if(systemLife == -1.0 || systemLife > lifeMax)
            for(; particlesCount < maxParticles; particlesCount ++) {
                Particle* particle = &particles[particlesCount];
                particle->pos = vec3rand(posMin, posMax)+position;
                particle->dir = vec3rand(dirMin, dirMax).normalize()*(dirMax-dirMin).length()*0.5;
                particle->life = frand(lifeMin, lifeMax);
                particle->size = frand(sizeMin, sizeMax);
            }
        
        btVector3 forceAux = force*animationFactor;
        for(unsigned int p = 0; p < particlesCount; p ++) {
            particles[p].life -= animationFactor;
            if(particles[p].life <= 0.0) {
                memcpy(&particles[p], &particles[p+1], sizeof(Particle)*(particlesCount-p-1));
                particlesCount --;
                p --;
                continue;
            }
            particles[p].pos += particles[p].dir*animationFactor;
            particles[p].dir += forceAux;
        }
    }else if(particleCalcTarget == 2) {
        shaderPrograms[particleCalculateSP]->use();
        currentShaderProgram->setUniformF("respawnParticles", (systemLife == -1.0 || systemLife > lifeMax) ? 1.0 : 0.0);
        currentShaderProgram->setUniformF("animationFactor", animationFactor);
        currentShaderProgram->setUniformF("lifeCenter", (lifeMax+lifeMin)*0.5);
        currentShaderProgram->setUniformF("lifeRange", lifeMax-lifeMin);
        currentShaderProgram->setUniformF("sizeCenter", (sizeMax+sizeMin)*0.5);
        currentShaderProgram->setUniformF("sizeRange", sizeMax-sizeMin);
        currentShaderProgram->setUniformVec3("posCenter", (posMax+posMin)*0.5+position);
        currentShaderProgram->setUniformVec3("posRange", posMax-posMin);
        currentShaderProgram->setUniformVec3("dirCenter", (dirMax+dirMin)*0.5);
        currentShaderProgram->setUniformVec3("dirRange", dirMax-dirMin);
        currentShaderProgram->setUniformVec3("velocityAdd", force*animationFactor);
        
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[activeVBO]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 8*sizeof(float), 0);
        currentShaderProgram->setAttribute(VELOCITY_ATTRIBUTE, 4, 8*sizeof(float), reinterpret_cast<float*>(4*sizeof(float)));
        
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, particlesVBO[!activeVBO]);
        glBindBufferBaseEXT(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0, particlesVBO[!activeVBO]);
        glBeginTransformFeedbackEXT(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, particlesCount);
        glEndTransformFeedbackEXT();
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0);
        glDisableVertexAttribArray(VELOCITY_ATTRIBUTE);
        glDisableVertexAttribArray(POSITION_ATTRIBUTE);
        activeVBO = !activeVBO;
    }
    
    return true;
}

void ParticlesObject::draw() {
    modelMat = getTransformation();
    if(texture->depth > 1)
        shaderPrograms[particleDrawAnimatedSP]->use();
    else
        shaderPrograms[particleDrawSP]->use();
    texture->use(0);
    
    currentShaderProgram->setUniformF("lifeInvMin", 5.0/lifeMin);
    currentShaderProgram->setUniformF("lifeInvMax", (float)texture->depth/lifeMax);
    if(transformAligned) {
        btMatrix3x3 viewNormalMat = getTransformation().getBasis();
        currentShaderProgram->setUniformMatrix3("viewNormalMat", &viewNormalMat);
    }
    
    if(particleCalcTarget == 1) {
        unsigned int index;
        float* vertices = new float[5*particlesCount];
        for(unsigned int p = 0; p < particlesCount; p ++) {
            index = p*5;
            vertices[index] = particles[p].pos.x();
            vertices[index+1] = particles[p].pos.y();
            vertices[index+2] = particles[p].pos.z();
            vertices[index+3] = particles[p].life;
            vertices[index+4] = particles[p].size;
        }
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 5*sizeof(float), vertices);
        currentShaderProgram->setAttribute(VELOCITY_ATTRIBUTE, 1, 5*sizeof(float), &vertices[4]);
    }else if(particleCalcTarget == 2) {
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[activeVBO]);
        currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 4, 8*sizeof(float), 0);
        currentShaderProgram->setAttribute(VELOCITY_ATTRIBUTE, 1, 8*sizeof(float), reinterpret_cast<float*>(7*sizeof(float)));
    }
    
    glDrawArrays(GL_POINTS, 0, particlesCount);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(VELOCITY_ATTRIBUTE);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
}

rapidxml::xml_node<xmlUsedCharType>* ParticlesObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    node->name("ParticlesObject");
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("particles");
    attribute->value(doc.allocate_string(stringOf((int)maxParticles).c_str()));
    node->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("force");
    attribute->value(doc.allocate_string(stringOf(force).c_str()));
    node->append_attribute(attribute);
    if(transformAligned) {
        attribute = doc.allocate_attribute();
        attribute->name("transformAligned");
        attribute->value("true");
        node->append_attribute(attribute);
    }
    
    node->append_node(writeBoundsNode(doc, "Life", lifeMin, lifeMax));
    node->append_node(writeBoundsNode(doc, "Size", sizeMin, sizeMax));
    node->append_node(writeBoundsNode(doc, "SpawnBox", posMin, posMax));
    node->append_node(writeBoundsNode(doc, "Velocity", dirMin, dirMax));
    node->append_node(fileManager.writeResource(doc, "Texture", texture));
    
    return node;
}