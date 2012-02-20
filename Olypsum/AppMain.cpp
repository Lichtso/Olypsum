//
//  AppMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "AppMain.h"

SDL_Surface* screen;
const SDL_VideoInfo* videoInfo;

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
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    //SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    
    screen = SDL_SetVideoMode(videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel, SDL_OPENGL | SDL_FULLSCREEN);
    if(!screen) {
        printf("Coudn't set fullscreen, Quit.\n");
        exit(3);
    }
    glClearColor(1, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    SDL_GL_SwapBuffers();
    
    while(true) {
        SDL_Event event;
        while(SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_KEYDOWN:
                    handleKeyDown(&event.key.keysym);
                    break;
                case SDL_KEYUP:
                    handleKeyUp(&event.key.keysym);
                    break;
                case SDL_MOUSEBUTTONDOWN:
                    handleMouseDown(&event.button);
                    break;
                case SDL_MOUSEBUTTONUP:
                    handleMouseUp(&event.button);
                    break;
                case SDL_MOUSEMOTION:
                    handleMouseMove(&event.motion);
                    break;
                case SDL_QUIT:
                    SDL_Quit();
                    exit(0);
                    break;
                default:
                    break;
            }
        }
        updateKeyState();
    }
}