//
//  Utilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <BulletCollision/btBulletCollisionCommon.h>
#import <BulletDynamics/btBulletDynamicsCommon.h>
#import <BulletSoftBody/btSoftBody.h>
#import <math.h>
#import <dirent.h>
#import <sys/stat.h>
#import <map>
#import <set>
#import <vector>
#import <string>
#import <sstream>

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

//! Converts a int into a std::string
std::string stringOf(int value);

//! Converts a float into a std::string
std::string stringOf(float value);

//! Converts a btVector3 into a std::string
std::string stringOf(btVector3& vec);

//! Converts a btTransform into a std::string
std::string stringOf(btTransform& mat);

/*! Converts a any value into a std::string
 @param format A string to be used for conversion like in printf()
 @param value The value to be converted
 @return The converted std::string
 */
template <class T> std::string stringOf(const char* format, T value) {
    char buffer[64];
    sprintf(buffer, format, value);
    return buffer;
}

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
    return ((max-min)*((T)rand()/RAND_MAX))+min;
}

inline btVector3 vec3rand(btVector3 min, btVector3 max) {
    return btVector3(frand(min.x(), max.x()), frand(min.y(), max.y()), frand(min.z(), max.z()));
}

extern int screenSize[3];
extern std::string resourcesDir, gameDataDir, parentDir;
extern float mouseTranslation[2];

#endif
