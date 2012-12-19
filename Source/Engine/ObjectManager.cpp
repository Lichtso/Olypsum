//
//  ObjectManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//
//

#import "Menu.h"

ALCdevice* soundDevice;
ALCcontext* soundContext;

GLuint decalVBO;
Texture randomNormalMap;
unsigned char inBuffersSSAO[] = { depthDBuffer }, outBuffersSSAO[] = { ssaoDBuffer };
unsigned char inBuffersCombine[] = { colorDBuffer, diffuseDBuffer, specularDBuffer, materialDBuffer, normalDBuffer, ssaoDBuffer }, outBuffersCombine[] = { colorDBuffer };
unsigned char inBuffersPost[] = { depthDBuffer, colorDBuffer }, outBuffersPost[] = { colorDBuffer };

ObjectManager::ObjectManager() {
    currentShadowLight = NULL;
}

ObjectManager::~ObjectManager() {
    alcDestroyContext(soundContext);
    alcCloseDevice(soundDevice);
    clear();
}

void ObjectManager::init() {
    //Init Decals
    float vertices[] = {
        -1.0, -1.0, 0.0, 0.0, 1.0, 0.0, 0.0, 1.0,
        1.0, -1.0, 0.0, 1.0, 1.0, 0.0, 0.0, 1.0,
        -1.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 1.0,
        1.0, 1.0, 0.0, 1.0, 0.0, 0.0, 0.0, 1.0
    };
    glGenBuffers(1, &decalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, decalVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //Init Lights
    randomNormalMap.width = 128;
    randomNormalMap.height = 128;
    randomNormalMap.loadRandom();
    randomNormalMap.uploadTexture(GL_TEXTURE_2D, GL_COMPRESSED_RGB);
    
    //Init Sound
    soundDevice = alcOpenDevice(NULL);
    soundContext = alcCreateContext(soundDevice, NULL);
    alcMakeContextCurrent(soundContext);
    log(info_log, std::string("OpenAL, sound output ")+alcGetString(soundDevice, ALC_DEVICE_SPECIFIER));
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
}

void ObjectManager::gameTick() {
    //Calculate Decals
    for(auto iterator = decals.begin(); iterator != decals.end(); iterator ++) {
        (*iterator)->life -= worldManager.animationFactor;
        if((*iterator)->life > 0.0) continue;
        delete *iterator;
        decals.erase(iterator);
        iterator --;
    }
    
    //Calculate GraphicObjects
    for(auto graphicObject : graphicObjects)
        graphicObject->gameTick();
    
    //Calculate ParticleSystems
    if(particleCalcTarget == 2) glEnable(GL_RASTERIZER_DISCARD_EXT);
    for(auto iterator = particlesObjects.begin(); iterator != particlesObjects.end(); iterator ++) {
        if((*iterator)->gameTick()) continue;
        particlesObjects.erase(iterator);
        iterator --;
    }
    if(particleCalcTarget == 2) glDisable(GL_RASTERIZER_DISCARD_EXT);
    
    //Calculate LightObjects
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = currentCam->getTransformation().getOrigin();
    std::sort(lightObjects.begin(), lightObjects.end(), lightPrioritySorter);
    
    unsigned int maxShadows = 1; //TODO
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
    btDispatcher* dispatcher = worldManager.physicsWorld->getDispatcher();
    unsigned int numManifolds = dispatcher->getNumManifolds();
	for(unsigned int i = 0; i < numManifolds; i ++) {
		btPersistentManifold* contactManifold = dispatcher->getManifoldByIndexInternal(i);
        if(contactManifold->getNumContacts() == 0) continue;
        
		void *objectA = static_cast<const btCollisionObject*>(contactManifold->getBody0())->getUserPointer(),
             *objectB = static_cast<const btCollisionObject*>(contactManifold->getBody1())->getUserPointer();
        PhysicObject *userObjectA = static_cast<PhysicObject*>(objectA),
                     *userObjectB = static_cast<PhysicObject*>(objectB);
        
        if(userObjectA) userObjectA->handleCollision(contactManifold, userObjectB);
        if(userObjectB) userObjectB->handleCollision(contactManifold, userObjectA);
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
    for(auto graphicObject : graphicObjects)
        if(graphicObject->inFrustum)
            graphicObject->draw();
    
    //Draw Decals
    if(currentShadowLight) return;
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindBuffer(GL_ARRAY_BUFFER, decalVBO);
    unsigned int byteStride = 8*sizeof(float);
    shaderPrograms[solidGeometrySP]->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(0*sizeof(float)));
    currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(3*sizeof(float)));
    currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(5*sizeof(float)));
    for(auto decal : decals) {
        modelMat = decal->transformation;
        
        if(decal->diffuse)
            decal->diffuse->use(GL_TEXTURE_2D, 0);
        
        if(decal->heightMap) {
            decal->heightMap->use(GL_TEXTURE_2D, 2);
            shaderPrograms[solidBumpGeometrySP]->use();
        }else{
            glActiveTexture(GL_TEXTURE2);
            glBindTexture(GL_TEXTURE_2D, 0);
            shaderPrograms[solidGeometrySP]->use();
        }
        
        currentShaderProgram->setUniformF("discardDensity", min(1.0F, decal->life));
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    }
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    
    //Draw Particle Systems
    if(particleCalcTarget == 0) return;
    modelMat.setIdentity();
    shaderPrograms[particleDrawSP]->use();
    glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);
    glDepthMask(GL_FALSE);
    for(auto particlesObject : particlesObjects)
        if(particlesObject->inFrustum)
            particlesObject->draw();
    glDepthMask(GL_TRUE);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_TRANSFORM_FEEDBACK_BUFFER_EXT, 0);
    glDisable(GL_VERTEX_PROGRAM_POINT_SIZE);
}

void ObjectManager::illuminate() {
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        if(lightObjects[i]->inFrustum)
            lightObjects[i]->draw();
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
}

void ObjectManager::drawFrame() {
    mainFBO.renderInDeferredBuffers(false);
    mainCam->use();
    drawScene();
    
    //Calculate Screen Blur
    if(screenBlurFactor > -1.0) {
        float speed = worldManager.animationFactor*5.0;
        if(currentMenu == inGameMenu) {
            screenBlurFactor -= min(screenBlurFactor*speed, speed);
            if(screenBlurFactor < 0.01) screenBlurFactor = 0.0;
        }else
            screenBlurFactor += min((2-screenBlurFactor)*speed, speed);
    }
    
    //Draw LightObjects
    glDisable(GL_BLEND);
    glDepthMask(GL_FALSE);
    glDisable(GL_DEPTH_TEST);
    illuminate();
    
    if(ssaoQuality) {
        glViewport(0, 0, screenSize[0] >> 1, screenSize[1] >> 1);
        randomNormalMap.use(GL_TEXTURE_2D, 1);
        shaderPrograms[ssaoSP]->use();
        mainFBO.renderDeferred(true, inBuffersSSAO, 1, outBuffersSSAO, 1);
        glViewport(0, 0, screenSize[0], screenSize[1]);
    }
    
    shaderPrograms[deferredCombineSP]->use();
    mainFBO.renderDeferred(true, inBuffersCombine, (ssaoQuality) ? 6 : 4, outBuffersCombine,
                           (objectManager.transparentAccumulator.size() > 0 || screenBlurFactor > 0.0 || edgeSmoothEnabled || depthOfFieldQuality) ? 1 : 0);
    mainFBO.renderTransparentInDeferredBuffers();
    
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
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDepthMask(GL_TRUE);
    glEnable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
}

ObjectManager objectManager;