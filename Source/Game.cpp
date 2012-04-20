//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Game.h"

Model* humanModel;
SkeletonPose* skeletonPose;
static float animationTime = 0;

void initGame() {
    humanModel = fileManager.getPackage(NULL)->getModel("man.dae");
    skeletonPose = new SkeletonPose(humanModel->skeleton);
    
    GUIImage* image = new GUIImage();
    image->texture = fileManager.getPackage(NULL)->getTexture("logo.png");
    image->sizeAlignment = GUISizeAlignment_Height;
    image->width = 400;
    image->posY = -350;
    currentScreenView->addChild(image);
    
    GUILabel* labelD = new GUILabel();
    labelD->text = std::string("COLLADA Tests");
    labelD->posY = 400;
    currentScreenView->addChild(labelD);
}

void renderScene() {
    glClearColor(0, 1, 1, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    animationTime += animationFactor;
    mainCam->camMat.setIdentity();
    mainCam->camMat.translate(Vector3(0,0,3));
    mainCam->calculate();
    mainCam->use();
    modelMat.setIdentity();
    
    mainFBO->renderInTexture(0);
    mainShaderProgram->use();
    skeletonPose->bonePoses["Back"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Back"]->poseMat.rotateY(0.4);
    skeletonPose->bonePoses["Back"]->poseMat.translate(Vector3(0.0, sin(animationTime)*0.5-0.8, 0.0));
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
    
    Vector3 size = Vector3(1.0, 1.0, 0.0);
    float vertices[] = {
        size.x, -size.y,
        1.0, 0.0,
        size.x, size.y,
        1.0, 1.0,
        -size.x, size.y,
        0.0, 1.0,
        -size.x, -size.y,
        0.0, 0.0
    };
    
    mainFBO->useTexture(0, 0);
    mainCam->setFullScreen();
    glClearColor(1, 1, 1, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    modelMat.setIdentity();
    spriteShaderProgram->use();
    spriteShaderProgram->setUniformF("light", 1.0);
    spriteShaderProgram->setAttribute(POSITION_ATTRIBUTE, 2, 4*sizeof(float), vertices);
    spriteShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, 4*sizeof(float), &vertices[2]);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    
    if(currentScreenView)
        currentScreenView->draw();
}

void calculateFrame() {
    renderScene();
}