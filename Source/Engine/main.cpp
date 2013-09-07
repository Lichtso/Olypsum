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

int main(int argc, char *argv[]) {
    NL::init();
    
    std::string companyDir = "~/.Gamefortec/";
    gameDataDir = companyDir+"Olypsum/";
    
    createDir(companyDir);
    createDir(gameDataDir);
    
    resourcesDir = std::string(???)+"Resources/";
    chdir(resourcesDir.c_str());
    
    AppMain(argc, argv);
    return 0;
}

#endif