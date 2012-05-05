//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Game.h"

GUILabel* labelFPS;
Model* humanModel;
SkeletonPose* skeletonPose;
static float animationTime = 0;

void initGame() {
    humanModel = fileManager.getPackage(NULL)->getModel("man.dae");
    skeletonPose = new SkeletonPose(humanModel->skeleton);
    
    if(false) {
        DirectionalLight* light = new DirectionalLight();
        light->direction = Vector3(0.0, -1.0, 0.0).normalize();
        light->distance = 50.0;
        light->range = 100.0;
        light->width = light->height = 2.0;
        light->shadowResolution = 1024;
        lightManager.lights.push_back((Light*)light);
    }
    
    if(false) {
        SpotLight* lightB = new SpotLight();
        lightB->position = Vector3(-1.0, 2.0, 2.0);
        lightB->direction = Vector3(0.5, -0.5, -1.0).normalize();
        lightB->cutoff = 20.0/180.0*M_PI;
        lightB->range = 10.0;
        lightB->shadowResolution = 512;
        lightManager.lights.push_back((Light*)lightB);
    }
    
    if(true) {
        PositionalLight* lightC = new PositionalLight();
        lightC->position = Vector3(0.0, 4.0, 0.0);
        lightC->direction = Vector3(1.0, 0.0, 0.0).normalize();
        lightC->range = 10.0;
        lightC->shadowResolution = 1024;
        lightManager.lights.push_back((Light*)lightC);
    }
    
    GUIImage* image = new GUIImage();
    image->texture = fileManager.getPackage(NULL)->getTexture("logo.png");
    image->sizeAlignment = GUISizeAlignment_Height;
    image->width = 400;
    image->posY = -350;
    //currentScreenView->addChild(image);
    
    GUILabel* label = new GUILabel();
    label->text = std::string("Graphic Tests");
    label->posY = 450;
    currentScreenView->addChild(label);
    
    labelFPS = new GUILabel();
    labelFPS->text = std::string("FPS");
    labelFPS->posY = 400;
    currentScreenView->addChild(labelFPS);
}

static void addVertex(float* vertices, float x, float y) {
    vertices[0] = (x-0.5)*10.0;
    vertices[1] = 0.0;
    vertices[2] = (y-0.5)*10.0;
    vertices[3] = 0.0;
    vertices[4] = 1.0;
    vertices[5] = 0.0;
    vertices[6] = x;
    vertices[7] = y;
};

void renderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelMat.setIdentity();
    modelMat.rotateY(0.4);
    modelMat.translate(Vector3(0.0, sin(animationTime)*0.5+0.2, 0.0));
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
    humanModel->draw(skeletonPose);
    
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
    currentShaderProgram->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 8*sizeof(float), vertices);
    currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, 8*sizeof(float), &vertices[3]);
    currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 8*sizeof(float), &vertices[6]);
    glDrawArrays(GL_TRIANGLES, 0, 6*size*size);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
}

void calculateFrame() {
    char str[64];
    sprintf(str, "FPS: %d", (int)(1.0/animationFactor));
    labelFPS->text = str;
    labelFPS->updateContent();
    
    animationTime += animationFactor*10.0;
    
    lightManager.lights[0]->calculateShadowmap();
    lightManager.setLights();
    //return;
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    mainCam->camMat.setIdentity();
    mainCam->camMat.translate(Vector3(0,1,3));
    mainCam->setFullScreen();
    mainCam->calculate();
    mainCam->use();
    modelMat.setIdentity();
    mainShaderProgram->use();
    glClearColor(1, 1, 1, 1);
    renderScene();
}