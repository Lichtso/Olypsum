//
//  InputController.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "InputController.h"

void handleKeyDown(SDL_keysym* key) {
    
}

void handleKeyUp(SDL_keysym* key) {
    if(key->sym == SDLK_ESCAPE) {
        switch(currentMenu) {
            case mainMenu:
                AppTerminate();
                break;
            case inGameMenu:
                setMenu(gameEscMenu);
                break;
            case gameEscMenu:
                setMenu(inGameMenu);
                break;
        }
    }
}


void handleMouseDown(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseUp(SDL_MouseButtonEvent* mouse) {
    
}

void handleMouseMove(SDL_MouseMotionEvent* mouse) {
    //float x = 0.5-0.5*mouse->x/currentScreenView->width,
    //      y = 0.5-0.5*mouse->y/currentScreenView->height;
    
    
}