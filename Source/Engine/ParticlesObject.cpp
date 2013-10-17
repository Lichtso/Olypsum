//
//  ParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptParticlesObject.h"

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

ParticlesObject::ParticlesObject() :activeVAO(0), systemLife(-1.0), force(btVector3(0, -9.81, 0)), transformAligned(false) {
    objectManager.particlesObjects.insert(this);
    body = new btCollisionObject();
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
}

void ParticlesObject::init() {
    if(optionsState.particleCalcTarget == 0) return;
    GLenum target = (optionsState.particleCalcTarget == 1) ? GL_DYNAMIC_DRAW : GL_STATIC_COPY;
    
    glGenVertexArrays(optionsState.particleCalcTarget, &particlesVAO[0]);
    glGenBuffers(optionsState.particleCalcTarget, &particlesVBO[0]);
    
    glBindVertexArray(particlesVAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[0]);
    glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*maxParticles, 0, target);
    glEnableVertexAttribArray(POSITION_ATTRIBUTE);
    glEnableVertexAttribArray(VELOCITY_ATTRIBUTE);
    glVertexAttribPointer(POSITION_ATTRIBUTE, 4, GL_FLOAT, false, 8*sizeof(float), 0);
    glVertexAttribPointer(VELOCITY_ATTRIBUTE, 4, GL_FLOAT, false, 8*sizeof(float), reinterpret_cast<float*>(4*sizeof(float)));
    glBindVertexArray(0);
    
    if(optionsState.particleCalcTarget == 2) {
        glBindVertexArray(particlesVAO[1]);
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[1]);
        glBufferData(GL_ARRAY_BUFFER, 8*sizeof(float)*maxParticles, 0, target);
        glEnableVertexAttribArray(POSITION_ATTRIBUTE);
        glEnableVertexAttribArray(VELOCITY_ATTRIBUTE);
        glVertexAttribPointer(POSITION_ATTRIBUTE, 4, GL_FLOAT, false, 8*sizeof(float), 0);
        glVertexAttribPointer(VELOCITY_ATTRIBUTE, 4, GL_FLOAT, false, 8*sizeof(float), reinterpret_cast<float*>(4*sizeof(float)));
        glBindVertexArray(0);
    }else{
        particles = new Particle[maxParticles];
        for(unsigned int p = 0; p < maxParticles; p ++)
            particles[p].life = -1.0;
    }
    
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptParticlesObject.functionTemplate->GetFunction()->NewInstance(1, &external);
}

ParticlesObject::ParticlesObject(unsigned int _maxParticles, btCollisionShape* collisionShape) :ParticlesObject() {
    maxParticles = _maxParticles;
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
    
    attribute = node->first_attribute("systemLife");
    if(attribute)
        sscanf(attribute->value(), "%f", &systemLife);
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Force");
    if(parameterNode) {
        XMLValueArray<float> vecData;
        vecData.readString(parameterNode->value(), "%f");
        force = vecData.getVector3();
    }else
        force = btVector3(0.0, 0.0, 0.0);
    
    transformAligned = node->first_node("TransformAligned");
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setCollisionShape(PhysicObject::readCollisionShape(node->first_node("PhysicsBody"), levelLoader));
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
    if(!readRangeNode(node, "Life", lifeMin, lifeMax)) return;
    if(!readRangeNode(node, "Size", sizeMin, sizeMax)) return;
    if(!readBoundsNode(node, "SpawnBox", posMin, posMax)) return;
    if(!readBoundsNode(node, "Velocity", dirMin, dirMax)) return;
    
    parameterNode = node->first_node("Texture");
    if(!parameterNode) {
        log(error_log, "Tried to construct ParticlesObject without \"Texture\"-node.");
        return;
    }
    attribute = parameterNode->first_attribute("src");
    if(!attribute) {
        log(error_log, "Found \"Texture\"-node without \"src\"-attribute.");
        return;
    }
    texture = fileManager.getResourceByPath<Texture>(levelLoader->filePackage, attribute->value());
    texture->uploadTexture(GL_TEXTURE_2D_ARRAY, GL_COMPRESSED_RGBA);
}

void ParticlesObject::clean() {
    if(body) {
        objectManager.physicsWorld->removeCollisionObject(body);
        delete body;
        body = NULL;
    }
    
    if(optionsState.particleCalcTarget == 1) {
        delete [] (Particle*)particles;
        glDeleteVertexArrays(1, &particlesVAO[0]);
        glDeleteBuffers(1, &particlesVBO[0]);
    }else{
        glDeleteVertexArrays(2, &particlesVAO[0]);
        glDeleteBuffers(2, &particlesVBO[0]);
    }
}

void ParticlesObject::removeClean() {
    objectManager.particlesObjects.erase(this);
    clean();
    VisualObject::removeClean();
}

void ParticlesObject::removeFast() {
    clean();
    VisualObject::removeFast();
}

void ParticlesObject::setTransformation(const btTransform& transformation) {
    body->setWorldTransform(transformation);
}

btTransform ParticlesObject::getTransformation() {
    return body->getWorldTransform();
}

bool ParticlesObject::gameTick() {
    btTransform transform = getTransformation();
    
    if(optionsState.particleCalcTarget == 0) systemLife = 0.0;
    if(systemLife > -1.0) {
        systemLife -= profiler.animationFactor;
        if(systemLife <= 0.0) {
            removeClean();
            return false;
        }
    }
    
    if(optionsState.particleCalcTarget == 1) {
        bool respawnParticles = systemLife == -1.0 || systemLife > lifeMax;
        btVector3 forceAux = force*profiler.animationFactor;
        for(unsigned int p = 0; p < maxParticles; p ++) {
            Particle& particle = particles[p];
            if(particles[p].life > 0.0) {
                particle.pos += particle.dir*profiler.animationFactor;
                particle.dir += forceAux;
                particle.life -= profiler.animationFactor;
            }else if(respawnParticles) {
                particle.pos = transform(vec3rand(posMin, posMax));
                particle.dir = transform(vec3rand(dirMin, dirMax).normalize()*(dirMax-dirMin).length()*0.5);
                particle.life = frand(lifeMin, lifeMax);
                particle.size = frand(sizeMin, sizeMax);
            }
        }
    }else if(optionsState.particleCalcTarget == 2) {
        shaderPrograms[advanceParticlesSP]->use();
        currentShaderProgram->setUniformMatrix4("modelMat", &transform);
        currentShaderProgram->setUniformMatrix3("normalMat", &transform.getBasis());
        currentShaderProgram->setUniformF("respawnParticles", (systemLife == -1.0 || systemLife > lifeMax) ? 1.0 : 0.0);
        currentShaderProgram->setUniformF("animationFactor", profiler.animationFactor);
        currentShaderProgram->setUniformF("lifeCenter", (lifeMax+lifeMin)*0.5);
        currentShaderProgram->setUniformF("lifeRange", lifeMax-lifeMin);
        currentShaderProgram->setUniformF("sizeCenter", (sizeMax+sizeMin)*0.5);
        currentShaderProgram->setUniformF("sizeRange", sizeMax-sizeMin);
        currentShaderProgram->setUniformVec3("posCenter", (posMax+posMin)*0.5);
        currentShaderProgram->setUniformVec3("posRange", posMax-posMin);
        currentShaderProgram->setUniformVec3("dirCenter", (dirMax+dirMin)*0.5);
        currentShaderProgram->setUniformVec3("dirRange", dirMax-dirMin);
        currentShaderProgram->setUniformVec3("velocityAdd", force*profiler.animationFactor);
        
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, particlesVBO[!activeVAO]);
        glBindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, particlesVBO[!activeVAO]);
        glBindVertexArray(particlesVAO[activeVAO]);
        glBeginTransformFeedback(GL_POINTS);
        glDrawArrays(GL_POINTS, 0, maxParticles);
        glEndTransformFeedback();
        glBindVertexArray(0);
        glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER, 0);
        activeVAO = !activeVAO;
    }
    
    return VisualObject::gameTick();
}

void ParticlesObject::draw() {
    if(texture->depth > 1)
        shaderPrograms[particlesAnimatedSP]->use();
    else
        shaderPrograms[particlesSP]->use();
    texture->use(0);
    
    currentShaderProgram->setUniformF("textureZScale", (float)texture->depth/lifeMax);
    currentShaderProgram->setUniformF("textureAScale", 5.0/lifeMin);
    if(transformAligned) {
        btMatrix3x3 viewNormalMat = getTransformation().getBasis();
        currentShaderProgram->setUniformMatrix3("viewNormalMat", &viewNormalMat);
        glDisable(GL_CULL_FACE);
    }
    
    glBindVertexArray(particlesVAO[activeVAO]);
    if(optionsState.particleCalcTarget == 1) {
        unsigned int index;
        float* vertices = new float[maxParticles*8];
        for(unsigned int p = 0; p < maxParticles; p ++) {
            index = p*8;
            vertices[index  ] = particles[p].pos.x();
            vertices[index+1] = particles[p].pos.y();
            vertices[index+2] = particles[p].pos.z();
            vertices[index+3] = particles[p].life;
            vertices[index+4] = particles[p].dir.x();
            vertices[index+5] = particles[p].dir.y();
            vertices[index+6] = particles[p].dir.z();
            vertices[index+7] = particles[p].size;
        }
        glBindBuffer(GL_ARRAY_BUFFER, particlesVBO[0]);
        glBufferSubData(GL_ARRAY_BUFFER, 0, maxParticles*8*sizeof(float), vertices);
    }
    
    glDrawArrays(GL_POINTS, 0, maxParticles);
    glBindVertexArray(0);
    glEnable(GL_CULL_FACE);
}

rapidxml::xml_node<xmlUsedCharType>* ParticlesObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    node->name("ParticlesObject");
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("particles");
    attribute->value(doc.allocate_string(stringOf((int)maxParticles).c_str()));
    node->append_attribute(attribute);
    
    if(systemLife > -1.0) {
        attribute = doc.allocate_attribute();
        attribute->name("systemLife");
        attribute->value(doc.allocate_string(stringOf(systemLife).c_str()));
        node->append_attribute(attribute);
    }
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Force");
    parameterNode->value(doc.allocate_string(stringOf(force).c_str()));
    node->append_node(parameterNode);
    if(transformAligned) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("TransformAligned");
        node->append_node(parameterNode);
    }
    
    node->append_node(writeBoundsNode(doc, "Life", lifeMin, lifeMax));
    node->append_node(writeBoundsNode(doc, "Size", sizeMin, sizeMax));
    node->append_node(writeBoundsNode(doc, "SpawnBox", posMin, posMax));
    node->append_node(writeBoundsNode(doc, "Velocity", dirMin, dirMax));
    node->append_node(fileManager.writeResource(doc, "Texture", texture));
    
    return node;
}