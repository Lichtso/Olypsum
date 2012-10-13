//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Utilities.h"

void log(logMessageType type, std::string message) {
    switch(type) {
        case info_log:
            printf("INFO: %s\n", message.c_str());
            break;
        case warning_log:
            printf("WARNING: %s\n", message.c_str());
            break;
        case error_log:
            printf("ERROR: %s\n", message.c_str());
            break;
        case shader_log:
            printf("SHADER: %s\n", message.c_str());
            break;
        case script_log:
            printf("SCRIPT: %s\n", message.c_str());
            break;
    }
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
                removeDir(filePath);
            else
                remove(filePath.c_str());
        }
        closedir(dir);
        return remove(path.c_str()) == 0;
    }else
        return false;
}

int screenSize[3];
std::string resourcesDir, gameDataDir, parentDir;
float mouseTranslation[2] = { 0.0, 0.0 };