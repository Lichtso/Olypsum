//
//  SDLMain.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.09.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef SDLMain_h
#define SDLMain_h
#include "AppMain.h"
#include <AppKit/AppKit.h>

@interface SDLMain : NSObject <NSApplicationDelegate>
    
@end

void updateVideoMode();

#endif