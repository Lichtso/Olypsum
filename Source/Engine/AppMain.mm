//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "AppMain.h"
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
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
    if(!videoInfo) {
        log(error_log, "Coudn't get video information, Quit.");
        exit(3);
    }
    screenSize[0] = videoInfo->current_w;
    screenSize[1] = videoInfo->current_h;
    SDL_Surface* screen = SDL_SetVideoMode(screenSize[0], screenSize[1], videoInfo->vfmt->BitsPerPixel, (fullScreenEnabled) ? SDL_OPENGL | SDL_FULLSCREEN : SDL_OPENGL);
    if(!screen) {
        log(error_log, "Coudn't set video mode, Quit.");
        exit(4);
    }
}
#endif

void AppMain(int argc, char *argv[]) {
    fileManager.loadOptions();
    
    //Init SDL
    if(SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) < 0) {
        log(error_log, "Couldn't init SDL, Quit.");
        exit(1);
    }
    
    if(TTF_Init() == -1) {
        log(error_log, "Coudn't init TTF lib, Quit.");
        exit(2);
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
    log(info_log, std::string("OpenGL, vendor: ")+glStr);
    glStr = (char*)glGetString(GL_RENDERER);
    log(info_log, std::string("OpenGL, renderer: ")+glStr);
    glStr = (char*)glGetString(GL_VERSION);
    log(info_log, std::string("OpenGL, version: ")+glStr);
    glStr = (char*)glGetString(GL_EXTENSIONS);
    //log(info_log, std::string("OpenGL, extensions: ")+glStr);
    
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glEnable(GL_POINT_SPRITE);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Init Cams
    mainCam = new Cam();
    mainCam->calculate();
    mainCam->use();
    guiCam = new Cam();
    guiCam->fov = 0.0;
    guiCam->calculate();
    
    //Init Resources
    mainFont = new TextFont();
    mainFont->size = screenSize[1]*0.05;
    mainFont->loadTTF("font");
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
                    event.button.x = (event.button.x+mouseTranslation[0])*screenSize[2];
                    event.button.y = (event.button.y+mouseTranslation[1])*screenSize[2];
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
                    event.button.x = (event.button.x+mouseTranslation[0])*screenSize[2];
                    event.button.y = (event.button.y+mouseTranslation[1])*screenSize[2];
                    if(currentScreenView)
                        currentScreenView->handleMouseUp(event.button.x, event.button.y);
                break;
                case SDL_MOUSEMOTION:
                    event.button.x = (event.button.x+mouseTranslation[0])*screenSize[2];
                    event.button.y = (event.button.y+mouseTranslation[1])*screenSize[2];
                    if(currentScreenView)
                        currentScreenView->handleMouseMove(event.button.x, event.button.y);
                    
                    //TODO: DEBUG
                    mainCam->camMat.setIdentity();
                    mainCam->camMat.setRotation(btQuaternion(((float)event.button.x/screenSize[0]-0.5)*M_PI*2.0, ((float)event.button.y/screenSize[1]-0.5)*M_PI*2.0, 0));
                    mainCam->camMat.setOrigin(mainCam->camMat.getBasis().inverse()*btVector3(0, 0, 3));
                    mainCam->calculate();
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
        
        if(worldManager.gameStatus == noGame) {
            glClearColor(0, 0, 0, 1);
            glViewport(0, 0, screenSize[0], screenSize[1]);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            objectManager.gameTick();
            worldManager.gameTick();
            soundSourcesManager.gameTick();
            lightManager.calculateShadows(1);
            particleSystemManager.gameTick();
            decalManager.gameTick();
            mainCam->use();
            mainFBO.renderInDeferredBuffers(false);
            objectManager.draw();
            decalManager.draw();
            particleSystemManager.draw();
            lightManager.drawDeferred();
        }
        if(currentScreenView) currentScreenView->draw();
        SDL_GL_SwapBuffers();
        
        gettimeofday(&timeNow, 0);
        worldManager.animationFactor = timeNow.tv_sec - timeThen.tv_sec;
        worldManager.animationFactor += (timeNow.tv_usec - timeThen.tv_usec) / 1000000.0;
        gettimeofday(&timeThen, 0);
        
        newFPS ++;
        timeInLastSec += worldManager.animationFactor;
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