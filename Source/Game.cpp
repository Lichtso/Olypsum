//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Game.h"

GUILabel* labelFPS;
GUISlider* soundTrackSlider;
Model* humanModel;
SkeletonPose* skeletonPose;
static float animationTime = 0;

static void updateSoundTrackSlider(GUISlider* slider) {
    soundSourcesManager.soundSources[0]->setTimeOffset(slider->value * soundSourcesManager.soundSources[0]->soundTrack->getLength());
}

void initGame() {
    humanModel = fileManager.getPackage(NULL)->getModel("man.dae");
    skeletonPose = new SkeletonPose(humanModel->skeleton);
    
    if(false) {
        DirectionalLight* light = new DirectionalLight();
        light->direction = Vector3(0.0, -1.0, 0.0).normalize();
        light->distance = 50.0;
        light->range = 100.0;
        light->width = light->height = 2.0;
        lightManager.lights.push_back((Light*)light);
    }
    
    if(false) {
        SpotLight* lightB = new SpotLight();
        lightB->position = Vector3(-2.0, 3.0, 3.5);
        lightB->direction = Vector3(0.5, -0.5, -1.0).normalize();
        lightB->cutoff = 20.0/180.0*M_PI;
        lightB->range = 10.0;
        lightManager.lights.push_back((Light*)lightB);
    }
    
    if(true) {
        PositionalLight* lightC = new PositionalLight();
        lightC->position = Vector3(-1.0, 3.0, 1.0);
        lightC->direction = Vector3(1.0, 0.0, 0.0).normalize();
        lightC->range = 10.0;
        lightManager.lights.push_back((Light*)lightC);
    }
    
    GUIImage* image = new GUIImage();
    image->texture = fileManager.getPackage(NULL)->getTexture("logo.png");
    image->sizeAlignment = GUISizeAlignment_Height;
    image->width = 400;
    image->posY = -350;
    currentScreenView->addChild(image);
    
    GUIFramedView* view = new GUIFramedView();
    view->width = 265;
    view->height = 50;
    view->posY = 450;
    currentScreenView->addChild(view);
    
    GUILabel* label = new GUILabel();
    label->text = std::string("Dj Bjra - Military Storm");
    label->posX = -30;
    label->posY = 10;
    label->width = 200;
    label->sizeAlignment = GUISizeAlignment_Height;
    label->textAlign = GUITextAlign_Left;
    view->addChild(label);
    
    labelFPS = new GUILabel();
    labelFPS->text = std::string("FPS");
    labelFPS->posX = 220;
    labelFPS->posY = 10;
    labelFPS->width = 100;
    labelFPS->sizeAlignment = GUISizeAlignment_Height;
    labelFPS->textAlign = GUITextAlign_Left;
    view->addChild(labelFPS);
    
    soundTrackSlider = new GUISlider();
    soundTrackSlider->width = 250;
    soundTrackSlider->posY = -25;
    soundTrackSlider->onChange = updateSoundTrackSlider;
    view->addChild(soundTrackSlider);
    
    ParticleSystem* particleSystem = new ParticleSystem();
    particleSystem->position = Vector3(3, 1, -3);
    particleSystem->posMin = Vector3(-0.1, 0, -0.1);
    particleSystem->posMax = Vector3(0.1, 0, 0.1);
    particleSystem->sizeMin = 0.05;
    particleSystem->sizeMax = 0.1;
    particleSystem->lifeMin = 3.0;
    particleSystem->lifeMax = 4.0;
    particleSystem->addMin = 20.0;
    particleSystem->addMax = 30.0;
    particleSystem->dirMin = Vector3(-0.1, 1.4, -0.1);
    particleSystem->dirMax = Vector3(0.1, 1.8, 0.1);
    particleSystem->texture = fileManager.getPackage(NULL)->getTexture("man.png");
    
    SoundSource* testSoundSource = new SoundSource();
    testSoundSource->setSoundTrack(fileManager.getPackage(NULL)->getSoundTrack("test.ogg"));
    testSoundSource->looping = true;
    //testSoundSource->play();
}

static void addVertex(float* vertices, float x, float y) {
    vertices[6] = x;
    vertices[7] = y;
    x -= 0.5;
    y -= 0.5;
    vertices[0] = x*10.0;
    vertices[1] = 0.0;
    vertices[2] = y*10.0;
    vertices[3] = 0.0;
    vertices[4] = 1.0;
    vertices[5] = 0.0;
};

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelMat.setIdentity();
    modelMat.rotateY(0.4);
    modelMat.translate(Vector3(0.0, sin(animationTime)*0.5+0.15, 0.0));
    skeletonPose->bonePoses["Back"]->poseMat = modelMat;
    float thighRotX = (1.0-sin(animationTime))*0.6,
    shinRotX = -0.8*(1.0-sin(animationTime)),
    footRotX = 0.4-0.8*(1.0+sin(animationTime));
    skeletonPose->bonePoses["Thigh_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Thigh_Right"]->poseMat.rotateX(thighRotX);
    skeletonPose->bonePoses["Thigh_Left"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Thigh_Left"]->poseMat.rotateX(thighRotX);
    skeletonPose->bonePoses["Shin_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Shin_Right"]->poseMat.rotateX(shinRotX);
    skeletonPose->bonePoses["Shin_Left"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Shin_Left"]->poseMat.rotateX(shinRotX);
    skeletonPose->bonePoses["Foot_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Foot_Right"]->poseMat.rotateX(footRotX);
    skeletonPose->bonePoses["Foot_Left"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Foot_Left"]->poseMat.rotateX(footRotX);
    skeletonPose->bonePoses["Fingers1_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Fingers1_Right"]->poseMat.rotateZ(-1.3*(cos(animationTime)*0.5+0.5));
    skeletonPose->bonePoses["Fingers2_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Fingers2_Right"]->poseMat.rotateZ(-1.3*(cos(animationTime)*0.5+0.5));
    skeletonPose->calculateBonePose(humanModel->skeleton->rootBone, NULL);
    humanModel->draw(1.0, skeletonPose);
    
    unsigned int size = 10, index;
    float vertices[48*size*size];
    for(unsigned int y = 0; y < size; y ++)
        for(unsigned int x = 0; x < size; x ++) {
            index = (x+y*size)*48;
            addVertex(&vertices[index], (float)x/size, (float)(y+1)/size);
            addVertex(&vertices[index+8], (float)(x+1)/size, (float)y/size);
            addVertex(&vertices[index+16], (float)x/size, (float)y/size);
            addVertex(&vertices[index+24], (float)x/size, (float)(y+1)/size);
            addVertex(&vertices[index+32], (float)(x+1)/size, (float)(y+1)/size);
            addVertex(&vertices[index+40], (float)(x+1)/size, (float)y/size);
        }
    
    modelMat.setIdentity();
    if(renderingState == RenderingScreen)
        mainShaderProgram->use();
    else if(renderingState == RenderingShadow)
        shadowShaderProgram->use();
    lightManager.setLights(Vector3(0, 0, 0));
    currentShaderProgram->setUniformF("discardDensity", 1.0);
    
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 8*sizeof(float), vertices);
    currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, 8*sizeof(float), &vertices[3]);
    currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 8*sizeof(float), &vertices[6]);
    glDrawArrays(GL_TRIANGLES, 0, 6*size*size);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
}

void calculateFrame() {
    char str[64];
    sprintf(str, "FPS: %d", currentFPS);
    labelFPS->text = str;
    labelFPS->updateContent();
    
    soundTrackSlider->value = soundSourcesManager.soundSources[0]->getTimeOffset() / soundSourcesManager.soundSources[0]->soundTrack->getLength();
    animationTime += animationFactor;
    
    lightManager.calculateShadows(1);
    //return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mainCam->camMat.setIdentity();
    //mainCam->camMat.rotateX(0.5);
    mainCam->camMat.translate(Vector3(0,1,2));
    mainCam->setFullScreen();
    mainCam->calculate();
    mainCam->use();
    modelMat.setIdentity();
    mainShaderProgram->use();
    glClearColor(1, 1, 1, 1);
    renderingState = RenderingScreen;
    renderScene();
}