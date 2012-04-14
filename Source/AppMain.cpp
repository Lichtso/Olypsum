//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "AppMain.h"

static float animationTime = 0;

Model* humanModel;
SkeletonPose* skeletonPose;

void AppMain(int argc, char *argv[]) {
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
        printf("Couldn't init SDL, Quit.\n");
        exit(1);
    }
    
    videoInfo = SDL_GetVideoInfo();
    if(!videoInfo) {
        printf("Coudn't get video information, Quit.\n");
        exit(2);
    }
    
    if(TTF_Init() == -1) {
        printf("Coudn't init TTF lib, Quit.\n");
        exit(3);
    }
    
    SDL_EnableUNICODE(1);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    screen = SDL_SetVideoMode(videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel, SDL_OPENGL);//SDL_FULLSCREEN
    if(!screen) {
        printf("Coudn't set fullscreen, Quit.\n");
        exit(4);
    }
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glClearColor(1, 1, 1, 0);
    
    //Init Cams
    mainCam = new Cam();
    mainCam->camMat.translate(Vector3(0,0,1));
    mainCam->calculate();
    mainCam->use();
    guiCam = new Cam();
    guiCam->fov = 0.0;
    guiCam->width = guiCam->viewport[2]/2;
    guiCam->height = guiCam->viewport[3]/2;
    guiCam->calculate();
    
    //Init Resources
    fileManager.loadPackage(NULL);
    mainFont = new TextFont();
    mainFont->size = 20;
    mainFont->loadTTF("font");
    titleFont = new TextFont();
    titleFont->size = 30;
    titleFont->loadTTF("font");
    mainShaderProgram = new ShaderProgram();
    mainShaderProgram->loadShaderProgram("main");
    mainShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    mainShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    mainShaderProgram->addAttribute(NORMAL_ATTRIBUTE, "normal");
    mainShaderProgram->link();
    shadowShaderProgram = new ShaderProgram();
    shadowShaderProgram->loadShaderProgram("shadow");
    shadowShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    shadowShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shadowShaderProgram->link();
    spriteShaderProgram = new ShaderProgram();
    spriteShaderProgram->loadShaderProgram("sprite");
    spriteShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    spriteShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    spriteShaderProgram->link();
    mainSkeletonShaderProgram = new ShaderProgram();
    mainSkeletonShaderProgram->loadShaderProgram("mainSkeleton");
    mainSkeletonShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    mainSkeletonShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    mainSkeletonShaderProgram->addAttribute(NORMAL_ATTRIBUTE, "normal");
    mainSkeletonShaderProgram->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    mainSkeletonShaderProgram->addAttribute(JOINT_ATTRIBUTE, "joints");
    mainSkeletonShaderProgram->link();
    currentScreenView = new GUIScreenView();
    
    //Init Game {
    humanModel = fileManager.getPackage(NULL)->getModel("human.dae");
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
    
    //}
    
    unsigned long thenTicks = 0, nowTicks;
    while(true) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    handleKeyDown(&event.key.keysym);
                    break;
                case SDL_KEYUP:
                    handleKeyUp(&event.key.keysym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handleMouseDown(&event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handleMouseUp(&event.button);
                    break;
                case SDL_MOUSEMOTION:
                    handleMouseMove(&event.motion);
                    break;
                case SDL_QUIT:
                    AppTerminate();
                    break;
                default:
                    break;
            }
        }
        updateKeyState();
        
        //Step Game {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        animationTime += animationFactor;
        mainCam->camMat.setIdentity();
        mainCam->camMat.translate(Vector3(0,0,3));
        mainCam->calculate();
        mainCam->use();
        modelMat.setIdentity();
        
        mainShaderProgram->use();
        skeletonPose->bonePoses["Back"]->poseMat.setIdentity();
        skeletonPose->bonePoses["Back"]->poseMat.rotateY(0.3);
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
        skeletonPose->calculateBonePose(humanModel->skeleton->rootBone, NULL);
        humanModel->draw(skeletonPose);
        
        /*char fpsStr[32];
        sprintf(fpsStr, "FPS: %d", (int)round(1.0/animationFactor));
        mainFont->renderStringToScreen(fpsStr, btVector3(1.0, -0.5, 0.0), 0.002, color, true);
        mainFont->renderStringToScreen(fpsStr, btVector3(-1.0, -0.5, 0.0), 0.002, color, true);
        mainFont->renderStringToScreen(fpsStr, btVector3(0.0, 0.5, 0.0), 0.002, color, true);
        */
        
        if(currentScreenView)
            currentScreenView->draw();
        SDL_GL_SwapBuffers();
        //}
        
        if(thenTicks > 0) {
			nowTicks = clock();
			animationFactor = (float)(nowTicks-thenTicks)/CLOCKS_PER_SEC;
			if(animationFactor < 1.0/50.0) animationFactor = 1.0/50.0;
            thenTicks = nowTicks;
        }else{
            thenTicks = clock();
			animationFactor = 0.0;
        }
    }
}

void AppTerminate() {
    TTF_Quit();
    SDL_Quit();
    exit(0);
}