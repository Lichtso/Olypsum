//
//  InputController.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "InputController.h"
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
    if(currentScreenView)
        currentScreenView->handleMouseDown(mouse->x, mouse->y);
}

void handleMouseUp(SDL_MouseButtonEvent* mouse) {
    if(currentScreenView)
        currentScreenView->handleMouseUp(mouse->x, mouse->y);
}

void handleMouseMove(SDL_MouseMotionEvent* mouse) {
    if(currentScreenView)
        currentScreenView->handleMouseMove(mouse->x, mouse->y);
}