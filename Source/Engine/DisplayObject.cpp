//
//  DisplayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.08.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

GraphicObject::GraphicObject() {
    objectManager.graphicObjects.insert(this);
}

void GraphicObject::remove() {
    objectManager.graphicObjects.erase(this);
    BaseObject::remove();
}



ModelObject::~ModelObject() {
    if(skeletonPose) delete [] skeletonPose;
    if(textureAnimation) delete [] textureAnimation;
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.reflectivity != 0)
            objectManager.reflectiveAccumulator.erase(this);
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
    skeletonPose[bone->jointIndex] = object->getTransformation() * bone->absoluteInv;
    for(auto childBone : bone->children)
        updateSkeletonPose(object->links[childBone->name]->getOther(object), childBone);
}

/* Not OpenGL 3.2 compatible
void ModelObject::drawBonePose(BaseObject* object, Bone* bone, float axesSize, float linesSize, float textSize) {
    btTransform transform = object->getTransformation(), inverseTransform = transform.inverse();
    btVector3* childrenPos = (linesSize > 0.0) ? new btVector3[bone->children.size()] : NULL;
    for(unsigned int i = 0; i < bone->children.size(); i ++) {
        BaseObject* childObject = object->links[bone->children[i]->name]->getOther(object);
        drawBonePose(childObject, bone->children[i], axesSize, linesSize, textSize);
        if(linesSize > 0.0) childrenPos[i] = inverseTransform(modelMat.getOrigin());
    }
    
    if(textSize > 0.0)
        mainFont->renderStringToScreen(bone->name.c_str(), transform.getOrigin(), 0.02, Color4(1.0), false);
    modelMat = transform;
    shaderPrograms[colorSP]->use();
    
    float* vertices;
    {
        unsigned int verticesCount = 36+12*bone->children.size();
        if(axesSize > 0.0) verticesCount += 36;
        if(linesSize > 0.0) verticesCount += 12*bone->children.size();
        vertices = (verticesCount) ? new float[verticesCount] : NULL;
    }
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, vertices);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &vertices[3]);
    
    if(axesSize > 0.0) {
        for(unsigned int i = 0; i < 6; i ++) {
            vertices[i*6  ] = (i == 1) ? axesSize : 0.0;
            vertices[i*6+1] = (i == 3) ? axesSize : 0.0;
            vertices[i*6+2] = (i == 5) ? axesSize : 0.0;
            vertices[i*6+3] = (i < 2) ? 1.0 : 0.0;
            vertices[i*6+4] = (i == 2 || i == 3) ? 1.0 : 0.0;
            vertices[i*6+5] = (i >= 4) ? 1.0 : 0.0;
        }
        glDrawArrays(GL_LINES, 0, 6);
    }
    
    if(linesSize > 0.0) {
        for(unsigned int i = 0; i < bone->children.size(); i ++) {
            btVector3 pos = childrenPos[i];
            vertices[i*12] = 0.0;
            vertices[i*12+1] = 0.0;
            vertices[i*12+2] = 0.0;
            vertices[i*12+3] = 1.0;
            vertices[i*12+4] = 1.0;
            vertices[i*12+5] = 1.0;
            vertices[i*12+6] = pos.x();
            vertices[i*12+7] = pos.y();
            vertices[i*12+8] = pos.z();
            vertices[i*12+9] = 0.5;
            vertices[i*12+10] = 0.5;
            vertices[i*12+11] = 0.5;
        }
        glLineWidth(linesSize);
        glDrawArrays(GL_LINES, 0, bone->children.size()*2);
        glLineWidth(1);
        delete [] childrenPos;
    }
    
    if(vertices) delete [] vertices;
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
}*/

bool ModelObject::gameTick() {
    BaseObject::gameTick();
    if(model->skeleton) {
        Bone* rootBone = model->skeleton->rootBone;
        updateSkeletonPose(links[rootBone->name]->getOther(this), rootBone);
    }
    unsigned int animatedMeshes = 0;
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.diffuse && model->meshes[i]->material.diffuse->depth > 1)
            textureAnimation[animatedMeshes ++] += profiler.animationFactor;
    return true;
}

void ModelObject::draw() {
    modelMat = getTransformation();
    model->draw(this);
}

void ModelObject::drawAccumulatedMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

/* Not OpenGL 3.2 compatible
void ModelObject::drawSkeletonPose(float axesSize, float linesSize, float textSize) {
    Bone* rootBone = model->skeleton->rootBone;
    drawBonePose(links[rootBone->name]->getOther(this), rootBone, axesSize, linesSize, textSize);
}*/

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
    
    currentShaderProgram->setUniformF("discardDensity", 1.0);
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose, model->skeleton->bones.size());
}

void ModelObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    levelLoader->pushObject(this);
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Model");
    if(!parameterNode) {
        log(error_log, "Tried to construct ModelObject without \"Model\"-node.");
        return;
    }
    model = fileManager.initResource<Model>(parameterNode);
    
    unsigned int animatedMeshes = 0;
    for(unsigned int i = 0; i < model->meshes.size(); i ++)
        if(model->meshes[i]->material.diffuse && model->meshes[i]->material.diffuse->depth > 1)
            animatedMeshes ++;
    if(animatedMeshes > 0)
        textureAnimation = new float[animatedMeshes];
    parameterNode = node->first_node("TextureAnimation");
    if(parameterNode) {
        XMLValueArray<float> animationTime;
        animationTime.readString(parameterNode->value(), "%f");
        if(animationTime.count != animatedMeshes) {
            log(error_log, "Tried to construct ModelObject with invalid \"TextureAnimation\"-node.");
            return;
        }
        memcpy(textureAnimation, animationTime.data, animationTime.count*sizeof(float));
    }
    
    if(model->skeleton) {
        skeletonPose = new btTransform[model->skeleton->bones.size()];
        setupBones(this, model->skeleton->rootBone);
    }
    parameterNode = node->first_node("SkeletonPose");
    if(parameterNode) {
        parameterNode = parameterNode->first_node();
        while(parameterNode) {
            rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("path");
            if(!attribute) {
                log(error_log, "Tried to construct BoneObject without \"path\"-attribute.");
                return;
            }
            BoneObject* boneObject = dynamic_cast<BoneObject*>(findObjectByPath(attribute->value()));
            if(!boneObject) {
                log(error_log, "Tried to construct BoneObject with invalid path.");
                return;
            }
            levelLoader->pushObject(boneObject);
            boneObject->setTransformation(BaseObject::readTransformtion(parameterNode, levelLoader));
            parameterNode = parameterNode->next_sibling();
        }
    }
}

rapidxml::xml_node<xmlUsedCharType>* ModelObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = PhysicObject::write(doc, levelSaver);
    if(!model) {
        log(error_log, "Tried to save ModelObject without model.");
        return node;
    }
    node->append_node(fileManager.writeResource(doc, "Model", model));
    if(textureAnimation) {
        rapidxml::xml_node<xmlUsedCharType>* textureAnimationNode = doc.allocate_node(rapidxml::node_element);
        textureAnimationNode->name("TextureAnimation");
        std::ostringstream data;
        unsigned int animatedMeshes = 0;
        for(unsigned int i = 0; i < model->meshes.size(); i ++)
            if(model->meshes[i]->material.diffuse->depth > 1) {
                if(animatedMeshes > 0) data << " ";
                data << textureAnimation[animatedMeshes ++];
            }
        textureAnimationNode->value(doc.allocate_string(data.str().c_str()));
        node->append_node(textureAnimationNode);
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
        buffer = new ColorBuffer(false, false, screenSize[0], screenSize[1]);
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
    btRigidBody::btRigidBodyConstructionInfo rBCI(mass, new simpleMotionState(transform), collisionShape, localInertia);
    body = new btRigidBody(rBCI);
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
        float linear, angular;
        attribute = parameterNode->first_attribute("linear");
        if(!attribute) {
            log(error_log, "Tried to construct RigidObject-Damping without \"linear\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &linear);
        attribute = parameterNode->first_attribute("angular");
        if(!attribute) {
            log(error_log, "Tried to construct RigidObject-Damping without \"angular\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%f", &angular);
        body->setDamping(linear, angular);
    }
}

RigidObject::~RigidObject() {
    if(body) {
        btRigidBody* rigidBody = getBody();
        while(rigidBody->getNumConstraintRefs() > 0) {
            btTypedConstraint* constraint = rigidBody->getConstraintRef(0);
            objectManager.physicsWorld->removeConstraint(constraint);
            delete constraint;
        }
        delete rigidBody->getMotionState();
        objectManager.physicsWorld->removeRigidBody(rigidBody);
        delete rigidBody;
        body = NULL;
    }
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
    if(model)
        ModelObject::gameTick();
    else
        BaseObject::gameTick();
    return true;
}

void RigidObject::draw() {
    //modelMat = getTransformation();
    
    //TODO: Debug drawing
    /*btBoxShape* boxShape = dynamic_cast<btBoxShape*>(body->getCollisionShape());
    if(boxShape) {
        LightBoxVolume bV(boxShape->getHalfExtentsWithoutMargin());
        bV.init();
        bV.drawDebug(Color4(1.0, 1.0, 0.0, 1.0));
    }*/
    
    if(model)
        ModelObject::draw();
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



WaterObject::WaterObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :waveSpeed(0.05) {
    ModelObject::init(node, levelLoader);
    PhysicObject::init(node, levelLoader);
}

WaterObject::~WaterObject() {
    
}

void WaterObject::addWave(float duration, float ampitude, float length, float originX, float originY) {
    if(waves.size() >= MAX_WAVES) return;
    Wave wave;
    wave.age = 0.0;
    wave.duration = duration;
    wave.ampitude = ampitude;
    wave.length = length;
    wave.originX = originX;
    wave.originY = originY;
    waves.push_back(wave);
}

bool WaterObject::gameTick() {
    for(unsigned int i = 0; i < waves.size(); i ++) {
        waves[i].age += profiler.animationFactor;
        if(waves[i].age >= waves[i].duration) {
            waves.erase(waves.begin()+i);
            i --;
        }
    }
    return BaseObject::gameTick();
}

void WaterObject::prepareShaderProgram(Mesh* mesh) {
    shaderPrograms[waterAnimatedGSP]->use();
    char str[64];
    for(unsigned int i = 0; i < MAX_WAVES; i ++) {
        bool active = (i < waves.size());
        sprintf(str, "waveLen[%d]", i);
        currentShaderProgram->setUniformF(str, active ? (1.0/waves[i].length) : 0.0);
        sprintf(str, "waveAge[%d]", i);
        currentShaderProgram->setUniformF(str, active ? waves[i].age/waves[i].length*waveSpeed-M_PI*2.0 : 0.0);
        sprintf(str, "waveAmp[%d]", i);
        currentShaderProgram->setUniformF(str, active ? waves[i].ampitude*(1.0-waves[i].age/waves[i].duration) : 0.0);
        sprintf(str, "waveOri[%d]", i);
        if(active)
            currentShaderProgram->setUniformVec2(str, waves[i].originX, waves[i].originY);
        else
            currentShaderProgram->setUniformVec2(str, 0.0, 0.0);
    }
    currentShaderProgram->setUniformF("discardDensity", 1.0);
}

void WaterObject::draw() {
    if(!objectManager.currentShadowLight)
        ModelObject::draw();
}

rapidxml::xml_node<xmlUsedCharType>* WaterObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = ModelObject::write(doc, levelSaver);
    node->name("WaterObject");
    return node;
}