//
//  AppMain.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef AppMain_h
#define AppMain_h

#include "ScriptManager.h"

void openExternURL(const char* str);
void setClipboardText(const char* str);
std::string getClipboardText();
bool hasClipboardText();

const SDL_VideoInfo* updateVideoModeInternal(bool& fullScreen);
void updateVideoMode();

void AppMain(int argc, char *argv[]);
void AppTerminate();

extern Uint8* keyState;
extern int keyStateSize;

#endif
