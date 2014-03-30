//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

void AppMain() {
    //Init paths
    /*if(resourcesPath.size() == 0) {
        char cwdPath[512];
        getcwd(cwdPath, sizeof(cwdPath)/sizeof(char)-1);
        resourcesPath = std::string(cwdPath)+'/'+executablePath;
    }*/
    resourcesPath = trimPath(resourcesPath, 2)+"/Resources/";
    
#ifdef WIN32
    supportPath = std::string(getenv("USERPROFILE"))+"\\Documents\\";
#elif defined __APPLE__
    supportPath = std::string(getenv("HOME"))+"/Library/Application Support/";
#else
    supportPath = std::string(getenv("HOME"))+"/.";
#endif
    
    supportPath = trimPath(supportPath+"Gamefortec", 0)+'/';
    createDir(supportPath);
    supportPath = supportPath+"Olypsum/";
    createDir(supportPath);
    optionsState.loadOptions();
    
    //Init SDL
    if(SDL_Init(SDL_INIT_VIDEO) < 0) {
        log(error_log, "Couldn't init SDL, Quit.");
        exit(1);
    }
    SDL_SetHint(SDL_HINT_VIDEO_MINIMIZE_ON_FOCUS_LOSS, "0");
    if(TTF_Init() == -1) {
        log(error_log, "Coudn't init TTF lib, Quit.");
        exit(2);
    }
    
    //Init OpenGL
    {
        SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
        SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
        SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
        SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
        SDL_GL_SetAttribute(SDL_GL_SHARE_WITH_CURRENT_CONTEXT, 1);
        menu.updateWindow();
        
#ifdef WIN32
        glewExperimental = GL_TRUE;
        if(glewInit() != GLEW_OK) {
            log(error_log, "GLEW failed to initialize, Quit.");
            exit(3);
        }
#endif
        
        time_t t = time(0);
        struct tm* date = gmtime(&t);
        char* buffer = new char[2048];
        sprintf(buffer, "Date / Time: %04d.%02d.%02d %02d:%02d:%02d", 1900+date->tm_year, 1+date->tm_mon, date->tm_mday, date->tm_hour, date->tm_min, date->tm_sec);
        log(typeless_log, buffer);
        log(typeless_log, std::string("Engine Version: ")+engineVersion);
        //log(typeless_log, std::string("Multi Threading: ")+stringOf(std::thread::hardware_concurrency()));
        char* glStr = NULL;
        GLint glAuxIa, glAuxIb;
        glStr = (char*)glGetString(GL_VENDOR);
        log(typeless_log, std::string("OpenGL Vendor: ")+glStr);
        glStr = (char*)glGetString(GL_RENDERER);
        log(typeless_log, std::string("OpenGL Renderer: ")+glStr);
        glStr = (char*)glGetString(GL_VERSION);
        log(typeless_log, std::string("OpenGL Driver: ")+glStr);
        glGetIntegerv(GL_MAJOR_VERSION, &glAuxIa);
        glGetIntegerv(GL_MINOR_VERSION, &glAuxIb);
        log(typeless_log, std::string("OpenGL Version: ")+stringOf(glAuxIa)+'.'+stringOf(glAuxIb));
        if(glAuxIa < 3 || (glAuxIa == 3 && glAuxIb < 2)) {
            log(error_log, "OpenGL version 3.2 is required, Quit.");
            exit(4);
        }
        
    #ifdef DEBUG
        sprintf(buffer, "OpenGL extensions found: ");
        glGetIntegerv(GL_NUM_EXTENSIONS, &glAuxIa);
        for(GLint i = 0; i < glAuxIa; i ++) {
            glStr = (char*)glGetStringi(GL_EXTENSIONS, i);
            sprintf(buffer, "%s%s, ", buffer, glStr);
        }
        log(typeless_log, buffer);
        sprintf(buffer, "OpenGL compressions found: ");
        glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS, &glAuxIa);
        GLint glCompressionFormats[glAuxIa];
        glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS, glCompressionFormats);
        for(GLint i = 0; i < glAuxIa; i ++)
            sprintf(buffer, "%s%d ", buffer, glCompressionFormats[i]);
        log(typeless_log, buffer);
    #endif
        delete[] buffer;
        
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &optionsState.anisotropy);
        optionsState.anisotropy = fmin(optionsState.anisotropy, pow(2.0, optionsState.surfaceQuality));
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glFrontFace(GL_CCW);
    }
    
    networkManager.init();
    objectManager.init();
    
    SDL_Event event;
    while(true) {
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_WINDOWEVENT:
                    switch (event.window.event) {
                        case SDL_WINDOWEVENT_FOCUS_LOST:
                            if(menu.current == Menu::inGame) {
                                menu.setGameEscMenu();
                                menu.sendPauseEvent();
                            }
                        break;
                        case SDL_WINDOWEVENT_RESIZED:
                            menu.updateVideoResulution();
                        break;
                    }
                break;
                case SDL_KEYDOWN: {
                    SDL_Keycode keycode = event.key.keysym.sym;
                    if(SDL_PeepEvents(&event, 1, SDL_GETEVENT, SDL_TEXTINPUT, SDL_TEXTINPUT))
                        menu.handleKeyDown(keycode, event.text.text);
                    else
                        menu.handleKeyDown(keycode, "");
                } break;
                case SDL_KEYUP:
                    menu.handleKeyUp(event.key.keysym.sym);
                break;
                case SDL_MOUSEBUTTONDOWN:
                    menu.handleMouseDown(event.button.button);
                break;
                case SDL_MOUSEBUTTONUP:
                    menu.handleMouseUp(event.button.button);
                break;
                case SDL_MOUSEWHEEL:
                    menu.handleMouseWheel(event.wheel.x, event.wheel.y);
                break;
                case SDL_MOUSEMOTION: {
                    int mouseX = event.motion.x*optionsState.videoScale - menu.screenView->width;
                    int mouseY = menu.screenView->height - event.motion.y*optionsState.videoScale;
                    if(menu.current == Menu::inGame && menu.mouseFixed) {
                        menu.mouseVelocityX += optionsState.mouseSensitivity*mouseX;
                        menu.mouseVelocityY += optionsState.mouseSensitivity*mouseY;
                    }else{
                        menu.mouseX = mouseX;
                        menu.mouseY = mouseY;
                        menu.screenView->handleMouseMove(menu.mouseX, menu.mouseY);
                    }
                } break;
                case SDL_QUIT:
                    AppTerminate();
                break;
                default:
                break;
            }
        }
        keyState = SDL_GetKeyboardState(&keyStateSize);
        
        if(menu.current == Menu::Name::inGame && profiler.isFirstFrameInSec()) {
            char str[64];
            sprintf(str, "FPS: %d", profiler.FPS);
            menu.consoleAdd(str, 1.0);
        }
        
        networkManager.gameTick();
        menu.gameTick();
        if(levelManager.gameStatus == noGame) {
            glClearColor(1, 1, 1, 1);
            glViewport(0, 0, optionsState.videoWidth*optionsState.videoScale, optionsState.videoHeight*optionsState.videoScale);
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glClear(GL_COLOR_BUFFER_BIT);
        }else{
            profiler.leaveSection("Rest");
            objectManager.gameTick();
        }
        menu.screenView->drawScreen();
        SDL_GL_SwapWindow(mainWindow);
        profiler.leaveSection("Swap Buffers");
        profiler.markFrame();
    }
}

void AppTerminate() {
    optionsState.saveOptions();
    SDL_GL_DeleteContext(glContext);
    SDL_SetWindowFullscreen(mainWindow, 0);
    SDL_DestroyWindow(mainWindow);
    TTF_Quit();
    SDL_Quit();
    exit(0);
}