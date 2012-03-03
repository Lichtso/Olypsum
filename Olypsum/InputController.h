//
//  InputController.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "GUITabs.h"

#ifndef InputController_h
#define InputController_h

//Keyboard
extern Uint8* keyState;
extern SDLMod modKeyState;
void updateKeyState();
void handleKeyDown(SDL_keysym* key);
void handleKeyUp(SDL_keysym* key);

//Mouse

void handleMouseDown(SDL_MouseButtonEvent* mouse);
void handleMouseUp(SDL_MouseButtonEvent* mouse);
void handleMouseMove(SDL_MouseMotionEvent* mouse);

#endif
