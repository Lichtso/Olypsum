//
//  Utilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Utilities_h
#define Utilities_h

//#include <math.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fstream>
#include <thread>
#include <regex>
#include <unordered_map>
#include <unordered_set>

#ifdef __APPLE__
#include <OpenGL/gl3.h>
#include <OpenAL/al.h>
#include <OpenAL/alc.h>
#include <SDL2_image/SDL_image.h>
#include <SDL2_ttf/SDL_ttf.h>
#else
#define GL_GLEXT_PROTOTYPES
#include <GL/gl.h>
#include <AL/al.h>
#include <AL/alc.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#endif

#include <SDL2/SDL.h>
#include <BulletCollision/btBulletCollisionCommon.h>
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <BulletSoftBody/btDefaultSoftBodySolver.h>
#include <BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
//#include <BulletMultiThreaded/PosixThreadSupport.h>
#include <BulletMultiThreaded/btParallelConstraintSolver.h>
#include <BulletMultiThreaded/btSoftBodySolver_OpenCLSIMDAware.h>
#include <BulletMultiThreaded/btSoftBodySolverVertexBuffer_OpenGL.h>
#include <BulletMultiThreaded/btSoftBodySolverOutputCLtoGL.h>
#include <Leap.h>
#include <netLink.h>
#define V8_USE_UNSAFE_HANDLES 1
#include <v8.h>

enum logMessageType {
    info_log = 0,
    warning_log,
    error_log,
    shader_log,
    script_log
};

/*! Logs a message to console and stdout
 @param type Type of the message
 @param message Message to be loged
 */
void log(logMessageType type, std::string message);

/*! Reads the size of a file
 @param filePath The file to be read
 @return file size in bytes or -1 if file does not exist
 */
int getFileSize(const std::string& filePath);

/*! Reads the content of a file
 @param filePath The file to be read
 @param logs Enables console logs if a error occurs
 @return A string with the raw content of the file
 */
std::unique_ptr<char[]> readFile(const std::string& filePath, bool logs);

/*! Writes content in a file
 @param filePath The file to be written
 @param content The content to be written
 @param logs Enables console logs if a error occurs
 @return Success
 */
bool writeFile(const std::string& filePath, const std::string& content, bool logs);

/*! Calculates the hash value of a file
 @param filePath Input file path
 @return Output hash value
 */
std::size_t hashFile(const std::string& filePath);

//! Checks for a directory at a given path
bool checkDir(std::string path);
/*! Creates a directory at a given path
 @return Success
 */
bool createDir(std::string path);
/*! Calls a function on each file (that isn't hidden by '.') and subdirectory (name ends with '/') in the given directory.
 The fies and subdirectories are processed in alphabetical order.
 @param path Directory path
 @param perFile Called per file
 @param enterDirectory Called at entering a subdirectory, return value has to be true to process recursivly
 @param leaveDirectory Called at leaving a directory
 @return Success
 */
bool forEachInDir(std::string path,
                  std::function<void(const std::string& directoryPath, std::string name)> perFile,
                  std::function<bool(const std::string& directoryPath, std::string name)> enterDirectory,
                  std::function<void(const std::string& directoryPath)> leaveDirectory);
/*! Removes a directory at a given path
 @return Success
 */
bool removeDir(std::string path);

/*! Normalizes the path and removes n directory names
 @param path The path to trim
 @param n How many directory names to remove from the end
 */
std::string trimPath(std::string path, size_t n);

//! Converts a int into a std::string
std::string stringOf(int value);
std::string stringOf(unsigned int value);

//! Converts a float into a std::string
std::string stringOf(float value);

//! Converts a btVector3 into a std::string
std::string stringOf(btVector3& vec);

//! Converts a btQuaternion into a std::string
std::string stringOf(btQuaternion& rot);

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

//! Generates a random number between min and max
template <class T> inline const T frand(T min, T max) {
    return ((max-min)*((T)rand()/RAND_MAX))+min;
}

//! Generates a random btVector3 between min and max
inline btVector3 vec3rand(btVector3 min, btVector3 max) {
    return btVector3(frand(min.x(), max.x()), frand(min.y(), max.y()), frand(min.z(), max.z()));
}

//! Convertes a Leap::Vector to a btVector3
inline btVector3 leapToBullet(Leap::Vector vec) {
    return btVector3(vec.x, vec.y, vec.z);
}

//! System time in seconds
double getTime();

/*! Compares two version strings
 @param a Version string (e.g. "1.0.0")
 @param b Version string (e.g. "1.0.0")
 @return -1: a < b, 0: a == b, 1: a > b
 */
char compareVersions(std::string a, std::string b);

extern const Uint8* keyState;
extern int keyStateSize;
extern SDL_Window* mainWindow;
extern SDL_GLContext glContext;
extern std::string executablePath, resourcesPath, supportPath;

#endif