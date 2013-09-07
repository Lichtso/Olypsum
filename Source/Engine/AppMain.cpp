//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

const SDL_VideoInfo* updateVideoModeInternal(bool& fullScreen) {
    const SDL_VideoInfo* videoInfo = SDL_GetVideoInfo();
    if(!videoInfo) {
        log(error_log, "Coudn't get video information, Quit.");
        exit(3);
    }
    screenSize[0] = videoInfo->current_w;
    screenSize[1] = videoInfo->current_h;
    
    if(optionsState.videoScale > screenSize[2])
        optionsState.videoScale = screenSize[2];
    if(optionsState.videoWidth > videoInfo->current_w || optionsState.videoHeight > videoInfo->current_h) {
        optionsState.videoWidth = videoInfo->current_w;
        optionsState.videoHeight = videoInfo->current_h;
    }
    fullScreen = optionsState.videoWidth == videoInfo->current_w && optionsState.videoHeight == videoInfo->current_h;
    optionsState.videoWidth *= optionsState.videoScale;
    optionsState.videoHeight *= optionsState.videoScale;
    prevOptionsState = optionsState;
    menu.clear();
    
    SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, optionsState.vSyncEnabled);
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8)
    
    return videoInfo;
}

void AppMain() {
    createDir(gameDataDir);
    gameDataDir += "Olypsum/";
    createDir(gameDataDir);
    
    optionsState.loadOptions();
    networkManager.init();
    
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
    
    //Init the rest
    objectManager.init();
    
    SDL_Event event;
    while(true) {
        while(SDL_PollEvent(&event)) {
            int mouseX = event.button.x*prevOptionsState.videoScale - menu.screenView->width,
                mouseY = menu.screenView->height - event.button.y*prevOptionsState.videoScale;
            
            switch(event.type) {
                case SDL_ACTIVEEVENT:
                    menu.handleActiveEvent(event.active.gain);
                break;
                case SDL_KEYDOWN:
                    menu.handleKeyDown(event);
                break;
                case SDL_KEYUP:
                    menu.handleKeyUp(event);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    switch(event.button.button) {
                        case SDL_BUTTON_LEFT:
                        case SDL_BUTTON_MIDDLE:
                        case SDL_BUTTON_RIGHT:
                            menu.handleMouseDown(mouseX, mouseY, event.button.button);
                            break;
                        case SDL_BUTTON_WHEELDOWN:
                            menu.handleMouseWheel(mouseX, mouseY, 0.0, -1.0);
                            break;
                        case SDL_BUTTON_WHEELUP:
                            menu.handleMouseWheel(mouseX, mouseY, 0.0, 1.0);
                            break;
                    }
                break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_WHEELDOWN || event.button.button == SDL_BUTTON_WHEELUP) break;
                    menu.handleMouseUp(mouseX, mouseY, event.button.button);
                break;
                case SDL_MOUSEMOTION:
                    menu.handleMouseMove(mouseX, mouseY);
                break;
                case SDL_QUIT:
                    AppTerminate();
                break;
                default:
                break;
            }
        }
        keyState = SDL_GetKeyState(&keyStateSize);
        
        if(menu.current == Menu::Name::inGame && profiler.isFirstFrameInSec()) {
            char str[64];
            sprintf(str, "FPS: %d", profiler.FPS);
            menu.consoleAdd(str, 1.0);
        }
        
        networkManager.gameTick();
        leapManager.gameTick();
        menu.gameTick();
        if(levelManager.gameStatus == noGame) {
            glClearColor(1, 1, 1, 1);
            glViewport(0, 0, prevOptionsState.videoWidth, prevOptionsState.videoHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            profiler.leaveSection("Rest");
            objectManager.gameTick();
        }
        menu.screenView->drawScreen();
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

Uint8* keyState;
int keyStateSize;