//
//  InputController.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "AppMain.h"

Uint8* keyState;
SDLMod modKeyState;

void updateKeyState() {
    keyState = SDL_GetKeyState(NULL);
    modKeyState = SDL_GetModState();
}

void handleKeyDown(SDL_keysym* key) {
    if(currentScreenView && currentScreenView->handleKeyDown(key))
        return;
    
}

void handleKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        AppTerminate();
        return;
    }
    
    if(currentScreenView && currentScreenView->handleKeyUp(key))
        return;
    
}



void handleMouseDown(SDL_MouseButtonEvent* mouse) {
    if(currentScreenView) {
        switch(mouse->button) {
            case SDL_BUTTON_LEFT:
                currentScreenView->handleMouseDown(mouse->x, mouse->y);
            break;
            case SDL_BUTTON_WHEELDOWN:
                currentScreenView->handleMouseWheel(mouse->x, mouse->y, -1.0);
            break;
            case SDL_BUTTON_WHEELUP:
                currentScreenView->handleMouseWheel(mouse->x, mouse->y, 1.0);
            break;
        }
    }
}

void handleMouseUp(SDL_MouseButtonEvent* mouse) {
    if(currentScreenView)
        currentScreenView->handleMouseUp(mouse->x, mouse->y);
}

void handleMouseMove(SDL_MouseMotionEvent* mouse) {
    if(currentScreenView)
        currentScreenView->handleMouseMove(mouse->x, mouse->y);
    
    float x = 0.5-0.5*mouse->x/currentScreenView->width,
          y = 0.5-0.5*mouse->y/currentScreenView->height;
    
    //TODO: Skeleton-Test
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.rotateX(y*1.5);
    skeletonPose->bonePoses["Upperarm_Right"]->poseMat.rotateY(2.0*x-1.0);
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.setIdentity();
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.rotateZ(-1.5*y);
    skeletonPose->bonePoses["Upperarm_Left"]->poseMat.rotateY(2.0*x+1.0);
    
}