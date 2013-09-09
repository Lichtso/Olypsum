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

void restartApplication() {
    std::system((executablePath+std::string(" &")).c_str());
    AppTerminate();
}

int main(int argc, const char** argv) {
    resourcesPath = argv[0];
    AppMain();
    return 0;
}

#endif