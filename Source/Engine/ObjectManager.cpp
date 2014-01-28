//
//  ObjectManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.11.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

ALCdevice* soundDevice;
ALCcontext* soundContext;

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
    collisionConfiguration = new btSoftBodyRigidBodyCollisionConfiguration();
    collisionDispatcher = new btCollisionDispatcher(collisionConfiguration);
    constraintSolver = new btSequentialImpulseConstraintSolver();
    
    //softBodySolver = NULL;
    softBodySolver = new btDefaultSoftBodySolver();
    softBodyOutput = NULL;
    broadphase = new btDbvtBroadphase();
}

ObjectManager::~ObjectManager() {
    alcDestroyContext(soundContext);
    alcCloseDevice(soundDevice);
    
    delete broadphase;
    delete softBodySolver;
    if(softBodyOutput) delete softBodyOutput;
    delete constraintSolver;
    delete collisionDispatcher;
    delete collisionConfiguration;
}

bool ObjectManager::initOpenCL() {
    cl_uint platformCount;
	cl_int clError = clGetPlatformIDs(0, NULL, &platformCount);
    if(!softBodySolver && !softBodyOutput && clError == CL_SUCCESS && platformCount > 0) {
		cl_platform_id* platforms = new cl_platform_id[platformCount];
		clError = clGetPlatformIDs(platformCount, platforms, NULL);
        
        for(int i = 0; i < platformCount; i ++) {
			char name[128], vendor[128], version[128];
			clError = clGetPlatformInfo(platforms[i], CL_PLATFORM_NAME, sizeof(name), name, NULL);
            clError = clGetPlatformInfo(platforms[i], CL_PLATFORM_VENDOR, sizeof(vendor), vendor, NULL);
            clError = clGetPlatformInfo(platforms[i], CL_PLATFORM_VERSION, sizeof(version), version, NULL);
            log(info_log, std::string("OpenCL Platform: ")+name+", "+vendor+", "+version);
            
            cl_uint num_entries = 16;
            cl_device_id devices[16];
            cl_uint computeUnitCount;
            cl_uint devicesCount = -1;
            
            clError = clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_CPU, num_entries, devices, &devicesCount);
            for(int j = 0; j < devicesCount; j ++) {
                clGetDeviceInfo(devices[j], CL_DEVICE_NAME, sizeof(name), name, NULL);
                clGetDeviceInfo(devices[j], CL_DEVICE_VENDOR, sizeof(vendor), vendor, NULL);
                clGetDeviceInfo(devices[j], CL_DRIVER_VERSION, sizeof(version), version, NULL);
                clGetDeviceInfo(devices[j], CL_DEVICE_MAX_COMPUTE_UNITS, sizeof(computeUnitCount), &computeUnitCount, NULL);
                log(info_log, std::string("OpenCL Device: ")+name+", "+vendor+", "+version+", "+stringOf(computeUnitCount)+" Cores");
            }
            
            cl_context_properties cps[7] = {
                CL_CONTEXT_PLATFORM, (cl_context_properties)platforms[i], 0
                /*CL_GL_CONTEXT_KHR, (cl_context_properties)pGLContext,
                CL_WGL_HDC_KHR, (cl_context_properties)pGLDC,
                0*/
            };
            cl_context context = clCreateContext(cps, 1, &devices[0], NULL, NULL, &clError);
            cl_command_queue commandQue = clCreateCommandQueue(context, devices[0], 0, &clError);
            softBodySolver = new btOpenCLSoftBodySolverSIMDAware(commandQue, context);
            softBodyOutput = new btSoftBodySolverOutputCLtoGL(commandQue, context);
		}
        
        delete [] platforms;
        return false;
    }else
        return false;
}

void ObjectManager::init() {
    //Init VAOs
    initLightVolumes();
    
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
    rectVAO.init(attributes, false);
    rectVAO.updateVertices(32, vertices, GL_STATIC_DRAW);
    rectVAO.indeciesCount = 4;
    rectVAO.drawType = GL_TRIANGLE_STRIP;
    
    //Init OpenCL
    initOpenCL();
    
    //Init OpenAL
    soundDevice = alcOpenDevice(NULL);
    soundContext = alcCreateContext(soundDevice, NULL);
    alcMakeContextCurrent(soundContext);
    alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED);
    printf("OpenAL sound output: %s\n", alcGetString(soundDevice, ALC_DEVICE_SPECIFIER));
    
    //Show loading screen
    guiCam = new CamObject();
    loadStaticShaderPrograms();
    menu.setLoadingMenu();
}

void ObjectManager::clear() {
    currentShadowLight = NULL;
    
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        lightObjects[i]->removeFast();
    lightObjects.clear();
    
    for(unsigned int i = 0; i < transparentAccumulator.size(); i ++)
        delete transparentAccumulator[i];
    transparentAccumulator.clear();
    
    for(auto matterObject : matterObjects)
        matterObject->removeFast();
    matterObjects.clear();
    
    for(auto particlesObject : particlesObjects)
        particlesObject->removeFast();
    particlesObjects.clear();
    
    for(auto simpleObject : simpleObjects)
        simpleObject->removeFast();
    simpleObjects.clear();
    
    physicsWorld.reset();
    mainCam = NULL;
}

void ObjectManager::initGame() {
    clear();
    physicsWorld.reset(new btSoftRigidDynamicsWorld(collisionDispatcher, broadphase, constraintSolver, collisionConfiguration, softBodySolver));
    physicsWorld->setInternalTickCallback(calculatePhysicsTick);
    sceneAmbient = btVector3(0.1, 0.1, 0.1);
    sceneFogColor = btVector3(0.8, 0.8, 0.8);
    sceneFogDistance = 0.0;
    levelManager.gameStatus = localGame; //Enable console log
}

void ObjectManager::gameTick() {
    glDepthMask(GL_TRUE);
    glEnable(GL_DEPTH_TEST);
    
    //Calculate LightObjects
    LightPrioritySorter lightPrioritySorter;
    lightPrioritySorter.position = mainCam->getTransformation().getOrigin();
    std::sort(lightObjects.begin(), lightObjects.end(), lightPrioritySorter);
    
    unsigned int maxShadows = (optionsState.shadowQuality > 0) ? 3 : 0;
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        if(!lightObjects[i]->updateShadowMap(i < maxShadows))
            i --;
    
    currentShadowLight = NULL;
    currentShadowIsParabolid = false;
    profiler.leaveSection("Calculate shadows");
    
    //Draw scene
    mainCam->use();
    bool keepInColorBuffer = optionsState.screenBlurFactor > 0.0 || optionsState.edgeSmoothEnabled ||
                             sceneFogDistance > 0.0 || optionsState.depthOfFieldQuality;
    drawFrame((keepInColorBuffer) ? mainFBO.gBuffers[colorDBuffer] : 0);
    profiler.leaveSection("Draw top frame");
    
    //Apply post effect shaders
    if(optionsState.ssaoQuality) {
        GLuint buffersSSAO[] = {
            mainFBO.gBuffers[normalDBuffer],
            mainFBO.gBuffers[depthDBuffer],
            mainFBO.gBuffers[ssaoDBuffer]
        };
        
        glDisable(GL_BLEND);
        shaderPrograms[ssaoSP]->use();
        glViewport(0, 0, optionsState.videoWidth, optionsState.videoHeight);
        mainFBO.renderInBuffers(true, buffersSSAO, 2, &buffersSSAO[2], 1);
        glViewport(0, 0, optionsState.videoWidth*optionsState.videoScale, optionsState.videoHeight*optionsState.videoScale);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_DST_COLOR, GL_ZERO);
        shaderPrograms[ssaoCombineSP]->use();
        mainFBO.renderInBuffers(true, &buffersSSAO[1], 2, &mainFBO.gBuffers[colorDBuffer], (keepInColorBuffer) ? 1 : 0);
        glDisable(GL_BLEND);
        
        profiler.leaveSection("Apply SSAO");
    }
    
    GLuint buffersPostRenderer[] = {
        mainFBO.gBuffers[depthDBuffer],
        mainFBO.gBuffers[colorDBuffer]
    };
    
    if(optionsState.screenBlurFactor > 0.0) {
        glDisable(GL_BLEND);
        shaderPrograms[blurSP]->use();
        currentShaderProgram->setUniformF("processingValue", optionsState.screenBlurFactor * optionsState.videoScale);
        mainFBO.renderInBuffers(true, &buffersPostRenderer[1], 1, 0, 0);
        profiler.leaveSection("Apply screen blur");
    }else{
        glDisable(GL_BLEND);
        
        if(optionsState.edgeSmoothEnabled) {
            shaderPrograms[edgeSmoothSP]->use();
            mainFBO.renderInBuffers(true, buffersPostRenderer, 2, &buffersPostRenderer[1],
                                    (sceneFogDistance > 0.0 || optionsState.depthOfFieldQuality) ? 1 : 0);
            profiler.leaveSection("Apply edge smooth");
        }
        
        if(sceneFogDistance > 0.0) {
            shaderPrograms[depthOfFieldFogSP]->use();
            currentShaderProgram->setUniformVec3("fogColor", sceneFogColor);
            currentShaderProgram->setUniformF("fogDistance", sceneFogDistance);
            mainFBO.renderInBuffers(true, buffersPostRenderer, 2, 0, 0);
            profiler.leaveSection("Apply depth of field and fog");
        }else if(optionsState.depthOfFieldQuality) {
            shaderPrograms[depthOfFieldSP]->use();
            mainFBO.renderInBuffers(true, buffersPostRenderer, 2, 0, 0);
            profiler.leaveSection("Apply depth of field");
        }
    }
    
    //Calculate Physics
    physicsWorld->stepSimulation(profiler.animationFactor, 4, 1.0/60.0); //Try to maintain 60 FPS
    profiler.leaveSection("Calculate physics");
    
    //Calculate MatterObjects
    foreach_e(matterObjects, iterator)
        (*iterator)->gameTick();
    
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
    
    //Script Animations
    scriptManager->gameTick();
    
    levelManager.mainScript->callFunction("ongametick", false, { });
    profiler.leaveSection("Execute script: ongametick()");
}

void ObjectManager::physicsTick() {
    unsigned int numManifolds = collisionDispatcher->getNumManifolds();
    
    //printf(str, "Collisions: %d", numManifolds);
    
	for(unsigned int i = 0; i < numManifolds; i ++) {
		btPersistentManifold* contactManifold = collisionDispatcher->getManifoldByIndexInternal(i);
        if(contactManifold->getNumContacts() == 0) continue;
        
		const btCollisionObject *objectA = contactManifold->getBody0(),
                                *objectB = contactManifold->getBody1();
        PhysicObject *userObjectA = static_cast<PhysicObject*>(objectA->getUserPointer()),
                     *userObjectB = static_cast<PhysicObject*>(objectB->getUserPointer());
        
        //printf("%p %p (%p %p) %d\n", objectA, objectB, userObjectA, userObjectB, contactManifold->getNumContacts());
        
        userObjectA->handleCollision(contactManifold, userObjectB);
        userObjectB->handleCollision(contactManifold, userObjectA);
	}
    
    levelManager.mainScript->callFunction("onphysicstick", false, { });
}

void ObjectManager::drawShadowCasters() {
    currentCam->doFrustumCulling();
    
    //Draw MatterObjects
    glDisable(GL_BLEND);
    for(auto matterObject : matterObjects)
        if(matterObject->inFrustum)
            matterObject->draw();
}

void ObjectManager::illuminate() {
    glDepthMask(GL_FALSE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE, GL_ONE);
    for(unsigned int i = 0; i < lightObjects.size(); i ++)
        if(lightObjects[i]->inFrustum)
            lightObjects[i]->draw();
    glFrontFace(GL_CCW);
    glDisable(GL_DEPTH_TEST);
}

void ObjectManager::drawFrame(GLuint renderTarget) {
    PlaneReflective* planeReflective = dynamic_cast<PlaneReflective*>(objectManager.currentReflective);
    
    //Render Mirrors
    if(optionsState.blendingQuality > 2) {
        if(!currentReflective) {
            for(auto matterObject : matterObjects) {
                RigidObject* rigidObject = dynamic_cast<RigidObject*>(matterObject);
                if(!rigidObject || !rigidObject->model)
                    continue;
                for(Mesh* mesh : rigidObject->model->meshes)
                    if(mesh->material.reflectivity == 0.0) {
                        continue;
                    }else if(mesh->material.reflectivity == -1.0) {
                        auto iterator = reflectiveAccumulator.find(rigidObject);
                        if(iterator == reflectiveAccumulator.end()) {
                            PlaneReflective* reflective = new PlaneReflective(rigidObject, mesh);
                            if(reflective->gameTick())
                                reflectiveAccumulator[rigidObject] = reflective;
                        }else if(!static_cast<PlaneReflective*>(iterator->second)->gameTick()) {
                            delete iterator->second;
                            reflectiveAccumulator.erase(iterator);
                        }
                        break;
                    }else{
                        //Environment Mapping
                        break;
                    }
            }
            currentReflective = NULL;
            currentCam->doFrustumCulling();
        }
    }else
        currentCam->doFrustumCulling();
    
    //Draw non transparent
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glFrontFace((planeReflective) ? GL_CW : GL_CCW);
    GLuint buffersCombine[] = {
        mainFBO.gBuffers[diffuseDBuffer],
        (renderTarget) ? renderTarget : mainFBO.gBuffers[colorDBuffer],
        mainFBO.gBuffers[specularDBuffer],
        0
    };
    mainFBO.renderInGBuffers(buffersCombine[1], true);
    currentCam->updateViewMat();
    
    //Push ParticlesObjects in transparentAccumulator
    if(optionsState.blendingQuality > 0)
        for(auto particlesObject : particlesObjects)
            if(particlesObject->inFrustum) {
                AccumulatedTransparent* transparent = new AccumulatedTransparent();
                transparent->object = particlesObject;
                transparent->mesh = NULL;
                transparentAccumulator.push_back(transparent);
            }
    
    //Draw MatterObjects
    for(auto matterObject : matterObjects)
        if(matterObject->inFrustum)
            matterObject->draw();
    
    //Illuminate non transparent
    illuminate();
    glDisable(GL_BLEND);
    shaderPrograms[deferredCombineSP0]->use();
    mainFBO.renderInBuffers(true, buffersCombine, 3, &buffersCombine[1], (renderTarget || transparentAccumulator.size() > 0) ? 1 : 0);
    
    //Draw transparent
    if(transparentAccumulator.size() > 0) {
        buffersCombine[3] = buffersCombine[1];
        buffersCombine[1] = mainFBO.gBuffers[transparentDBuffer];
        
        btVector3 camMatPos = currentCam->getTransformation().getOrigin();
        std::sort(transparentAccumulator.begin(), transparentAccumulator.end(), [&camMatPos](AccumulatedTransparent* a, AccumulatedTransparent* b) {
            return (a->object->getTransformation().getOrigin()-camMatPos).length() > (b->object->getTransformation().getOrigin()-camMatPos).length();
        });
        
        for(unsigned int i = 0; i < transparentAccumulator.size(); i ++) {
            AccumulatedTransparent* transparent = transparentAccumulator[i];
            ShaderProgramName sp = deferredCombine1SP;
            
            mainFBO.renderInGBuffers(buffersCombine[1], transparent->mesh != NULL);
            glEnable(GL_DEPTH_TEST);
            if(transparent->mesh) {
                if(optionsState.blendingQuality > 1) {
                    glActiveTexture(GL_TEXTURE3);
                    glBindTexture(GL_TEXTURE_RECTANGLE, buffersCombine[3]);
                    sp = deferredCombine2SP;
                }
                glDisable(GL_BLEND);
                glDepthMask(GL_TRUE);
                glFrontFace((planeReflective) ? GL_CW : GL_CCW);
                static_cast<RigidObject*>(transparent->object)->drawAccumulatedMesh(transparent->mesh);
            }else{
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
                glDepthMask(GL_FALSE);
                glFrontFace(GL_CCW);
                static_cast<ParticlesObject*>(transparent->object)->draw();
            }
            delete transparent;
            
            illuminate();
            if(sp == deferredCombine1SP) {
                glEnable(GL_BLEND);
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
            }else
                glDisable(GL_BLEND);
            shaderPrograms[sp]->use();
            mainFBO.renderInBuffers(true, buffersCombine, 3, &buffersCombine[3], 1);
        }
        if(!renderTarget)
            mainFBO.copyBuffer(buffersCombine[3], 0);
    }
    transparentAccumulator.clear();
}

void ObjectManager::updateRendererSettings() {
    for(unsigned int i = 0; i < 3; i ++) {
        shaderPrograms[deferredCombineSP0+i]->use();
        shaderPrograms[deferredCombineSP0+i]->setUniformVec3("sceneAmbient", sceneAmbient);
    }
}

ObjectManager objectManager;