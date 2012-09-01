//
//  Game.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

void initScene() {
    //SkeletonPose* skeletonPose = new SkeletonPose(humanModel->skeleton);
    ModelOnlyObject* object = new ModelOnlyObject(fileManager.getPackage("Default")->getModel("boxes.dae"));
    object->transformation.setIdentity();
    objectManager.objects.push_back(object);
    object = new ModelOnlyObject(fileManager.getPackage("Default")->getModel("boxes.dae"));
    object->transformation.setIdentity();
    object->transformation.translate(Vector3(0,0,-3));
    objectManager.objects.push_back(object);
    mainCam->camMat.setIdentity();
    //mainCam->camMat.rotateX(0.5);
    mainCam->camMat.translate(Vector3(0,2,3));
    
    if(0) {
        SpotLight* light = new SpotLight();
        light->position = Vector3(-1.0, 2.0, 1.5);
        light->direction = Vector3(0.5, -0.5, -1.0).normalize();
        light->cutoff = 20.0/180.0*M_PI;
        light->range = 10.0;
    }else{
        PositionalLight* lightC = new PositionalLight();
        lightC->position = Vector3(2.0, 3.0, 0.0);
        lightC->direction = Vector3(0.0, 0.0, 1.0).normalize();
        lightC->range = 5.0;
    }
}

/*void renderScene() {
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
}*/

void calculateFrame() {
    mainCam->calculate();
    mainCam->use();
}