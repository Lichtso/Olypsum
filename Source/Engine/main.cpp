//
//  main.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef __APPLE__
#include "AppMain.h"

void updateVideoMode() {
    bool fullScreen;
    screenSize[2] = 1;
    const SDL_VideoInfo* videoInfo = updateVideoModeInternal(fullScreen);
    SDL_Surface* screen = SDL_SetVideoMode(optionsState.videoWidth,
                                           optionsState.videoHeight,
                                           videoInfo->vfmt->BitsPerPixel,
                                           (fullScreen) ? SDL_OPENGL | SDL_FULLSCREEN : SDL_OPENGL);
    if(!screen) {
        log(error_log, "Coudn't set video mode, Quit.");
        exit(4);
    }
}

void setClipboardText(const char* str) {
    
}

std::string getClipboardText() {
    return "";
}

bool hasClipboardText() {
    return false;
}

void openExternURL(const char* str) {
    
}

void restartApplication() {
    AppTerminate();
}

int main(int argc, const char** argv) {
    NL::init();
    
    resourcesDir = argv[0];
    if(argv[0][0] != '/') {
        char cwdPath[512];
        getcwd(cwdPath, sizeof(cwdPath)/sizeof(char)-1);
        resourcesDir = std::string(cwdPath)+'/'+resourcesDir;
    }
    resourcesDir = trimPath(resourcesDir.c_str(), 2)+"/Resources/";
    gameDataDir = std::string(getenv("HOME"))+"/.Gamefortec/";
    
    AppMain();
    return 0;
}

#endif