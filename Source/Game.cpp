//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

Model* humanModel = NULL;
SkeletonPose* skeletonPose;
static float animationTime = 0;

void renderScene() {
    modelMat.setIdentity();
    modelMat.rotateY(0.4);
    modelMat.translate(Vector3(0.0, sin(animationTime)*0.5+0.15, 0.0));
    skeletonPose->bonePoses["Back"] = modelMat;
    float thighRotX = (1.0-sin(animationTime))*0.6,
    shinRotX = -0.8*(1.0-sin(animationTime)),
    footRotX = 0.4-0.8*(1.0+sin(animationTime));
    skeletonPose->bonePoses["Thigh_Right"].setIdentity();
    skeletonPose->bonePoses["Thigh_Right"].rotateX(thighRotX);
    skeletonPose->bonePoses["Thigh_Left"].setIdentity();
    skeletonPose->bonePoses["Thigh_Left"].rotateX(thighRotX);
    skeletonPose->bonePoses["Shin_Right"].setIdentity();
    skeletonPose->bonePoses["Shin_Right"].rotateX(shinRotX);
    skeletonPose->bonePoses["Shin_Left"].setIdentity();
    skeletonPose->bonePoses["Shin_Left"].rotateX(shinRotX);
    skeletonPose->bonePoses["Foot_Right"].setIdentity();
    skeletonPose->bonePoses["Foot_Right"].rotateX(footRotX);
    skeletonPose->bonePoses["Foot_Left"].setIdentity();
    skeletonPose->bonePoses["Foot_Left"].rotateX(footRotX);
    skeletonPose->bonePoses["Fingers1_Right"].setIdentity();
    skeletonPose->bonePoses["Fingers1_Right"].rotateZ(-1.3*(cos(animationTime)*0.5+0.5));
    skeletonPose->bonePoses["Fingers2_Right"].setIdentity();
    skeletonPose->bonePoses["Fingers2_Right"].rotateZ(-1.3*(cos(animationTime)*0.5+0.5));
    skeletonPose->calculate();
    humanModel->draw(1.0, skeletonPose);
}

void calculateFrame() {
    if(!humanModel) {
        humanModel = fileManager.getPackage("Default")->getModel("man.dae");
        skeletonPose = new SkeletonPose(humanModel->skeleton);
    }
    
    animationTime += animationFactor;
    mainCam->camMat.setIdentity();
    //mainCam->camMat.rotateX(0.5);
    mainCam->camMat.translate(Vector3(0,1,2));
    mainCam->calculate();
    mainCam->use();
}