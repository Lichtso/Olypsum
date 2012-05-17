//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "InputController.h"
#import <sys/time.h>

Uint8* keyState;
SDLMod modKeyState;

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
    
    char* glStr = NULL;
    glStr = (char*)glGetString(GL_VENDOR);
    printf("%s\n", glStr);
    glStr = (char*)glGetString(GL_RENDERER);
    printf("%s\n", glStr);
    glStr = (char*)glGetString(GL_VERSION);
    printf("%s\n", glStr);
    glStr = (char*)glGetString(GL_EXTENSIONS);
    printf("%s\n\n", glStr);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    mainFBO.init();
    
    //Init Cams
    mainCam = new Cam();
    mainCam->width = videoInfo->current_w/2;
    mainCam->height = videoInfo->current_h/2;
    mainCam->calculate();
    mainCam->use();
    guiCam = new Cam();
    guiCam->fov = 0.0;
    guiCam->width = videoInfo->current_w/2;
    guiCam->height = videoInfo->current_h/2;
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
    mainShaderProgram->setUniformF("discardDensity", 1.0);
    shadowShaderProgram = new ShaderProgram();
    shadowShaderProgram->loadShaderProgram("shadow");
    shadowShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    shadowShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shadowShaderProgram->link();
    shadowShaderProgram->setUniformF("discardDensity", 1.0);
    spriteShaderProgram = new ShaderProgram();
    spriteShaderProgram->loadShaderProgram("sprite");
    spriteShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    spriteShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    spriteShaderProgram->link();
    blurShaderProgram = new ShaderProgram();
    blurShaderProgram->loadShaderProgram("blur");
    blurShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    blurShaderProgram->link();
    mainSkeletonShaderProgram = new ShaderProgram();
    mainSkeletonShaderProgram->loadShaderProgram("mainSkeleton");
    mainSkeletonShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    mainSkeletonShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    mainSkeletonShaderProgram->addAttribute(NORMAL_ATTRIBUTE, "normal");
    mainSkeletonShaderProgram->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    mainSkeletonShaderProgram->addAttribute(JOINT_ATTRIBUTE, "joints");
    mainSkeletonShaderProgram->link();
    mainSkeletonShaderProgram->setUniformF("discardDensity", 1.0);
    shadowSkeletonShaderProgram = new ShaderProgram();
    shadowSkeletonShaderProgram->loadShaderProgram("shadowSkeleton");
    shadowSkeletonShaderProgram->addAttribute(POSITION_ATTRIBUTE, "position");
    shadowSkeletonShaderProgram->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shadowSkeletonShaderProgram->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    shadowSkeletonShaderProgram->addAttribute(JOINT_ATTRIBUTE, "joints");
    shadowSkeletonShaderProgram->link();
    shadowSkeletonShaderProgram->setUniformF("discardDensity", 1.0);
    currentScreenView = new GUIScreenView();
    
    SDL_Event event;
    SDL_PollEvent(&event);
    initGame();
    
    timeval timeThen, timeNow;
    gettimeofday(&timeThen, 0);
    while(true) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    if(currentScreenView && currentScreenView->handleKeyDown(&event.key.keysym))
                        return;
                    handleKeyDown(&event.key.keysym);
                break;
                case SDL_KEYUP:
                    if(currentScreenView && currentScreenView->handleKeyUp(&event.key.keysym))
                        return;
                    handleKeyUp(&event.key.keysym);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    if(currentScreenView) {
                        switch(event.button.button) {
                            case SDL_BUTTON_LEFT:
                                currentScreenView->handleMouseDown(event.button.x, event.button.y);
                            break;
                            case SDL_BUTTON_WHEELDOWN:
                                currentScreenView->handleMouseWheel(event.button.x, event.button.y, -1.0);
                            break;
                            case SDL_BUTTON_WHEELUP:
                                currentScreenView->handleMouseWheel(event.button.x, event.button.y, 1.0);
                            break;
                        }
                    }
                    handleMouseDown(&event.button);
                break;
                case SDL_MOUSEBUTTONUP:
                    if(currentScreenView)
                        currentScreenView->handleMouseUp(event.button.x, event.button.y);
                    handleMouseUp(&event.button);
                break;
                case SDL_MOUSEMOTION:
                    if(currentScreenView)
                        currentScreenView->handleMouseMove(event.button.x, event.button.y);
                    handleMouseMove(&event.motion);
                break;
                case SDL_QUIT:
                    AppTerminate();
                break;
                default:
                break;
            }
        }
        keyState = SDL_GetKeyState(NULL);
        modKeyState = SDL_GetModState();
        
        calculateFrame();
        if(currentScreenView) currentScreenView->draw();
        SDL_GL_SwapBuffers();
        
        gettimeofday(&timeNow, 0);
        animationFactor = timeNow.tv_sec - timeThen.tv_sec;
        animationFactor += (timeNow.tv_usec - timeThen.tv_usec) / 1000000.0;
        gettimeofday(&timeThen, 0);
    }
}

void AppTerminate() {
    TTF_Quit();
    SDL_Quit();
    exit(0);
}