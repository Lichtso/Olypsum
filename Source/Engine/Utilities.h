//
//  Utilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <math.h>
#import <dirent.h>
#import <sys/stat.h>
#import <map>
#import <string>
#import <vector>

#ifndef Utilities_h
#define Utilities_h

#define VERSION "0.0.1"

enum logMessageType {
    info_log = 0,
    warning_log = 1,
    error_log = 2,
    shader_log = 3,
    script_log = 4
};

void log(logMessageType type, std::string message);
bool checkDir(std::string path);
bool createDir(std::string path);
bool scanDir(std::string path, std::vector<std::string>& files);
bool removeDir(std::string path);

template <class T> inline const T max(T a, T b) {
	return (b<a)?a:b;
}

template <class T> inline const T min(T a, T b) {
	return (b>a)?a:b;
}

template <class T> inline const T clamp(T x, T a, T b) {
	return (x<a)?a:((x>b)?b:x);
}

template <class T> inline const T frand(T min, T max) {
    return ((max-min)*((float)rand()/RAND_MAX))+min;
}

extern int screenSize[3];
extern std::string resourcesDir, gameDataDir, parentDir;
extern float mouseTranslation[2];

#endif
