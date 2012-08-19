//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"
#import <sys/time.h>
#import <Cocoa/Cocoa.h>

Uint8* keyState;
SDLMod modKeyState;
float timeInLastSec = 0.0;
unsigned int currentFPS = 0, newFPS = 0;

#ifdef __MAC_OS__
#import "macCompCode/SDLMain.h"
#else
void updateVideoMode() {
    screen = SDL_SetVideoMode(videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel, (fullScreenEnabled) ? SDL_OPENGL | SDL_FULLSCREEN : SDL_OPENGL);
    if(!screen) {
        printf("Coudn't set video mode, Quit.\n");
        exit(4);
    }
    printf("Video mode: %d x %d\n", videoInfo->current_w, videoInfo->current_h);
}
#endif

void clearCurrentWorld() {
    soundSourcesManager.clear();
    particleSystemManager.clear();
    decalManager.clear();
    fileManager.clear();
    lightManager.clear();
}

void AppMain(int argc, char *argv[]) {
    fileManager.loadOptions();
    
    //Init SDL
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
    updateVideoMode();
    
    //Init OpenGL
    char* glStr = NULL;
    glStr = (char*)glGetString(GL_VENDOR);
    printf("OpenGL, vendor: %s\n", glStr);
    glStr = (char*)glGetString(GL_RENDERER);
    printf("OpenGL, renderer: %s\n", glStr);
    glStr = (char*)glGetString(GL_VERSION);
    printf("OpenGL, version: %s\n", glStr);
    glStr = (char*)glGetString(GL_EXTENSIONS);
    //printf("OpenGL, extensions: %s\n\n", glStr);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SPRITE);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
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
    mainFont = new TextFont();
    mainFont->size = videoInfo->current_h*0.05;
    mainFont->loadTTF("font");
    mainFBO.init();
    lightManager.init();
    decalManager.init();
    
    //Load Shader Programs
    loadStaticShaderPrograms();
    setMenu(loadingMenu);
    loadDynamicShaderPrograms();
    
    SDL_Event event;
    SDL_PollEvent(&event);
    setMenu(mainMenu);
    
    timeval timeThen, timeNow;
    gettimeofday(&timeThen, 0);
    while(true) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    if(currentScreenView && currentScreenView->handleKeyDown(&event.key.keysym))
                        break;
                break;
                case SDL_KEYUP:
                    if(currentScreenView && currentScreenView->handleKeyUp(&event.key.keysym))
                        break;
                    handleMenuKeyUp(&event.key.keysym);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    event.button.x = event.button.x*mouseTranslation[0]+mouseTranslation[2];
                    event.button.y = event.button.y*mouseTranslation[1]+mouseTranslation[3];
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
                break;
                case SDL_MOUSEBUTTONUP:
                    event.button.x = event.button.x*mouseTranslation[0]+mouseTranslation[2];
                    event.button.y = event.button.y*mouseTranslation[1]+mouseTranslation[3];
                    if(currentScreenView)
                        currentScreenView->handleMouseUp(event.button.x, event.button.y);
                break;
                case SDL_MOUSEMOTION:
                    event.button.x = event.button.x*mouseTranslation[0]+mouseTranslation[2];
                    event.button.y = event.button.y*mouseTranslation[1]+mouseTranslation[3];
                    if(currentScreenView)
                        currentScreenView->handleMouseMove(event.button.x, event.button.y);
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
        
        if(currentMenu == inGameMenu) {
            char str[64];
            sprintf(str, "FPS: %d", currentFPS);
            GUILabel* labelFPS = (GUILabel*)currentScreenView->children[0];
            labelFPS->text = str;
            labelFPS->updateContent();
        }
        
        if(gameStatus == noGame) {
            glClearColor(1, 1, 1, 1);
            glViewport(0, 0, videoInfo->current_w, videoInfo->current_h);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            calculateFrame();
            soundSourcesManager.calculate();
            lightManager.calculateShadows(1);
            particleSystemManager.calculate();
            decalManager.calculate();
            mainCam->use();
            mainFBO.renderInDeferredBuffers();
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            renderScene();
            decalManager.draw();
            particleSystemManager.draw();
            lightManager.useLights();
        }
        if(currentScreenView) currentScreenView->draw();
        SDL_GL_SwapBuffers();
        
        gettimeofday(&timeNow, 0);
        animationFactor = timeNow.tv_sec - timeThen.tv_sec;
        animationFactor += (timeNow.tv_usec - timeThen.tv_usec) / 1000000.0;
        gettimeofday(&timeThen, 0);
        
        newFPS ++;
        timeInLastSec += animationFactor;
        if(timeInLastSec >= 1.0) {
            timeInLastSec -= 1.0;
            currentFPS = newFPS;
            newFPS = 0;
        }
    }
}

void AppTerminate() {
    TTF_Quit();
    SDL_Quit();
    exit(0);
}