//
//  ObjectManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//
//

#include "Controls.h"

ALCdevice* soundDevice;
ALCcontext* soundContext;

VertexArrayObject decalVAO;
unsigned char inBuffersSSAO[] = { depthDBuffer, positionDBuffer, ssaoDBuffer }, outBuffersSSAO[] = { ssaoDBuffer };
unsigned char inBuffersCombine[] = { colorDBuffer, diffuseDBuffer, specularDBuffer, materialDBuffer }, outBuffersCombine[] = { colorDBuffer };
unsigned char inBuffersPost[] = { depthDBuffer, colorDBuffer }, outBuffersPost[] = { colorDBuffer };

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
    physicsWorld = NULL;
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

void ObjectManager::initPhysics() {
    physicsWorld = new btDiscreteDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration);
    physicsWorld->setInternalTickCallback(calculatePhysicsTick);
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
    
    if(physicsWorld) {
        delete physicsWorld;
        physicsWorld = NULL;
    }
}

void ObjectManager::gameTick() {
    //Calculate Physics
    physicsWorld->stepSimulation(animationFactor, 4, 1.0/60.0); //Try to maintain 60 FPS
    
    //Calculate Decals
    for(auto iterator = decals.begin(); iterator != decals.end(); iterator ++) {
        (*iterator)->life -= animationFactor;
        if((*iterator)->life > 0.0) continue;
        delete *iterator;
        decals.erase(iterator);
        iterator --;
    }
    
    //Calculate GraphicObjects
    for(auto graphicObject : graphicObjects)
        graphicObject->gameTick();
    
    //Calculate ParticleSystems
    if(particleCalcTarget == 2) glEnable(GL_RASTERIZER_DISCARD);
    for(auto iterator = particlesObjects.begin(); iterator != particlesObjects.end(); iterator ++) {
        if((*iterator)->gameTick()) continue;
        particlesObjects.erase(iterator);
        iterator --;
    }
    if(particleCalcTarget == 2) glDisable(GL_RASTERIZER_DISCARD);
    
    //Calculate LightObjects
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->getTransformation().getOrigin();
    std::sort(lightObjects.begin(), lightObjects.end(), lightPrioritySorter);
    
    unsigned int maxShadows = shadowQuality;
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        lightObjects[i]->gameTick(i < maxShadows);
    currentShadowLight = NULL;
    
    //Calculate SimpleObjects
    for(auto iterator = simpleObjects.begin(); iterator != simpleObjects.end(); iterator ++) {
        if((*iterator)->gameTick()) continue;
        simpleObjects.erase(iterator);
        iterator --;
    }
}

void ObjectManager::physicsTick() {
    unsigned int numManifolds = collisionDispatcher->getNumManifolds();
    
    //TODO: Debugging
    if(currentMenu == inGameMenu) {
        char str[64];
        sprintf(str, "Collisions: %d", numManifolds);
        GUILabel* label = static_cast<GUILabel*>(currentScreenView->children[1]);
        label->text = str;
        label->updateContent();
    }
    
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
    //Reset Frustum Flags
    for(auto graphicObject : graphicObjects)
        graphicObject->inFrustum = false;
    
    for(auto particlesObject : particlesObjects)
        particlesObject->inFrustum = false;
    
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        lightObjects[i]->inFrustum = false;
    
    //Calculate Frustum Culling
    if(currentShadowLight)
        currentCam->doFrustumCulling(CollisionMask_Static | CollisionMask_Object);
    else
        currentCam->doFrustumCulling(CollisionMask_Light | CollisionMask_Static | CollisionMask_Object);
    
    //Draw GraphicObjects
    glDisable(GL_BLEND);
    for(auto graphicObject : graphicObjects)
        if(graphicObject->inFrustum)
            graphicObject->draw();
    glEnable(GL_BLEND);
    
    //Draw Decals
    if(currentShadowLight) return;
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
    
    //Draw Particle Systems
    if(particleCalcTarget == 0) return;
    for(auto particlesObject : particlesObjects)
        if(particlesObject->inFrustum)
            particlesObject->draw();
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

void ObjectManager::drawFrame() {
    mainFBO.renderInDeferredBuffers(false);
    mainCam->use();
    currentCam->updateAudioListener();
    drawScene();
    
    //Calculate Screen Blur
    if(screenBlurFactor > -1.0) {
        float speed = animationFactor*5.0;
        if(currentMenu == inGameMenu) {
            screenBlurFactor -= min(screenBlurFactor*speed, speed);
            if(screenBlurFactor < 0.01) screenBlurFactor = 0.0;
        }else
            screenBlurFactor += min((2-screenBlurFactor)*speed, speed);
    }
    
    //Draw LightObjects
    illuminate();
    
    //Draw Transparent
    bool keepInColorBuffer = screenBlurFactor > 0.0 || edgeSmoothEnabled || depthOfFieldQuality;
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(true, inBuffersCombine, 4, outBuffersCombine,
                           (objectManager.transparentAccumulator.size() > 0 || keepInColorBuffer) ? 1 : 0);
    mainFBO.renderTransparentInDeferredBuffers(keepInColorBuffer);
    
    if(ssaoQuality) {
        glViewport(0, 0, screenSize[0] >> screenSize[2], screenSize[1] >> screenSize[2]);
        shaderPrograms[ssaoSP]->use();
        mainFBO.renderDeferred(true, inBuffersSSAO, 1, outBuffersSSAO, 1);
        glViewport(0, 0, screenSize[0], screenSize[1]);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        shaderPrograms[ssaoCombineSP]->use();
        mainFBO.renderDeferred(true, &inBuffersSSAO[1], 2, outBuffersCombine, (keepInColorBuffer) ? 1 : 0);
        glDisable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    
    if(screenBlurFactor > 0.0) {
        shaderPrograms[blurSP]->use();
        currentShaderProgram->setUniformF("processingValue", screenBlurFactor);
        mainFBO.renderDeferred(true, inBuffersCombine, 1, outBuffersCombine, 0);
    }else{
        if(edgeSmoothEnabled) {
            shaderPrograms[edgeSmoothSP]->use();
            mainFBO.renderDeferred(true, inBuffersPost, 2, outBuffersPost, (depthOfFieldQuality) ? 1 : 0);
        }
        
        if(depthOfFieldQuality) {
            shaderPrograms[depthOfFieldSP]->use();
            mainFBO.renderDeferred(true, inBuffersPost, 2, outBuffersPost, 0);
        }
    }
    
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
}

float animationFactor;
ObjectManager objectManager;