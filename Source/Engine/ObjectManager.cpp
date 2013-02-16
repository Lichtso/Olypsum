//
//  ObjectManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//
//

#include "AppMain.h"

ALCdevice* soundDevice;
ALCcontext* soundContext;
VertexArrayObject decalVAO;

//! @cond
class LightPrioritySorter {
    public:
    btVector3 position;
    bool operator()(LightObject* a, LightObject* b) {
        return a->getPriority(position) > b->getPriority(position);
    }
};

static void calculatePhysicsTick(btDynamicsWorld* world, btScalar timeStep) {
    objectManager.physicsTick();
}
//! @endcond

ObjectManager::ObjectManager() {
    currentShadowLight = NULL;
    collisionConfiguration = new btDefaultCollisionConfiguration();
    collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
    constraintSolver = new btSequentialImpulseConstraintSolver();
    broadphase = new btDbvtBroadphase();
}

ObjectManager::~ObjectManager() {
    alcDestroyContext(soundContext);
    alcCloseDevice(soundDevice);
    clear();
    
    delete broadphase;
    delete constraintSolver;
    delete collisionDispatcher;
    delete collisionConfiguration;
}

void ObjectManager::init() {
    //Init Decals
    float vertices[] = {
        -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0
    };
    VertexArrayObject::Attribute attr;
    std::vector<VertexArrayObject::Attribute> attributes;
    attr.name = POSITION_ATTRIBUTE;
    attr.size = 3;
    attributes.push_back(attr);
    attr.name = TEXTURE_COORD_ATTRIBUTE;
    attr.size = 2;
    attributes.push_back(attr);
    attr.name = NORMAL_ATTRIBUTE;
    attr.size = 3;
    attributes.push_back(attr);
    decalVAO.init(attributes, false);
    decalVAO.updateVertices(32, vertices, GL_STATIC_DRAW);
    decalVAO.elementsCount = 4;
    decalVAO.drawType = GL_TRIANGLE_STRIP;
    
    //Init Sound
    soundDevice = alcOpenDevice(NULL);
    soundContext = alcCreateContext(soundDevice, NULL);
    alcMakeContextCurrent(soundContext);
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    log(info_log, std::string("OpenAL, sound output: ")+alcGetString(soundDevice, ALC_DEVICE_SPECIFIER));
}

void ObjectManager::clear() {
    currentShadowLight = NULL;
    
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        delete lightObjects[i];
    lightObjects.clear();
    
    for(unsigned int i = 0; i < transparentAccumulator.size(); i ++)
        delete transparentAccumulator[i];
    transparentAccumulator.clear();
    
    for(auto decal : decals)
        delete decal;
    decals.clear();
    
    for(auto graphicObject : graphicObjects)
        delete graphicObject;
    graphicObjects.clear();
    
    for(auto particlesObject : particlesObjects)
        delete particlesObject;
    particlesObjects.clear();
    
    for(auto simpleObject : simpleObjects)
        delete simpleObject;
    simpleObjects.clear();
    
    physicsWorld.reset();
    controlsMangager.reset();
    scriptManager.reset();
}

void ObjectManager::initGame() {
    physicsWorld.reset(new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration));
    physicsWorld->setInternalTickCallback(calculatePhysicsTick);
    controlsMangager.reset(new ControlsMangager());
    scriptManager.reset(new ScriptManager());
    sceneAmbient = btVector3(0.1, 0.1, 0.1);
}

void ObjectManager::gameTick() {
    //Calculate LightObjects
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->getTransformation().getOrigin();
    std::sort(lightObjects.begin(), lightObjects.end(), lightPrioritySorter);
    
    unsigned int maxShadows = 3;
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        lightObjects[i]->gameTick(i < maxShadows);
    
    currentShadowLight = NULL;
    currentShadowIsParabolid = false;
    profiler.leaveSection("Calculate shadows");
    
    //Draw not transparent
    mainCam->use();
    bool keepInColorBuffer = optionsState.screenBlurFactor > 0.0 || optionsState.edgeSmoothEnabled || optionsState.depthOfFieldQuality;
    drawFrame((keepInColorBuffer) ? mainFBO.gBuffers[colorDBuffer] : 0);
    
    profiler.leaveSection("Draw top frame");
    
    //Apply post effect shaders
    if(optionsState.ssaoQuality) {
        GLuint buffersSSAO[] = {
            mainFBO.gBuffers[depthDBuffer],
            mainFBO.gBuffers[ssaoDBuffer]
        };
        
        glViewport(0, 0, screenSize[0] >> screenSize[2], screenSize[1] >> screenSize[2]);
        shaderPrograms[ssaoSP]->use();
        mainFBO.renderInBuffers(true, buffersSSAO, 1, &buffersSSAO[1], 1);
        glViewport(0, 0, screenSize[0], screenSize[1]);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        shaderPrograms[ssaoCombineSP]->use();
        mainFBO.renderInBuffers(true, buffersSSAO, 2, &mainFBO.gBuffers[colorDBuffer], (keepInColorBuffer) ? 1 : 0);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        profiler.leaveSection("Apply SSAO");
    }
    
    GLuint buffersPostRenderer[] = {
        mainFBO.gBuffers[depthDBuffer],
        mainFBO.gBuffers[colorDBuffer]
    };
    
    if(optionsState.screenBlurFactor > 0.0) {
        shaderPrograms[blurSP]->use();
        currentShaderProgram->setUniformF("processingValue", optionsState.screenBlurFactor);
        mainFBO.renderInBuffers(true, &buffersPostRenderer[1], 1, 0, 0);
        profiler.leaveSection("Apply screen blur");
    }else{
        if(optionsState.edgeSmoothEnabled) {
            shaderPrograms[edgeSmoothSP]->use();
            mainFBO.renderInBuffers(true, buffersPostRenderer, 2, &buffersPostRenderer[1], (optionsState.depthOfFieldQuality) ? 1 : 0);
            profiler.leaveSection("Apply edge smooth");
        }
        
        if(optionsState.depthOfFieldQuality) {
            shaderPrograms[depthOfFieldSP]->use();
            mainFBO.renderInBuffers(true, buffersPostRenderer, 2, 0, 0);
            profiler.leaveSection("Apply depth of field");
        }
    }
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    
    //Calculate Physics
    controlsMangager->gameTick();
    physicsWorld->stepSimulation(profiler.animationFactor, 4, 1.0/60.0); //Try to maintain 60 FPS
    profiler.leaveSection("Calculate physics");
    
    //Calculate Decals
    for(auto iterator = decals.begin(); iterator != decals.end(); iterator ++) {
        (*iterator)->life -= profiler.animationFactor;
        if((*iterator)->life > 0.0) continue;
        delete *iterator;
        decals.erase(iterator);
        iterator --;
    }
    
    //Calculate GraphicObjects
    for(auto graphicObject : graphicObjects)
        graphicObject->gameTick();
    
    //Calculate SimpleObjects
    foreach_e(simpleObjects, iterator)
    (*iterator)->gameTick();
    profiler.leaveSection("Calculate objects");
    
    //Calculate ParticleSystems
    if(optionsState.particleCalcTarget == 2) glEnable(GL_RASTERIZER_DISCARD);
    foreach_e(particlesObjects, iterator)
    (*iterator)->gameTick();
    if(optionsState.particleCalcTarget == 2) glDisable(GL_RASTERIZER_DISCARD);
    profiler.leaveSection("Calculate particle systems");
}

void ObjectManager::physicsTick() {
    unsigned int numManifolds = collisionDispatcher->getNumManifolds();
    
    //printf(str, "Collisions: %d", numManifolds);
    
	for(unsigned int i = 0; i < numManifolds; i ++) {
		btPersistentManifold* contactManifold = collisionDispatcher->getManifoldByIndexInternal(i);
        if(contactManifold->getNumContacts() == 0) continue;
        
		const btCollisionObject *objectA = static_cast<const btCollisionObject*>(contactManifold->getBody0()),
        *objectB = static_cast<const btCollisionObject*>(contactManifold->getBody1());
        PhysicObject *userObjectA = static_cast<PhysicObject*>(objectA->getUserPointer()),
        *userObjectB = static_cast<PhysicObject*>(objectB->getUserPointer());
        
        //printf("%p %p (%p %p) %d\n", objectA, objectB, userObjectA, userObjectB, contactManifold->getNumContacts());
        
        userObjectA->handleCollision(contactManifold, userObjectB);
        userObjectB->handleCollision(contactManifold, userObjectA);
	}
    
    for(auto graphicObject : graphicObjects)
        graphicObject->physicsTick();
}

void ObjectManager::drawScene() {
    //Calculate Frustum Culling
    currentCam->doFrustumCulling();
    
    //Draw GraphicObjects
    glDisable(GL_BLEND);
    for(auto graphicObject : graphicObjects)
        if(graphicObject->inFrustum)
            graphicObject->draw();
    glEnable(GL_BLEND);
}

void ObjectManager::illuminate() {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        if(lightObjects[i]->inFrustum)
            lightObjects[i]->draw();
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_BLEND);
}

void ObjectManager::drawFrame(GLuint renderTarget) {
    GLuint buffersCombine[] = {
        mainFBO.gBuffers[diffuseDBuffer],
        mainFBO.gBuffers[materialDBuffer],
        (renderTarget) ? renderTarget : mainFBO.gBuffers[colorDBuffer],
        mainFBO.gBuffers[specularDBuffer],
        0
    };
    
    /* Render Mirrors
    for(auto graphicObject : graphicObjects) {
        ModelObject* modelObject = dynamic_cast<ModelObject*>(graphicObject);
        if(!modelObject)
            continue;
        for(Mesh* mesh : modelObject->model->meshes)
            if(mesh->material.reflectorNormal != btVector3(0.0, 0.0, 0.0)) {
                btTransform objectTransform = modelObject->getTransformation();
                
            }
    }*/
    
    //Draw non transparent
    mainFBO.renderInGBuffers(buffersCombine[2]);
    currentCam->updateFrustum();
    drawScene();
    
    //Draw Decals
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    for(auto decal : decals) {
        modelMat = decal->transformation;
        
        if(decal->diffuse)
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
        decalVAO.draw();
    }
    
    //Push ParticlesObjects in transparentAccumulator
    for(auto particlesObject : particlesObjects)
        if(particlesObject->inFrustum) {
            AccumulatedTransparent* transparent = new AccumulatedTransparent();
            transparent->object = particlesObject;
            transparent->mesh = NULL;
            transparentAccumulator.push_back(transparent);
        }
    
    //Illuminate non transparent
    illuminate();
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderInBuffers(true, buffersCombine, 4, &buffersCombine[2], (renderTarget || transparentAccumulator.size() > 0) ? 1 : 0);
    
    //Draw transparent
    if(transparentAccumulator.size() > 0) {
        buffersCombine[4] = buffersCombine[2];
        buffersCombine[2] = mainFBO.gBuffers[transparentDBuffer];
        
        btVector3 camMatPos = currentCam->getTransformation().getOrigin();
        std::sort(transparentAccumulator.begin(), transparentAccumulator.end(), [&camMatPos](AccumulatedTransparent* a, AccumulatedTransparent* b){
            return (a->object->getTransformation().getOrigin()-camMatPos).length() > (b->object->getTransformation().getOrigin()-camMatPos).length();
        });
        
        for(unsigned int i = 0; i < transparentAccumulator.size(); i ++) {
            mainFBO.renderInGBuffers(buffersCombine[2]);
            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
            
            AccumulatedTransparent* transparent = transparentAccumulator[i];
            if(optionsState.blendingQuality > 1) {
                glActiveTexture((transparent->mesh) ? GL_TEXTURE3 : GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_RECTANGLE, buffersCombine[4]);
            }
            
            if(transparent->mesh) {
                glDepthMask(GL_TRUE);
                static_cast<ModelObject*>(transparent->object)->drawAccumulatedMesh(transparent->mesh);
            }else{
                glDepthMask(GL_FALSE);
                static_cast<ParticlesObject*>(transparent->object)->draw();
            }
            delete transparent;
            
            illuminate();
            shaderPrograms[deferredCombineTransparentSP]->use();
            mainFBO.renderInBuffers(true,
                                    buffersCombine, (optionsState.blendingQuality == 1) ? 5 : 4,
                                    &buffersCombine[4], 1);
        }
        if(!renderTarget)
            mainFBO.copyBuffer(buffersCombine[4], 0);
    }
    transparentAccumulator.clear();
}

void ObjectManager::updateRendererSettings() {
    shaderPrograms[deferredCombineSP]->use();
    shaderPrograms[deferredCombineSP]->setUniformVec3("sceneAmbient", sceneAmbient);
    shaderPrograms[deferredCombineTransparentSP]->use();
    shaderPrograms[deferredCombineTransparentSP]->setUniformVec3("sceneAmbient", sceneAmbient);
}

ObjectManager objectManager;