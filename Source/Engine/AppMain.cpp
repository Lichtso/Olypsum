//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

Uint8* keyState;
SDLMod modKeyState;
const float loadingScreenTime = 0.1;
float timeInLastSec = 0.0, loadingScreen = loadingScreenTime;

#ifndef __MAC_OS__
void updateVideoMode() {
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
    if(!videoInfo) {
        log(error_log, "Coudn't get video information, Quit.");
        exit(3);
    }
    screenSize[0] = videoInfo->current_w;
    screenSize[1] = videoInfo->current_h;
    screenSize[2] = 1;
    SDL_Surface* screen = SDL_SetVideoMode(screenSize[0], screenSize[1], videoInfo->vfmt->BitsPerPixel, (fullScreenEnabled) ? SDL_OPENGL | SDL_FULLSCREEN : SDL_OPENGL);
    if(!screen) {
        log(error_log, "Coudn't set video mode, Quit.");
        exit(4);
    }
}
#endif

void AppMain(int argc, char *argv[]) {
    optionsState.loadOptions();
    
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
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, optionsState.vSyncEnabled);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    updateVideoMode();
    
    //Init OpenGL
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &optionsState.anisotropy);
    optionsState.anisotropy = fmin(optionsState.anisotropy, pow(2.0, optionsState.surfaceQuality));
    char* glStr = NULL;
    glStr = (char*)glGetString(GL_VENDOR);
    log(info_log, std::string("OpenGL, vendor: ")+glStr);
    glStr = (char*)glGetString(GL_RENDERER);
    log(info_log, std::string("OpenGL, renderer: ")+glStr);
    glStr = (char*)glGetString(GL_VERSION);
    log(info_log, std::string("OpenGL, version: ")+glStr);
    //glStr = (char*)glGetString(GL_EXTENSIONS);
    //log(info_log, std::string("OpenGL, extensions: ")+glStr);
    
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Init Cams
    mainCam = new Cam();
    mainCam->gameTick();
    mainCam->use();
    guiCam = new Cam();
    guiCam->fov = 0.0;
    guiCam->near = -1.0;
    guiCam->far = 1.0;
    guiCam->gameTick();
    
    //Init Resources
    mainFont = new TextFont();
    mainFont->size = screenSize[1]*0.04;
    mainFont->loadTTF("font");
    italicFont = new TextFont();
    italicFont->size = screenSize[1]*0.04;
    italicFont->loadTTF("font_italic");
    initLightVolumes();
    objectManager.init();
    
    //Load Shader Programs
    loadStaticShaderPrograms();
    setMenu(loadingMenu);
    loadDynamicShaderPrograms();
    
    SDL_Event event;
    SDL_PollEvent(&event);
    while(true) {
        while(SDL_PollEvent(&event)) {
            if(!currentScreenView) break;
            switch(event.type) {
                case SDL_ACTIVEEVENT:
                    if(!event.active.gain && currentMenu == inGameMenu)
                        setMenu(gameEscMenu);
                break;
                case SDL_KEYDOWN:
                    if(currentScreenView->handleKeyDown(&event.key.keysym))
                        break;
                break;
                case SDL_KEYUP:
                    if(currentScreenView->handleKeyUp(&event.key.keysym))
                        break;
                    handleMenuKeyUp(&event.key.keysym);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    event.button.x *= screenSize[2];
                    event.button.y *= screenSize[2];
                    switch(event.button.button) {
                        case SDL_BUTTON_LEFT:
                            currentScreenView->handleMouseDown(event.button.x, event.button.y);
                        case SDL_BUTTON_MIDDLE:
                        case SDL_BUTTON_RIGHT:
                            if(controlsMangager && currentMenu == inGameMenu)
                                controlsMangager->handleMouseDown(event.button.x, event.button.y, event);
                            break;
                        case SDL_BUTTON_WHEELDOWN:
                        case SDL_BUTTON_WHEELUP:
                            float delta = (event.button.button == SDL_BUTTON_WHEELDOWN) ? -1.0 : 1.0;
                            if(!currentScreenView->handleMouseWheel(event.button.x, event.button.y, delta)
                               && controlsMangager && currentMenu == inGameMenu)
                                controlsMangager->handleMouseWheel(event.button.x, event.button.y, delta);
                            break;
                    }
                break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_WHEELDOWN || event.button.button == SDL_BUTTON_WHEELUP) break;
                    event.button.x *= screenSize[2];
                    event.button.y *= screenSize[2];
                    if(!currentScreenView->handleMouseUp(event.button.x, event.button.y) && controlsMangager && currentMenu == inGameMenu)
                        controlsMangager->handleMouseUp(event.button.x, event.button.y, event);
                break;
                case SDL_MOUSEMOTION:
                    event.button.x *= screenSize[2];
                    event.button.y *= screenSize[2];
                    currentScreenView->handleMouseMove(event.button.x, event.button.y);
                    if(controlsMangager && currentMenu == inGameMenu)
                        controlsMangager->handleMouseMove(event.button.x, event.button.y, event);
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
        
        if(currentMenu == inGameMenu && profiler.isFirstFrameInSec()) {
            char str[64];
            sprintf(str, "FPS: %d", profiler.FPS);
            if(controlsMangager)
                controlsMangager->consoleAdd(str, 0.9);
        }
        
        if(levelManager.gameStatus == noGame) {
            if(currentMenu == loadingMenu) {
                loadingScreen -= profiler.animationFactor;
                GUIProgressBar* progressBar = static_cast<GUIProgressBar*>(currentScreenView->children[0]);
                progressBar->value = 1.0-loadingScreen/loadingScreenTime;
                if(loadingScreen <= 0.0)
                    setMenu(mainMenu);
            }
            
            glClearColor(1, 1, 1, 1);
            glViewport(0, 0, screenSize[0], screenSize[1]);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            profiler.leaveSection("Rest");
            objectManager.gameTick();
        }
        if(currentScreenView) currentScreenView->draw();
        profiler.leaveSection("GUI Draw");
        SDL_GL_SwapBuffers();
        profiler.leaveSection("Swap Buffers");
        profiler.markFrame();
    }
}

void AppTerminate() {
    TTF_Quit();
    SDL_Quit();
    exit(0);
}