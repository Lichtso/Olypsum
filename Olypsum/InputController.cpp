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
    
}

void handleKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE)
        AppTerminate();
    
}



void handleMouseDown(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseUp(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseMove(SDL_MouseMotionEvent* mouse) {
    
}