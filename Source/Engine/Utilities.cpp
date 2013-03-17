//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Controls.h"

void log(logMessageType type, std::string message) {
    std::string typeStr;
    switch(type) {
        case info_log:
            typeStr = "INFO: ";
            break;
        case warning_log:
            typeStr = "WARNING: ";
            break;
        case error_log:
            typeStr = "ERROR: ";
            break;
        case shader_log:
            typeStr = "SHADER-";
            break;
        case script_log:
            typeStr = "SCRIPT: ";
            break;
    }
    
    message = typeStr+message;
    printf("%s\n", message.c_str());
    if(controlsMangager)
        controlsMangager->consoleAdd(message);
}

int getFileSize(const std::string& filePath) {
    std::ifstream file;
    file.open(filePath.c_str());
    if(!file.is_open())
        return -1;
    unsigned int fileSize = file.tellg();
    file.close();
    return fileSize;
}

std::unique_ptr<char[]> readFile(const std::string& filePath, bool logs) {
    std::ifstream file;
    file.open(filePath.c_str(), std::ios::ate);
    if(!file.is_open()) {
        if(!logs) return NULL;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be found.");
        return NULL;
    }
    unsigned int fileSize = file.tellg();
    std::unique_ptr<char[]> data(new char[fileSize+1]);
    file.seekg(0, std::ios::beg);
    file.read(data.get(), fileSize);
    file.close();
    data[fileSize] = 0;
    return data;
}

bool writeFile(const std::string& filePath, const std::string& content, bool logs) {
    std::ofstream file;
    file.open(filePath.c_str(), std::ios_base::trunc);
    if(!file.is_open()) {
        if(!logs) return false;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be opened.");
        return false;
    }
    file << content;
    file.close();
    return true;
}

bool checkDir(std::string path) {
    DIR* dir = opendir(path.c_str());
    if(dir) {
        closedir(dir);
        return true;
    }else
        return false;
}

bool createDir(std::string path) {
    if(checkDir(path))
        return false;
    else{
        mkdir(path.c_str(), S_IRWXU | S_IRWXG);
        return true;
    }
}

bool scanDir(std::string path, std::vector<std::string>& files) {
    DIR* dir = opendir(path.c_str());
    if(dir) {
        dirent* file;
        while((file = readdir(dir))) {
            if(file->d_namlen < 3 && strncmp(file->d_name, "..", file->d_namlen) == 0) continue;
            std::string name(file->d_name, file->d_namlen);
            if(file->d_type == DT_DIR) name += '/';
            files.push_back(name);
        }
        closedir(dir);
        return true;
    }else
        return false;
}

bool removeDir(std::string path) {
    DIR* dir = opendir(path.c_str());
    if(dir) {
        dirent* file;
        while((file = readdir(dir))) {
            if(file->d_namlen < 3 && strncmp(file->d_name, "..", file->d_namlen) == 0) continue;
            std::string filePath = path+std::string(file->d_name, file->d_namlen);
            if(file->d_type == DT_DIR)
                removeDir(filePath+'/');
            else
                remove(filePath.c_str());
        }
        closedir(dir);
        return remove(path.c_str()) == 0;
    }else
        return false;
}

std::string stringOf(int value) {
    char buffer[32];
    sprintf(buffer, "%d", value);
    return buffer;
}

std::string stringOf(unsigned int value) {
    char buffer[32];
    sprintf(buffer, "%d", value);
    return buffer;
}

std::string stringOf(float value) {
    char buffer[32];
    sprintf(buffer, "%g", value);
    return buffer;
}

std::string stringOf(btVector3& vec) {
    char buffer[128];
    sprintf(buffer, "%g %g %g", vec.x(), vec.y(), vec.z());
    return buffer;
}

std::string stringOf(btQuaternion& rot) {
    char buffer[128];
    sprintf(buffer, "%g %g %g %g", rot.x(), rot.y(), rot.z(), rot.w());
    return buffer;
}

std::string stringOf(btTransform& mat) {
    btScalar values[16];
    mat.getOpenGLMatrix(values);
    
    std::ostringstream ss;
    ss << "(";
    for(unsigned char i = 0; i < 16; i ++) {
        if(i % 4 > 0) ss << " ";
        ss << values[i];
        if(i % 4 == 3 && i < 15) ss << ",\n ";
    }
    ss << ")";
    return ss.str();
}

int screenSize[3];
std::string resourcesDir, gameDataDir, parentDir;