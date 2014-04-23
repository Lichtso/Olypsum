//
//  VisualObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "Scripting/ScriptManager.h"

MatterObject::MatterObject() :integrity(1.0) {
    objectManager.matterObjects.insert(this);
}

void MatterObject::removeClean() {
    for(auto decal : decals)
        delete decal;
    
    decals.clear();
    objectManager.matterObjects.erase(this);
    
    VisualObject::removeClean();
}

void MatterObject::removeFast() {
    for(auto decal : decals)
        delete decal;
    
    VisualObject::removeFast();
}

bool MatterObject::gameTick() {
    if(integrity <= 0.0) {
        integrity -= profiler.animationFactor;
        if(integrity < -1.0) {
            removeClean();
            return false;
        }
    }
    
    foreach_e(decals, iterator) {
        (*iterator)->life -= profiler.animationFactor;
        if((*iterator)->life > 0.0) continue;
        delete *iterator;
        decals.erase(iterator);
    }
    
    return VisualObject::gameTick();
}

void MatterObject::draw() {
    if(objectManager.currentShadowLight) return;
    
    btTransform transform = getTransformation();
    Texture::unbind(1, GL_TEXTURE_2D);
    
    for(auto decal : decals) {
        modelMat = transform * decal->transformation;
        decal->diffuse->use(0);
        
        if(decal->heightMap) {
            decal->heightMap->use(2);
            shaderPrograms[solidBumpGSP]->use();
        }else{
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            shaderPrograms[solidGSP]->use();
        }
        
        currentShaderProgram->setUniformF("discardDensity", min(1.0F, decal->life));
        rectVAO.draw();
    }
}

void MatterObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    rapidxml::xml_node<xmlUsedCharType>* parameterNode;
    
    if((parameterNode = node->first_node("Integrity")))
        sscanf(parameterNode->value(), "%f", &integrity);
}

rapidxml::xml_node<xmlUsedCharType>* MatterObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    if(integrity != 1.0) {
        rapidxml::xml_node<xmlUsedCharType>* integrityNode = doc.allocate_node(rapidxml::node_element);
        integrityNode->name("Integrity");
        integrityNode->value(doc.allocate_string(stringOf(integrity).c_str()));
        node->append_node(integrityNode);
    }
    return node;
}



SoftObject::SoftObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    //btSoftBodyHelpers
}

void SoftObject::removeClean() {
    if(body) {
        btSoftBody* softBody = getBody();
        objectManager.physicsWorld->removeSoftBody(softBody);
        delete softBody;
        body = NULL;
    }
    MatterObject::removeClean();
}

void SoftObject::removeFast() {
    if(body) {
        btSoftBody* softBody = getBody();
        delete softBody;
        body = NULL;
    }
    MatterObject::removeFast();
}



void simpleMotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const {
    centerOfMassWorldTrans = transformation;
}

void simpleMotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans) {
    transformation = centerOfMassWorldTrans;
}



void comMotionState::getWorldTransform(btTransform& centerOfMassWorldTrans) const {
    centerOfMassWorldTrans = centerOfMassOffset.inverse() * transformation;
}

void comMotionState::setWorldTransform(const btTransform& centerOfMassWorldTrans) {
    transformation = centerOfMassWorldTrans * centerOfMassOffset;
}



RigidObject::RigidObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    MatterObject::init(node, levelLoader);
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("PhysicsBody");
    if(!parameterNode) {
        log(error_log, "Tried to construct RigidObject without \"PhysicsBody\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("mass");
    if(!attribute) {
        log(error_log, "Tried to construct RigidObject without \"mass\"-attribute.");
        return;
    }
    float mass = 1.0;
    sscanf(attribute->value(), "%f", &mass);
    btCollisionShape* collisionShape = PhysicObject::readCollisionShape(parameterNode);
    if(!collisionShape) return;
    btVector3 localInertia;
    collisionShape->calculateLocalInertia(mass, localInertia);
    btTransform transform = BaseObject::readTransformtion(node, levelLoader);
    body = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(mass, new simpleMotionState(transform), collisionShape, localInertia));
    body->setUserPointer(this);
    btRigidBody* body = getBody();
    body->setSleepingThresholds(0.5, 0.5);
    if(mass == 0.0)
        objectManager.physicsWorld->addRigidBody(body, CollisionMask_Static, CollisionMask_Object);
    else
        objectManager.physicsWorld->addRigidBody(body, CollisionMask_Object, CollisionMask_Zone | CollisionMask_Static | CollisionMask_Object);
    
    XMLValueArray<float> vecData;
    parameterNode = node->first_node("AngularVelocity");
    if(parameterNode) {
        vecData.readString(parameterNode->value(), "%f");
        body->setAngularVelocity(vecData.getVector3());
    }
    parameterNode = node->first_node("LinearVelocity");
    if(parameterNode) {
        vecData.readString(parameterNode->value(), "%f");
        body->setLinearVelocity(vecData.getVector3());
    }
    parameterNode = node->first_node("AngularFactor");
    if(parameterNode) {
        vecData.readString(parameterNode->value(), "%f");
        body->setAngularFactor(vecData.getVector3());
    }
    parameterNode = node->first_node("LinearFactor");
    if(parameterNode) {
        vecData.readString(parameterNode->value(), "%f");
        body->setLinearFactor(vecData.getVector3());
    }
    parameterNode = node->first_node("Damping");
    if(parameterNode) {
        float value;
        attribute = parameterNode->first_attribute("linear");
        if(!attribute) {
            log(error_log, "Tried to construct RigidObject-Damping without \"linear\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &value);
        body->setLinearDamping(value);
        attribute = parameterNode->first_attribute("angular");
        if(!attribute) {
            log(error_log, "Tried to construct RigidObject-Damping without \"angular\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &value);
        body->setAngularDamping(value);
    }
    
    parameterNode = node->first_node("Model");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("src");
        if(!attribute) {
            log(error_log, "Found \"Model\"-node without \"src\"-attribute.");
            return;
        }
        if((parameterNode = node->first_node("TextureAnimationTime"))) {
            XMLValueArray<float> animationTime;
            animationTime.readString(parameterNode->value(), "%f");
            if(animationTime.count != textureAnimationTime.size()) {
                log(error_log, "Tried to construct ModelObject with invalid \"TextureAnimation\"-node.");
                return;
            }
            for(unsigned int i = 0; i < textureAnimationTime.size(); i ++)
                textureAnimationTime[i] = animationTime.data[i];
        }
        setModel(levelLoader, fileManager.getResourceByPath<Model>(levelLoader->filePackage, attribute->value()));
    }
    
    PhysicObject::readFrictionAndRestitution(node);
    ScriptInstance(RigidObject);
}

void RigidObject::removeClean() {
    removeModel();
    if(body) {
        btRigidBody* rigidBody = getBody();
        delete rigidBody->getMotionState();
        objectManager.physicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        body = NULL;
    }
    MatterObject::removeClean();
}

void RigidObject::removeFast() {
    textureAnimationTime.clear();
    skeletonPose.reset();
    if(body) {
        btRigidBody* rigidBody = getBody();
        delete rigidBody->getMotionState();
        delete rigidBody;
        body = NULL;
    }
    MatterObject::removeFast();
}

void RigidObject::setTransformation(const btTransform& transformation) {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    motionState->transformation = transformation;
}

btTransform RigidObject::getTransformation() {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    return motionState->transformation;
}

bool RigidObject::gameTick() {
    for(unsigned int i = 0; i < textureAnimationTime.size(); i ++)
        if(textureAnimationTime[i ++] >= 0.0)
            textureAnimationTime[i ++] += profiler.animationFactor;
    if(skeletonPose) {
        btTransform transform = getTransformation();
        for(unsigned int i = 0; i < model->skeleton->bones.size(); i ++)
            skeletonPose.get()[i] = transform;
    }
    return MatterObject::gameTick();
}

rapidxml::xml_node<xmlUsedCharType>* RigidObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = MatterObject::write(doc, levelSaver);
    node->name("RigidObject");
    btRigidBody* body = getBody();
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode;
    btVector3 vector = body->getAngularVelocity();
    if(vector.length() > 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("AngularVelocity");
        parameterNode->value(doc.allocate_string(stringOf(vector).c_str()));
        node->append_node(parameterNode);
    }
    vector = body->getLinearVelocity();
    if(vector.length() > 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("LinearVelocity");
        parameterNode->value(doc.allocate_string(stringOf(vector).c_str()));
        node->append_node(parameterNode);
    }
    vector = body->getAngularFactor();
    if(vector != btVector3(1, 1, 1)) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("AngularFactor");
        parameterNode->value(doc.allocate_string(stringOf(vector).c_str()));
        node->append_node(parameterNode);
    }
    vector = body->getLinearFactor();
    if(vector != btVector3(1, 1, 1)) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("LinearFactor");
        parameterNode->value(doc.allocate_string(stringOf(vector).c_str()));
        node->append_node(parameterNode);
    }
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("mass");
    if(body->getInvMass() == 0.0)
        attribute->value("0.0");
    else
        attribute->value(doc.allocate_string(stringOf(1.0f/body->getInvMass()).c_str()));
    node->first_node("PhysicsBody")->append_attribute(attribute);
    
    if(body->getLinearDamping() != 0.0 || body->getAngularDamping() != 0.0) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("Damping");
        node->append_node(parameterNode);
        attribute = doc.allocate_attribute();
        attribute->name("linear");
        attribute->value(doc.allocate_string(stringOf(body->getLinearDamping()).c_str()));
        parameterNode->append_attribute(attribute);
        attribute = doc.allocate_attribute();
        attribute->name("angular");
        attribute->value(doc.allocate_string(stringOf(body->getAngularDamping()).c_str()));
        parameterNode->append_attribute(attribute);
    }
    
    if(model) {
        node->append_node(fileManager.writeResource(doc, "Model", model));
        if(textureAnimationTime.size() > 0) {
            rapidxml::xml_node<xmlUsedCharType>* textureAnimationNode = doc.allocate_node(rapidxml::node_element);
            textureAnimationNode->name("TextureAnimationTime");
            std::ostringstream data;
            for(unsigned int i = 0; i < textureAnimationTime.size(); i ++) {
                if(i > 0) data << " ";
                data << textureAnimationTime[i];
            }
            textureAnimationNode->value(doc.allocate_string(data.str().c_str()));
            node->append_node(textureAnimationNode);
        }
    }
    
    return node;
}

bool RigidObject::getKinematic() {
    return body->getCollisionFlags() & btCollisionObject::CF_KINEMATIC_OBJECT;
}

void RigidObject::setKinematic(bool active) {
    if(getBody()->getInvMass() == 0.0) return;
    if(active) {
        body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
        body->setActivationState(DISABLE_DEACTIVATION);
    }else if(getKinematic()) {
        body->setCollisionFlags(body->getCollisionFlags() & (~ btCollisionObject::CF_KINEMATIC_OBJECT));
        body->forceActivationState(ACTIVE_TAG);
    }
}

void RigidObject::removeModel() {
    if(!model) return;
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.reflectivity != 0)
            objectManager.reflectiveAccumulator.erase(this);
    foreach_e(links, iterator)
        if(dynamic_cast<BoneLink*>(*iterator)) {
            (*iterator)->b->removeClean();
            (*iterator)->removeClean();
        }
    textureAnimationTime.clear();
    skeletonPose.reset();
}

void RigidObject::setModel(LevelLoader* levelLoader, FileResourcePtr<Model> _model) {
    removeModel();
    model = _model;
    if(!model) return;
    
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.diffuse && model->meshes[i]->material.diffuse->depth > 1)
            textureAnimationTime.push_back(0.0);
    
    if(model->skeleton)
        skeletonPose.reset(new btTransform[model->skeleton->bones.size()]);
}

void RigidObject::draw() {
    if(model) {
        modelMat = getTransformation();
        model->draw(this);
    }
    MatterObject::draw();
}

void RigidObject::drawAccumulatedMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

void RigidObject::prepareShaderProgram(Mesh* mesh) {
    if(mesh->material.diffuse) {
        if(mesh->material.diffuse->depth > 1) {
            unsigned int meshIndex = 0;
            for(unsigned int i = 0; i < model->meshes.size(); i ++) {
                if(model->meshes[i] == mesh)
                    break;
                if(model->meshes[i]->material.diffuse->depth > 1)
                    meshIndex ++;
            }
            mesh->material.diffuse->use(0, textureAnimationTime[meshIndex]);
        }else
            mesh->material.diffuse->use(0);
    }
    
    currentShaderProgram->setUniformF("discardDensity", clamp(integrity+1.0F, 0.0F, 1.0F));
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose.get(), model->skeleton->bones.size());
}

BoneLink* RigidObject::findBoneLinkOfName(const std::string& name) {
    if(!skeletonPose) return NULL;
    auto bones = model->skeleton->bones;
    auto iterator = bones.find(name);
    if(iterator == bones.end()) return NULL;
    Bone* bone = iterator->second;
    for(auto link : links)
        if(dynamic_cast<BoneLink*>(link) && ((BoneLink*)link)->bone == bone)
            return (BoneLink*)link;
    return NULL;
}



Reflective::Reflective(RigidObject* objectB, Mesh* meshB) :object(objectB), mesh(meshB), buffer(NULL) {
    
}

Reflective::~Reflective() {
    if(buffer)
        delete buffer;
}

PlaneReflective::PlaneReflective(RigidObject* objectB, Mesh* meshB) :Reflective(objectB, meshB) {
    
}

bool PlaneReflective::gameTick() {
    btBoxShape* shape = dynamic_cast<btBoxShape*>(object->getBody()->getCollisionShape());
    if(!shape) return false;
    
    btTransform transform = object->getTransformation();
    plane = transform.getBasis().getColumn(shape->getHalfExtentsWithoutMargin().minAxis());
    plane.setW(-plane.dot(transform.getOrigin()));
    if(plane.dot(currentCam->getTransformation().getOrigin()) < -plane.w()) return false;
    
    if(!buffer)
        buffer = new ColorBuffer(false, false, optionsState.videoWidth, optionsState.videoHeight);
    objectManager.currentReflective = this;
    
    if(currentCam->doFrustumCulling())
        return false;
    
    glEnable(GL_CLIP_DISTANCE0);
    objectManager.drawFrame(buffer->texture);
    glDisable(GL_CLIP_DISTANCE0);
    return true;
}