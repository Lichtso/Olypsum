//
//  SDLMain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.09.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "SDLMain.h"

@implementation SDLMain

- (void)applicationWillTerminate:(NSNotification *)notification {
    AppTerminate();
}

- (void)applicationDidFinishLaunching:(NSNotification *)note {
    AppMain();
}

float getMaxVideoScale() {
    return [[NSScreen mainScreen] backingScaleFactor];
}

@end