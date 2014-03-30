//
//  main.mm
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "SDLMain.h"

int main(int argc, const char** argv) {
    NSString* aux = [[[NSBundle mainBundle] infoDictionary] objectForKey:@"CFBundleShortVersionString"];
    engineVersion = new char[64];
    [aux getCString:const_cast<char*>(engineVersion) maxLength:64 encoding:NSUTF8StringEncoding];
    
    resourcesPath = argv[0];
    return NSApplicationMain(argc, argv);
}