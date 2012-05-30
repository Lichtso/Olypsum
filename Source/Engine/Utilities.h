//
//  Utilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenGL/gl.h>
#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import <SDL/SDL.h>
#import <SDL_image/SDL_image.h>
#import <SDL/SDL_thread.h>
#import <SDL/SDL_mutex.h>
#import <BulletDynamics/btBulletDynamicsCommon.h>
#import <math.h>
#import <time.h>
#import <string>
#import "rapidxml.hpp"
#import <map>
#import <vector>
#import <dirent.h>

#ifndef Utilities_h
#define Utilities_h

#define xmlUsedCharType char
#define ENGINE_NAME "Olypsum-Engine 0.0.1"

char* parseXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const char* filePath, unsigned int& fileSize);
void msleep(unsigned long milisec);

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

extern std::string resourcesDir, gameDataDir, parentDir;
extern float animationFactor;

#endif