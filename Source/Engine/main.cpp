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

#ifdef WIN32
/*int WINAPI WinMain(HINSTANCE hInstance,
                   HINSTANCE hPrevInstance,
                   LPSTR lpCmdLine,
                   int nCmdShow) {
    log(info_log, "Windows start");
	//AppMain();
	return 0;
}*/
int main(int argc, const char** argv) {
    resourcesPath = argv[0];
    log(info_log, std::string("Windows start: ")+resourcesPath);
    return 0;
}
#else
int main(int argc, const char** argv) {
    resourcesPath = argv[0];
    AppMain();
    return 0;
}
#endif

#endif