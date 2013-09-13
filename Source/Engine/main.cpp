//
//  main.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 22.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef __APPLE__
#include "AppMain.h"

float getMaxVideoScale() {
    return 1.0;
}

int main(int argc, const char** argv) {
    resourcesPath = argv[0];
    AppMain();
    return 0;
}

#endif