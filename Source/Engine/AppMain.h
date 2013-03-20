//
//  AppMain.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"
#include "netLink-1.0.3/socket.h"

#ifndef AppMain_h
#define AppMain_h

extern Uint8* keyState;

const SDL_VideoInfo* updateVideoModeInternal(bool& fullScreen);
void updateVideoMode();
void AppMain(int argc, char *argv[]);
void AppTerminate();

#endif
