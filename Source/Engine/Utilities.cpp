//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Menu.h"

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
            typeStr = "SHADER: ";
            break;
        case script_log:
            typeStr = "SCRIPT: ";
            break;
    }
    
    message = typeStr+message;
    printf("%s\n", message.c_str());
    if(levelManager.gameStatus != noGame)
        menu.consoleAdd(message);
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

std::size_t hashFile(const std::string& filePath) {
    std::ifstream file;
    file.open(filePath.c_str(), std::ios_base::binary);
    if(!file.is_open()) return 0;
    std::stringstream stream;
    file >> stream.rdbuf();
    file.close();
    return std::hash<std::string>()(stream.str());
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

bool forEachInDir(std::string path,
                  std::function<void(const std::string& directoryPath, std::string name)> perFile,
                  std::function<bool(const std::string& directoryPath, std::string name)> enterDirectory,
                  std::function<void(const std::string& directoryPath)> leaveDirectory) {
    DIR* dir = opendir(path.c_str());
    if(!dir) return false;
    
    dirent* item;
    std::set<std::string> items;
    while((item = readdir(dir))) {
        std::string name(item->d_name);
        if(*name.begin() == '.') continue;
        if(item->d_type == DT_DIR)
            name += '/';
        items.insert(name);
    }
    
    for(std::string name : items) {
        if(*(name.end()-1) == '/') {
            if(enterDirectory && enterDirectory(path, name))
                forEachInDir(path+name, perFile, enterDirectory, leaveDirectory);
        }else if(perFile)
            perFile(path, name);
    }
    
    closedir(dir);
    if(leaveDirectory)
        leaveDirectory(path);
    return true;
}

bool removeDir(std::string path) {
    return forEachInDir(path, [](const std::string& directoryPath, std::string name) {
        remove((directoryPath+name).c_str());
    }, [](const std::string& directoryPath, std::string name) {
        return true;
    }, [](const std::string& directoryPath) {
        remove((directoryPath).c_str());
    });
}

std::string trimPath(std::string path, size_t n) {
    std::stringstream ss(path);
    std::vector<std::string> tokens;
    std::string token;
    
    while(std::getline(ss, token, '/')) {
        if(token == ".") continue;
        
        if(tokens.size() > 0) {
            if(token.size() == 0) continue;
            
            if(token == ".." && tokens[tokens.size()-1] != "..") {
                tokens.pop_back();
                continue;
            }
        }
        
        tokens.push_back(token);
    }
    
    ss.clear();
    ss.str("");
    for(size_t i = 0; i < tokens.size()-n; i ++) {
        if(i > 0)
            ss << '/';
        ss << tokens[i];
    }
    
    return ss.str();
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

double getTime() {
#ifdef OS_WIN32
    SYSTEMTIME now;
    GetSystemTime(&now);
    return now.wHour*3600.0 + now.wMinute*60.0 + now.wSecond + now.wMilliseconds/1000.0;
#else
    struct timeval now;
    gettimeofday(&now, NULL);
    return now.tv_sec + now.tv_usec/1000000.0;
#endif
}

char compareVersions(std::string strA, std::string strB) {
    std::stringstream ss(strA);
    std::vector<int> a;
    std::string token;
    while(std::getline(ss, token, '.')) {
        int number;
        sscanf(token.c_str(), "%d", &number);
        a.push_back(number);
    }
    
    ss.clear();
    ss.str(strB);
    int i = 0;
    while(std::getline(ss, token, '.')) {
        int number;
        sscanf(token.c_str(), "%d", &number);
        if(a.size() <= i || a[i] < number)
            return -1;
        else if(a[i] > number)
            return 1;
        i ++;
    }
    
    return (a.size() > i) ? 1 : 0;
}

const Uint8* keyState = NULL;
int keyStateSize = 0;
SDL_Window* mainWindow = NULL;
SDL_GLContext glContext = NULL;
std::string executablePath, resourcesPath, supportPath;