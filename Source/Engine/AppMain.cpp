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

void AppMain(int argc, char *argv[]) {
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
    updateVideoMode();
    
    //Init OpenGL
    {
        char* glStr = NULL;
        GLint glAuxIa, glAuxIb;
        glStr = (char*)glGetString(GL_VENDOR);
        log(info_log, std::string("OpenGL vendor: ")+glStr);
        glStr = (char*)glGetString(GL_RENDERER);
        log(info_log, std::string("OpenGL renderer: ")+glStr);
        glStr = (char*)glGetString(GL_VERSION);
        log(info_log, std::string("OpenGL driver: ")+glStr);
        glGetIntegerv(GL_MAJOR_VERSION, &glAuxIa);
        glGetIntegerv(GL_MINOR_VERSION, &glAuxIb);
        log(info_log, std::string("OpenGL version: ")+stringOf(glAuxIa)+"."+stringOf(glAuxIb));
        if(glAuxIa < 3 || (glAuxIa == 3 && glAuxIb < 2)) {
            log(error_log, std::string("OpenGL version 3.2 is required, Quit.")+glStr);
            exit(5);
        }
#ifdef DEBUG
        std::ostringstream stream;
        stream << "OpenGL extensions found: ";
        glGetIntegerv(GL_NUM_EXTENSIONS, &glAuxIa);
        for(GLint i = 0; i < glAuxIa; i ++) {
            glStr = (char*)glGetStringi(GL_EXTENSIONS, i);
            stream << glStr << " ";
        }
        log(info_log, stream.str());
        stream.str("");
        stream << "OpenGL compressions found: ";
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &glAuxIa);
        GLint glCompressionFormats[glAuxIa];
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, glCompressionFormats);
        for(GLint i = 0; i < glAuxIa; i ++)
            stream << stringOf(glCompressionFormats[i]) << " ";
        log(info_log, stream.str());
#endif
    }
    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &optionsState.anisotropy);
    optionsState.anisotropy = fmin(optionsState.anisotropy, pow(2.0, optionsState.surfaceQuality));
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glFrontFace(GL_CCW);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    //Init Cams
    guiCam = new CamObject();
    guiCam->fov = 0.0;
    guiCam->near = -1.0;
    guiCam->far = 1.0;
    guiCam->updateViewMat();
    
    //Init Resources
    mainFont = new TextFont();
    mainFont->size = prevOptionsState.videoWidth*0.04;
    mainFont->loadTTF("font");
    italicFont = new TextFont();
    italicFont->size = prevOptionsState.videoHeight*0.04;
    italicFont->loadTTF("font_italic");
    initLightVolumes();
    objectManager.init();
    loadStaticShaderPrograms();
    menu.setMenu(Menu::Name::loading);
    
    SDL_Event event;
    while(true) {
        while(SDL_PollEvent(&event)) {
            if(!currentScreenView) break;
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
                            menu.handleMouseDown(event);
                            break;
                        case SDL_BUTTON_WHEELDOWN:
                        case SDL_BUTTON_WHEELUP:
                            menu.handleMouseWheel(event);
                            break;
                    }
                break;
                case SDL_MOUSEBUTTONUP:
                    if(event.button.button == SDL_BUTTON_WHEELDOWN || event.button.button == SDL_BUTTON_WHEELUP) break;
                    menu.handleMouseUp(event);
                break;
                case SDL_MOUSEMOTION:
                    menu.handleMouseMove(event);
                break;
                case SDL_QUIT:
                    AppTerminate();
                break;
                default:
                break;
            }
        }
        keyState = SDL_GetKeyState(NULL);
        //modKeyState = SDL_GetModState();
        
        if(menu.current == Menu::Name::inGame && profiler.isFirstFrameInSec()) {
            char str[64];
            sprintf(str, "FPS: %d", profiler.FPS);
            menu.consoleAdd(str, 1.0);
        }
        
        if(levelManager.gameStatus == noGame) {
            glClearColor(1, 1, 1, 1);
            glViewport(0, 0, prevOptionsState.videoWidth, prevOptionsState.videoHeight);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            profiler.leaveSection("Rest");
            objectManager.gameTick();
        }
        menu.gameTick();
        //if(currentScreenView)
            currentScreenView->draw();
        networkManager.gameTick();
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