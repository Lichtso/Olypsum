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
    objectManager.graphicObjects.erase(this);
    DisplayObject::removeClean();
}



void ModelObject::removeClean() {
    setModel(NULL);
    GraphicObject::removeClean();
}

void ModelObject::removeFast() {
    textureAnimation.clear();
    skeletonPose.reset();
    GraphicObject::removeFast();
}

void ModelObject::setupBones(BaseObject* object, Bone* bone) {
    LinkInitializer initializer;
    initializer.object[0] = object;
    initializer.object[1] = object = new BoneObject(bone);
    initializer.name[0] = ".."; //parent
    initializer.name[1] = bone->name;
    new TransformLink(initializer);
    for(auto childBone : bone->children)
        setupBones(object, childBone);
}

void ModelObject::writeBones(rapidxml::xml_document<char> &doc, LevelSaver* levelSaver,
                             rapidxml::xml_node<xmlUsedCharType>* node, BoneObject *object) {
    node->append_node(object->write(doc, levelSaver));
    for(auto iterator : object->links) {
        if(iterator.first == "..") continue; //Don't process parent again
        BoneObject* boneObject = dynamic_cast<BoneObject*>(iterator.second->getOther(object));
        if(boneObject)
            writeBones(doc, levelSaver, node, boneObject);
    }
}

void ModelObject::updateSkeletonPose(BaseObject* object, Bone* bone) {
    skeletonPose.get()[bone->jointIndex] = object->getTransformation() * bone->absoluteInv;
    for(auto childBone : bone->children)
        updateSkeletonPose(object->links[childBone->name]->getOther(object), childBone);
}

void ModelObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptModelObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
}

bool ModelObject::gameTick() {
    BaseObject::gameTick();
    if(!model) return false;
    if(model->skeleton) {
        Bone* rootBone = model->skeleton->rootBone;
        updateSkeletonPose(links[rootBone->name]->getOther(this), rootBone);
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
    return true;
}

void ModelObject::setModel(FileResourcePtr<Model> _model) {
    if(model) {
        for(unsigned int i = 0; i < model->meshes.size(); i ++)
            if(model->meshes[i]->material.reflectivity != 0)
                objectManager.reflectiveAccumulator.erase(this);
        foreach_e(links, link)
            if(dynamic_cast<TransformLink*>(link->second))
                link->second->removeClean(this, link);
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
        setupBones(this, model->skeleton->rootBone);
    }
}

void ModelObject::draw() {
    if(!model) return;
    modelMat = getTransformation();
    model->draw(this);
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
    setModel(fileManager.initResource<Model>(attribute->value()));
    
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
    
    if((parameterNode = node->first_node("SkeletonPose"))) {
        parameterNode = parameterNode->first_node();
        while(parameterNode) {
            attribute = parameterNode->first_attribute("path");
            if(!attribute) {
                log(error_log, "Tried to construct BoneObject without \"path\"-attribute.");
                return;
            }
            BoneObject* boneObject = dynamic_cast<BoneObject*>(findObjectByPath(attribute->value()));
            if(!boneObject) {
                log(error_log, "Tried to construct BoneObject with invalid path.");
                return;
            }
            boneObject->init(parameterNode, levelLoader);
            parameterNode = parameterNode->next_sibling();
        }
    }
    
    if((parameterNode = node->first_node("Integrity")))
        sscanf(parameterNode->value(), "%f", &integrity);
}

rapidxml::xml_node<xmlUsedCharType>* ModelObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    if(!model) {
        log(error_log, "Tried to save ModelObject without model.");
        return node;
    }
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
    if(skeletonPose) {
        rapidxml::xml_node<xmlUsedCharType>* skeletonPoseNode = doc.allocate_node(rapidxml::node_element);
        skeletonPoseNode->name("SkeletonPose");
        writeBones(doc, levelSaver, skeletonPoseNode,
                   static_cast<BoneObject*>(links[model->skeleton->rootBone->name]->getOther(this)));
        node->append_node(skeletonPoseNode);
    }
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
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
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