//
//  AppMain.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef AppMain_h
#define AppMain_h

#include "Scripting/ScriptManager.h"

//! Returns the highest available video scale factor of the mainWindow
float getMaxVideoScale();

//! Initializes and starts the main run loop
void AppMain();

//! Does one game tick (main run loop)
void AppGameTick();

//! Saves the options, cleans up and quits
void AppTerminate();

#endif
