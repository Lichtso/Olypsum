//
//  main.mm
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "SDLMain.h"
#undef main

int main(int argc, const char** argv) {
    resourcesPath = argv[0];
    return NSApplicationMain(argc, argv);
}