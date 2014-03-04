//
//  Header.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.03.14.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Header_h
#define Header_h

#define V8_ALLOW_ACCESS_TO_RAW_HANDLE_CONSTRUCTOR
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#define __BT_SKIP_UINT64_H
#define _USE_MATH_DEFINES
#include <math.h>
#include <SDKDDKVer.h>
#include <windows.h>
#include <direct.h>
#include <GL/glew.h>
#include <OpenAL/include/al.h>
#include <OpenAL/include/alc.h>
#include <SDL2/include/SDL.h>
#include <SDL2_image/include/SDL_image.h>
#include <SDL2_ttf/include/SDL_ttf.h>
#include <libvorbis/include/vorbis/vorbisfile.h>
#pragma comment(lib, "opengl32")
#pragma comment(lib, "glu32")
#pragma comment(lib, "glew32")
#pragma comment(lib, "LinearMath")
#pragma comment(lib, "BulletCollision")
#pragma comment(lib, "BulletDynamics")
#pragma comment(lib, "BulletMultiThreaded")
#pragma comment(lib, "BulletSoftBody")
#pragma comment(lib, "SDL2")
#pragma comment(lib, "SDL2_image")
#pragma comment(lib, "SDL2_ttf")
#pragma comment(lib, "v8")
#pragma comment(lib, "v8_base.x64.lib")
#pragma comment(lib, "v8_snapshot")
#pragma comment(lib, "OpenAL32")
#pragma comment(lib, "OpenCL")
#pragma comment(lib, "libogg_static")
#pragma comment(lib, "libvorbis_static")
#pragma comment(lib, "libvorbisfile_static")
#pragma comment(lib, "netLink")
#define mkdir _mkdir
#define rmdir _rmdir
#define getcwd _getcwd
#define popen _popen
#define pclose _pclose
#define SYSTEM_SLASH '\\'
#else
#define SYSTEM_SLASH '/'
#include <SDL2/SDL.h>
#include <vorbis/vorbisfile.h>
#include <dirent.h>
#include <sys/time.h>
#ifdef __APPLE__
#define GL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED
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
#endif
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <regex>
#include <unordered_map>
#include <unordered_set>

#include <bulletPhysics/src/btBulletCollisionCommon.h>
#include <bulletPhysics/src/btBulletDynamicsCommon.h>
#include <bulletPhysics/src/BulletSoftBody/btSoftRigidDynamicsWorld.h>
#include <bulletPhysics/src/BulletSoftBody/btDefaultSoftBodySolver.h>
#include <bulletPhysics/src/BulletSoftBody/btSoftBodyRigidBodyCollisionConfiguration.h>
#include <bulletPhysics/src/BulletMultiThreaded/btParallelConstraintSolver.h>
#include <bulletPhysics/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolver_OpenCLSIMDAware.h>
#include <bulletPhysics/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolverVertexBuffer_OpenGL.h>
#include <bulletPhysics/src/BulletMultiThreaded/GpuSoftBodySolvers/OpenCL/btSoftBodySolverOutputCLtoGL.h>
#include <netLink/include/netLink.h>
#include <v8/include/v8.h>

#endif