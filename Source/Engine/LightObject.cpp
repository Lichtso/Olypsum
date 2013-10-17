//
//  LightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

LightBoxVolume lightBox(btVector3(1, 1, 1));
LightSphereVolume lightSphere(1, 10, 6);
LightParabolidVolume lightCone(1, 12, 0);
LightParabolidVolume lightParabolid(1, 10, 4);

void initLightVolumes() {
    lightBox.init();
    lightSphere.init();
    lightCone.init();
    lightParabolid.init();
}



void LightObject::setPhysicsShape(btCollisionShape* shape) {
    if(body->getCollisionShape())
        delete body->getCollisionShape();
    body->setCollisionShape(shape);
}

LightObject::LightObject() {
    objectManager.lightObjects.push_back(this);
    body = new btCollisionObject();
    body->setCollisionShape(new btSphereShape(1.0));
    body->setUserPointer(this);
    body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Light, 0);
    
    shadowMap = NULL;
    color = Color4(1.0);
}

LightObject::~LightObject() {
    if(shadowMap) delete shadowMap;
}

void LightObject::removeClean() {
    for(int i = 0; i < objectManager.lightObjects.size(); i ++)
        if(objectManager.lightObjects[i] == this) {
            objectManager.lightObjects.erase(objectManager.lightObjects.begin()+i);
            break;
        }
    VisualObject::removeClean();
}

btTransform LightObject::getTransformation() {
    return shadowCam.getTransformation();
}

float LightObject::getRange() {
    return shadowCam.far;
}

bool LightObject::updateShadowMap(bool shadowActive) {
    if(!shadowActive) {
        if(shadowMap)
            deleteShadowMap();
        return false;
    }
    if(!shadowMap)
        shadowMap = new ColorBuffer(true, optionsState.cubemapsEnabled && dynamic_cast<PositionalLight*>(this),
                                    mainFBO.shadowMapSize, mainFBO.shadowMapSize);
    shadowCam.use();
    objectManager.currentShadowLight = this;
    return VisualObject::gameTick();
}

void LightObject::draw() {
    currentShaderProgram->setUniformF("lInvRange", 1.0/shadowCam.far);
    currentShaderProgram->setUniformVec3("lColor", color.getVector());
    
    bool reflection = dynamic_cast<PlaneReflective*>(objectManager.currentReflective);
    if(currentCam->testInverseNearPlaneHit(static_cast<btDbvtProxy*>(body->getBroadphaseHandle()))) { //PART IN FRUSTUM
        glDisable(GL_DEPTH_TEST);
        glFrontFace((reflection) ? GL_CCW : GL_CW);
    }else{ //ALL IN FRUSTUM
        glEnable(GL_DEPTH_TEST);
        glFrontFace((reflection) ? GL_CW : GL_CCW);
    }
    
    GLuint buffersLight[] = {
        mainFBO.gBuffers[positionDBuffer],
        mainFBO.gBuffers[normalDBuffer],
        mainFBO.gBuffers[materialDBuffer],
        mainFBO.gBuffers[diffuseDBuffer],
        mainFBO.gBuffers[specularDBuffer]
    };
    mainFBO.renderInBuffers(false, buffersLight, 3, &buffersLight[3], 2);
}

void LightObject::init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
    BaseObject::init(node, levelLoader);
    node = node->first_node("Color");
    if(!node) {
        log(error_log, "Tried to construct LightObject without \"Color\"-node.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(node->value(), "%f");
    color = Color4(vecData.data[0], vecData.data[1], vecData.data[2]);
}

rapidxml::xml_node<xmlUsedCharType>* LightObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("LightObject");
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("type");
    node->append_attribute(attribute);
    rapidxml::xml_node<xmlUsedCharType>* colorNode = doc.allocate_node(rapidxml::node_element);
    colorNode->name("Color");
    char buffer[64];
    sprintf(buffer, "%f %f %f", color.r, color.g, color.b);
    colorNode->value(doc.allocate_string(buffer));
    node->append_node(colorNode);
    return node;
}

void LightObject::deleteShadowMap() {
    if(shadowMap)
        delete shadowMap;
    shadowMap = NULL;
}



DirectionalLight::DirectionalLight() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptDirectionalLight.functionTemplate->GetFunction()->NewInstance(1, &external);

    shadowCam.near = 1.0;
}

DirectionalLight::DirectionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :DirectionalLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct DirectionalLight without \"Bounds\"-node.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(bounds->value(), "%f");
    setBounds(vecData.getVector3());
    LightObject::init(node, levelLoader);
}

void DirectionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(transformation * shiftMat);
}

void DirectionalLight::setBounds(btVector3 bounds) {
    shadowCam.fov = -bounds.y();
    shadowCam.aspect = bounds.x()/bounds.y();
    shadowCam.far = bounds.z();
    shadowCam.near = bounds.z()*0.01;
    
    bounds.setZ(bounds.z()*0.5);
    setPhysicsShape(new btBoxShape(bounds));
}

btVector3 DirectionalLight::getBounds() {
    return btVector3(-shadowCam.fov*shadowCam.aspect, -shadowCam.fov, shadowCam.far);
}

bool DirectionalLight::updateShadowMap(bool shadowActive) {
    shadowCam.updateViewMat();
    if(!LightObject::updateShadowMap(shadowActive)) return true;
    objectManager.currentShadowIsParabolid = false;
    
    mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
    objectManager.drawShadowCasters();
    return true;
}

void DirectionalLight::draw() {
    modelMat.setIdentity();
    modelMat.setBasis(modelMat.getBasis().scaled(btVector3(-shadowCam.fov*shadowCam.aspect, -shadowCam.fov, shadowCam.far*0.5)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far*0.5));
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shaderPrograms[directionalShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        shadowMap->use(3);
    }else
        shaderPrograms[directionalLightSP]->use();
    
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2));
    LightObject::draw();
    lightBox.draw();
}

float DirectionalLight::getPriority(btVector3 position) {
    return 2.0;
}

rapidxml::xml_node<xmlUsedCharType>* DirectionalLight::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = LightObject::write(doc, levelSaver);
    node->first_attribute("type")->value("directional");
    rapidxml::xml_node<xmlUsedCharType>* boundsNode = doc.allocate_node(rapidxml::node_element);
    boundsNode->name("Bounds");
    char buffer[64];
    sprintf(buffer, "%f %f %f", -shadowCam.fov*shadowCam.aspect, -shadowCam.fov, shadowCam.far);
    boundsNode->value(doc.allocate_string(buffer));
    node->append_node(boundsNode);
    return node;
}



SpotLight::SpotLight() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptSpotLight.functionTemplate->GetFunction()->NewInstance(1, &external);
    
    shadowCam.near = 0.1;
}

SpotLight::SpotLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :SpotLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct SpotLight without \"Bounds\"-node.");
        return;
    }
    float cutoff, range;
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = bounds->first_attribute("range");
    if(!attribute) {
        log(error_log, "Tried to construct SpotLight without \"range\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &range);
    attribute = bounds->first_attribute("cutoff");
    if(!attribute) {
        log(error_log, "Tried to construct SpotLight without \"cutoff\"-attribute.");
        return;
    }
    sscanf(attribute->value(), "%f", &cutoff);
    setBounds(cutoff, range);
    LightObject::init(node, levelLoader);
}

void SpotLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    btTransform shiftMat;
    shiftMat.setIdentity();
    shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
    body->setWorldTransform(transformation * shiftMat);
}

void SpotLight::setBounds(float cutoff, float range) {
    shadowCam.fov = cutoff;
    shadowCam.far = range;
    
    setPhysicsShape(new btConeShapeZ(tan(shadowCam.fov*0.5)*shadowCam.far, shadowCam.far));
}

float SpotLight::getCutoff() {
    return shadowCam.fov*0.5;
}

bool SpotLight::updateShadowMap(bool shadowActive) {
    if(!LightObject::updateShadowMap(shadowActive)) return true;
    objectManager.currentShadowIsParabolid = false;
    shadowCam.updateViewMat();
    
    mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
    //Render circle mask
    shaderPrograms[circleMaskSP]->use();
    rectVAO.draw();
    objectManager.drawShadowCasters();
    
    return true;
}

void SpotLight::draw() {
    float radius = tan(shadowCam.fov*0.5)*shadowCam.far;
    modelMat.setIdentity();
    modelMat.setBasis(modelMat.getBasis().scaled(btVector3(radius*1.05, radius*1.05, shadowCam.far)));
    modelMat.setOrigin(btVector3(0.0, 0.0, -shadowCam.far));
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shaderPrograms[spotShadowLightSP]->use();
        Matrix4 shadowMat = shadowCam.viewMat;
        shadowMat.makeTextureMat();
        currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowMat);
        shadowMap->use(3);
    }else
        shaderPrograms[spotLightSP]->use();
    
    currentShaderProgram->setUniformF("lCutoff", -cos(shadowCam.fov*0.5));
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2)*-1.0);
    LightObject::draw();
    lightCone.draw();
}

float SpotLight::getPriority(btVector3 position) {
    return 1.0;
}

rapidxml::xml_node<xmlUsedCharType>* SpotLight::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = LightObject::write(doc, levelSaver);
    node->first_attribute("type")->value("spot");
    rapidxml::xml_node<xmlUsedCharType>* boundsNode = doc.allocate_node(rapidxml::node_element);
    boundsNode->name("Bounds");
    node->append_node(boundsNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("cutoff");
    attribute->value(doc.allocate_string(stringOf(shadowCam.fov).c_str()));
    boundsNode->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("range");
    attribute->value(doc.allocate_string(stringOf(shadowCam.far).c_str()));
    boundsNode->append_attribute(attribute);
    return node;
}



PositionalLight::PositionalLight() :shadowMapB(NULL) {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    scriptPositionalLight.functionTemplate->GetFunction()->NewInstance(1, &external);
    
    shadowCam.near = 0.1;
}

PositionalLight::PositionalLight(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :PositionalLight() {
    rapidxml::xml_node<xmlUsedCharType>* bounds = node->first_node("Bounds");
    if(!node) {
        log(error_log, "Tried to construct PositionalLight without \"Bounds\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = bounds->first_attribute("range");
    if(!attribute) {
        log(error_log, "Tried to construct PositionalLight without \"range\"-attribute.");
        return;
    }
    float range;
    sscanf(attribute->value(), "%f", &range);
    attribute = bounds->first_attribute("omniDirectional");
    if(!attribute) {
        log(error_log, "Tried to construct PositionalLight without \"omniDirectional\"-attribute.");
        return;
    }
    setBounds(strcmp(attribute->value(), "true") == 0, range);
    LightObject::init(node, levelLoader);
}

PositionalLight::~PositionalLight() {
    if(shadowMapB) delete shadowMapB;
}

void PositionalLight::setTransformation(const btTransform& transformation) {
    shadowCam.setTransformation(transformation);
    if(getOmniDirectional())
        body->setWorldTransform(transformation);
    else{
        btTransform shiftMat;
        shiftMat.setIdentity();
        shiftMat.setOrigin(btVector3(0, 0, -shadowCam.far*0.5));
        body->setWorldTransform(transformation * shiftMat);
    }
}

void PositionalLight::setBounds(bool omniDirectional, float range) {
    shadowCam.fov = (omniDirectional) ? M_PI*2.0 : M_PI;
    shadowCam.far = range;
    
    if(omniDirectional)
        setPhysicsShape(new btSphereShape(range));
    else
        setPhysicsShape(new btCylinderShape(btVector3(range, range, range*0.5)));
}

bool PositionalLight::getOmniDirectional() {
    return abs(shadowCam.fov-M_PI*2.0) < 0.001;
}

bool PositionalLight::updateShadowMap(bool shadowActive) {
    if(!LightObject::updateShadowMap(shadowActive)) return true;
    shadowCam.updateViewMat();
    Matrix4 viewMat = shadowCam.viewMat;
    
    if(optionsState.cubemapsEnabled) {
        objectManager.currentShadowIsParabolid = false;
        float fov = shadowCam.fov;
        shadowCam.fov = M_PI_2;
        btTransform camMat = shadowCam.getTransformation();
        for(GLenum side = GL_TEXTURE_CUBE_MAP_POSITIVE_X; side <= GL_TEXTURE_CUBE_MAP_NEGATIVE_Z; side ++) {
            btQuaternion rotation;
            switch(side) {
                case GL_TEXTURE_CUBE_MAP_POSITIVE_X:
                    rotation.setEulerZYX(M_PI, M_PI_2, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_X:
                    rotation.setEulerZYX(M_PI, -M_PI_2, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Y:
                    rotation.setEulerZYX(0.0, 0.0, M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Y:
                    rotation.setEulerZYX(0.0, 0.0, -M_PI_2);
                break;
                case GL_TEXTURE_CUBE_MAP_POSITIVE_Z:
                    rotation.setEulerZYX(M_PI, M_PI, 0.0);
                break;
                case GL_TEXTURE_CUBE_MAP_NEGATIVE_Z:
                    rotation.setEulerZYX(M_PI, 0.0, 0.0);
                break;
            }
            btTransform rotTransform = btTransform::getIdentity();
            rotTransform.setRotation(rotation);
            shadowCam.setTransformation(camMat * rotTransform);
            shadowCam.updateViewMat();
            mainFBO.renderInTexture(shadowMap, side);
            objectManager.drawShadowCasters();
        }
        shadowCam.setTransformation(camMat);
        shadowCam.viewMat = viewMat;
        shadowCam.fov = fov;
    }else{
        if(getOmniDirectional() && !shadowMapB)
            shadowMapB = new ColorBuffer(true, false, mainFBO.shadowMapSize, mainFBO.shadowMapSize);
        
        glEnable(GL_CLIP_DISTANCE0);
        objectManager.currentShadowIsParabolid = true;
        shaderPrograms[solidParabolidShadowSP]->use();
        shaderPrograms[solidParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        shaderPrograms[skeletalParabolidShadowSP]->use();
        shaderPrograms[skeletalParabolidShadowSP]->setUniformF("lRange", shadowCam.far);
        mainFBO.renderInTexture(shadowMap, GL_TEXTURE_2D);
        objectManager.drawShadowCasters();
        if(shadowMapB) {
            shadowCam.viewMat.scale(btVector3(-1.0, 1.0, -1.0));
            mainFBO.renderInTexture(shadowMapB, GL_TEXTURE_2D);
            objectManager.drawShadowCasters();
            shadowCam.viewMat = viewMat;
        }
        glDisable(GL_CLIP_DISTANCE0);
    }
    return true;
}

void PositionalLight::draw() {
    modelMat.setIdentity();
    btMatrix3x3 basis(btQuaternion(btVector3(0.0, 1.0, 0.0), M_PI));
    modelMat.setBasis(basis.scaled(btVector3(shadowCam.far*1.05, shadowCam.far*1.05, shadowCam.far*1.05)));
    modelMat = shadowCam.getTransformation()*modelMat;
    
    if(shadowMap) {
        shadowMap->use(3);
        if(shadowMapB) {
            shadowMapB->use(4);
            shaderPrograms[positionalShadowDualLightSP]->use();
        }else
            shaderPrograms[positionalShadowLightSP]->use();
    }else
        shaderPrograms[positionalLightSP]->use();
    
    currentShaderProgram->setUniformMatrix4("lShadowMat", &shadowCam.viewMat);
    currentShaderProgram->setUniformF("lCutoff", (getOmniDirectional()) ? 1.0 : 0.0);
    currentShaderProgram->setUniformVec3("lPosition", shadowCam.getTransformation().getOrigin());
    currentShaderProgram->setUniformVec3("lDirection", shadowCam.getTransformation().getBasis().getColumn(2)*-1.0);
    if(optionsState.cubemapsEnabled)
        currentShaderProgram->setUniformVec2("shadowDepthTransform", (shadowCam.far+shadowCam.near)/(shadowCam.far-shadowCam.near)*0.5+0.5,
                                                                    -(shadowCam.far*shadowCam.near)/(shadowCam.far-shadowCam.near)*1.005);
    
    LightObject::draw();
    if(abs(shadowCam.fov-M_PI) < 0.001)
        lightParabolid.draw();
    else
        lightSphere.draw();
}

void PositionalLight::deleteShadowMap() {
    LightObject::deleteShadowMap();
    if(shadowMapB)
        delete shadowMapB;
    shadowMapB = NULL;
}

float PositionalLight::getPriority(btVector3 position) {
    return 1.0-(position-shadowCam.getTransformation().getOrigin()).length()/shadowCam.far;
}

rapidxml::xml_node<xmlUsedCharType>* PositionalLight::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = LightObject::write(doc, levelSaver);
    node->first_attribute("type")->value("positional");
    rapidxml::xml_node<xmlUsedCharType>* boundsNode = doc.allocate_node(rapidxml::node_element);
    boundsNode->name("Bounds");
    node->append_node(boundsNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("omniDirectional");
    if(getOmniDirectional())
        attribute->value("true");
    else
        attribute->value("false");
    boundsNode->append_attribute(attribute);
    attribute = doc.allocate_attribute();
    attribute->name("range");
    attribute->value(doc.allocate_string(stringOf(shadowCam.far).c_str()));
    boundsNode->append_attribute(attribute);
    return node;
}