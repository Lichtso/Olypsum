//
//  DisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptDisplayObject.h"

GraphicObject::GraphicObject() {
    objectManager.graphicObjects.insert(this);
}

void GraphicObject::removeClean() {
    for(auto decal : decals)
        delete decal;
    
    decals.clear();
    objectManager.graphicObjects.erase(this);
    
    DisplayObject::removeClean();
}

void GraphicObject::removeFast() {
    for(auto decal : decals)
        delete decal;
    
    DisplayObject::removeFast();
}

bool GraphicObject::gameTick() {
    foreach_e(decals, iterator) {
        (*iterator)->life -= profiler.animationFactor;
        if((*iterator)->life > 0.0) continue;
        delete *iterator;
        decals.erase(iterator);
    }
    
    return DisplayObject::gameTick();
}

void GraphicObject::draw() {
    if(objectManager.currentShadowLight) return;
    
    btTransform transform = getTransformation();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    
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



void ModelObject::removeClean() {
    setModel(NULL, NULL);
    GraphicObject::removeClean();
}

void ModelObject::removeFast() {
    textureAnimation.clear();
    skeletonPose.reset();
    GraphicObject::removeFast();
}

void ModelObject::setupBones(LevelLoader* levelLoader, BaseObject* object, Bone* bone) {
    LinkInitializer initializer;
    initializer.object[0] = object;
    initializer.object[1] = object = new BoneObject(bone);
    btTransform transform = btTransform::getIdentity();
    (new TransformLink())->init(initializer, transform);
    if(levelLoader)
        levelLoader->pushObject(object);
    for(auto childBone : bone->children)
        setupBones(levelLoader, object, childBone);
}

void ModelObject::writeBones(rapidxml::xml_document<char> &doc, LevelSaver* levelSaver, BoneObject *object) {
    object->write(doc, levelSaver);
    for(unsigned int i = 0; i < object->bone->children.size(); i ++)
        for(auto iterator : object->links) {
            BoneObject* boneObject = dynamic_cast<BoneObject*>(iterator->b);
            if(!boneObject || boneObject->bone != object->bone->children[i]) continue;
            writeBones(doc, levelSaver, boneObject);
            break;
        }
}

void ModelObject::updateSkeletonPose(BoneObject* object, Bone* bone) {
    skeletonPose.get()[bone->jointIndex] = object->getTransformation() * bone->absoluteInv;
    for(auto iterator : object->links) {
        BoneObject* boneObject = dynamic_cast<BoneObject*>(iterator->b);
        if(boneObject && boneObject != object)
            updateSkeletonPose(boneObject, boneObject->bone);
    }
}

void ModelObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptModelObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
}

bool ModelObject::gameTick() {
    if(model->skeleton) {
        Bone* rootBone = model->skeleton->rootBone;
        updateSkeletonPose(getRootBone(), rootBone);
    }
    for(unsigned int i = 0; i < textureAnimation.size(); i ++)
        if(textureAnimation[i ++] >= 0.0)
            textureAnimation[i ++] += profiler.animationFactor;
    if(integrity <= 0.0) {
        integrity -= profiler.animationFactor;
        if(integrity < -1.0) {
            removeClean();
            return false;
        }
    }
    return GraphicObject::gameTick();
}

BoneObject* ModelObject::getRootBone() {
    if(model->skeleton) {
        Bone* rootBone = model->skeleton->rootBone;
        for(auto iterator : links) {
            if(!dynamic_cast<TransformLink*>(iterator)) continue;
            BoneObject* rootBoneObject = dynamic_cast<BoneObject*>(iterator->b);
            if(rootBoneObject && rootBoneObject->bone == rootBone)
                return rootBoneObject;
        }
    }
    return NULL;
}

void ModelObject::setModel(LevelLoader* levelLoader, FileResourcePtr<Model> _model) {
    if(model) {
        for(unsigned int i = 0; i < model->meshes.size(); i ++)
            if(model->meshes[i]->material.reflectivity != 0)
                objectManager.reflectiveAccumulator.erase(this);
        foreach_e(links, iterator)
            if(dynamic_cast<TransformLink*>(*iterator) && dynamic_cast<BoneObject*>((*iterator)->b))
                (*iterator)->removeClean(this);
    }
    textureAnimation.clear();
    skeletonPose.reset();
    
    model = _model;
    if(!model) return;
    
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.diffuse && model->meshes[i]->material.diffuse->depth > 1)
            textureAnimation.push_back(0.0);
    
    if(model->skeleton) {
        skeletonPose.reset(new btTransform[model->skeleton->bones.size()]);
        setupBones(levelLoader, this, model->skeleton->rootBone);
    }
}

void ModelObject::draw() {
    if(model) {
        modelMat = getTransformation();
        model->draw(this);
    }
    GraphicObject::draw();
}

void ModelObject::drawAccumulatedMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

void ModelObject::prepareShaderProgram(Mesh* mesh) {
    if(mesh->material.diffuse) {
        if(mesh->material.diffuse->depth > 1) {
            unsigned int meshIndex = 0;
            for(unsigned int i = 0; i < model->meshes.size(); i ++) {
                if(model->meshes[i] == mesh)
                    break;
                if(model->meshes[i]->material.diffuse->depth > 1)
                    meshIndex ++;
            }
            mesh->material.diffuse->use(0, textureAnimation[meshIndex]);
        }else
            mesh->material.diffuse->use(0);
    }
    
    currentShaderProgram->setUniformF("discardDensity", clamp(integrity+1.0F, 0.0F, 1.0F));
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose.get(), model->skeleton->bones.size());
}

void ModelObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Model");
    if(!parameterNode) {
        log(error_log, "Tried to construct ModelObject without \"Model\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("src");
    if(!attribute) {
        log(error_log, "Found \"Model\"-node without \"src\"-attribute.");
        return;
    }
    setModel(levelLoader, fileManager.getResourceByPath<Model>(attribute->value()));
    
    if((parameterNode = node->first_node("TextureAnimation"))) {
        XMLValueArray<float> animationTime;
        animationTime.readString(parameterNode->value(), "%f");
        if(animationTime.count != textureAnimation.size()) {
            log(error_log, "Tried to construct ModelObject with invalid \"TextureAnimation\"-node.");
            return;
        }
        for(unsigned int i = 0; i < textureAnimation.size(); i ++)
            textureAnimation[i] = animationTime.data[i];
    }
    
    if((parameterNode = node->first_node("Integrity")))
        sscanf(parameterNode->value(), "%f", &integrity);
}

rapidxml::xml_node<xmlUsedCharType>* ModelObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    node->append_node(fileManager.writeResource(doc, "Model", model));
    if(textureAnimation.size() > 0) {
        rapidxml::xml_node<xmlUsedCharType>* textureAnimationNode = doc.allocate_node(rapidxml::node_element);
        textureAnimationNode->name("TextureAnimation");
        std::ostringstream data;
        for(unsigned int i = 0; i < textureAnimation.size(); i ++) {
            if(i > 0) data << " ";
            data << textureAnimation[i];
        }
        textureAnimationNode->value(doc.allocate_string(data.str().c_str()));
        node->append_node(textureAnimationNode);
    }
    if(integrity != 1.0) {
        rapidxml::xml_node<xmlUsedCharType>* integrityNode = doc.allocate_node(rapidxml::node_element);
        integrityNode->name("Integrity");
        integrityNode->value(doc.allocate_string(stringOf(integrity).c_str()));
        node->append_node(integrityNode);
    }
    if(skeletonPose)
        writeBones(doc, levelSaver, getRootBone());
    return node;
}



Reflective::Reflective(ModelObject* objectB, Mesh* meshB) :object(objectB), mesh(meshB), buffer(NULL) {
    
}

Reflective::~Reflective() {
    if(buffer)
        delete buffer;
}

PlaneReflective::PlaneReflective(ModelObject* objectB, Mesh* meshB) :Reflective(objectB, meshB) {
    
}

bool PlaneReflective::gameTick() {
    btBoxShape* shape = dynamic_cast<btBoxShape*>(object->getBody()->getCollisionShape());
    if(!shape) return false;
    
    btTransform transform = object->getTransformation();
    plane = transform.getBasis().getColumn(shape->getHalfExtentsWithoutMargin().minAxis());
    plane.setW(-plane.dot(transform.getOrigin()));
    if(plane.dot(currentCam->getTransformation().getOrigin()) < plane.w()) return false;
    
    if(!buffer)
        buffer = new ColorBuffer(false, false, prevOptionsState.videoWidth, prevOptionsState.videoHeight);
    objectManager.currentReflective = this;
    
    if(currentCam->doFrustumCulling())
        return false;
    
    glEnable(GL_CLIP_DISTANCE0);
    objectManager.drawFrame(buffer->texture);
    glDisable(GL_CLIP_DISTANCE0);
    return true;
}



SoftObject::SoftObject() {
    //btSoftBodyHelpers
}

void SoftObject::removeClean() {
    if(body) {
        btSoftBody* softBody = getBody();
        objectManager.physicsWorld->removeSoftBody(softBody);
        delete softBody;
        body = NULL;
    }
    GraphicObject::removeClean();
}

void SoftObject::removeFast() {
    if(body) {
        btSoftBody* softBody = getBody();
        delete softBody;
        body = NULL;
    }
    GraphicObject::removeFast();
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
    ModelObject::init(node, levelLoader);
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
    btCollisionShape* collisionShape = PhysicObject::readCollisionShape(parameterNode, levelLoader);
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
}

void RigidObject::removeClean() {
    if(body) {
        btRigidBody* rigidBody = getBody();
        delete rigidBody->getMotionState();
        objectManager.physicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        body = NULL;
    }
    ModelObject::removeClean();
}

void RigidObject::removeFast() {
    if(body) {
        btRigidBody* rigidBody = getBody();
        delete rigidBody->getMotionState();
        delete rigidBody;
        body = NULL;
    }
    ModelObject::removeFast();
}

void RigidObject::setTransformation(const btTransform& transformation) {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    motionState->transformation = transformation;
}

btTransform RigidObject::getTransformation() {
    simpleMotionState* motionState = static_cast<simpleMotionState*>(getBody()->getMotionState());
    return motionState->transformation;
}

void RigidObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptRigidObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), instance);
}

rapidxml::xml_node<xmlUsedCharType>* RigidObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = ModelObject::write(doc, levelSaver);
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