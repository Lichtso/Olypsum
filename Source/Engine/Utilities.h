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
#import <string>
#import <map>
#import <vector>
#import <iostream>
#import <fstream>
#import <math.h>
#import <time.h>
#import <dirent.h>
#import <sys/stat.h>
#import <sys/types.h>
#import "rapidxml.hpp"
#import "rapidxml_print.hpp"

#ifndef Utilities_h
#define Utilities_h

#define xmlUsedCharType char
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
char* readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, unsigned int& fileSize, bool logs);
void addXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>* nodes, const char* name, const char* value);
bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath);

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

class GUIColor {
    public:
    unsigned char r, g, b, a;
    GUIColor() : r(0), g(0), b(0), a(255) {};
    GUIColor(unsigned char gray) : r(gray), g(gray), b(gray), a(255) {};
    GUIColor(unsigned char gray, unsigned char aB) : r(gray), g(gray), b(gray), a(aB) {};
    GUIColor(unsigned char rB, unsigned char gB, unsigned char bB) : r(rB), g(gB), b(bB), a(255) {};
    GUIColor(unsigned char rB, unsigned char gB, unsigned char bB, unsigned char aB) : r(rB), g(gB), b(bB), a(aB) {};
    GUIColor& operator=(const GUIColor& B);
    SDL_Color getSDL();
};

extern SDL_Surface* screen;
extern const SDL_VideoInfo* videoInfo;
extern std::string resourcesDir, gameDataDir, parentDir;
extern float mouseTranslation[4];

#endif
