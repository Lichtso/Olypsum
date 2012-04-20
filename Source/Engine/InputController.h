//
//  InputController.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Game.h"

#ifndef InputController_h
#define InputController_h

//Keyboard
void handleKeyDown(SDL_keysym* key);
void handleKeyUp(SDL_keysym* key);

//Mouse
void handleMouseDown(SDL_MouseButtonEvent* mouse);
void handleMouseUp(SDL_MouseButtonEvent* mouse);
void handleMouseMove(SDL_MouseMotionEvent* mouse);

#endif