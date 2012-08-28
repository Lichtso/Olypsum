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
    }
}

void createDir(std::string path) {
    DIR* dir = opendir(path.c_str());
    if(dir)
        closedir(dir);
    else
        mkdir(path.c_str(), S_IRWXU | S_IRWXG);
}

char* readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, unsigned int& fileSize, bool logs) {
    FILE* fp = fopen(filePath.c_str(), "r");
    if(!fp) {
        if(!logs) return NULL;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be found.");
        return NULL;
    }
    
    fseek(fp, 0, SEEK_END);
	fileSize = ftell(fp);
	rewind(fp);
    char* data = new char[fileSize+1];
	fread(data, 1, fileSize, fp);
    fclose(fp);
    data[fileSize] = 0;
    doc.parse<0>(data);
    
    return data;
}

bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath) {
    std::ofstream outfile;
    outfile.open(filePath.c_str(), std::ios_base::trunc);
    outfile << doc;
    outfile.close();
    doc.clear();
    return true;
}

GUIColor& GUIColor::operator=(const GUIColor& B) {
    r = B.r;
    g = B.g;
    b = B.b;
    a = B.a;
    return *this;
}

SDL_Color GUIColor::getSDL() {
    SDL_Color B;
    B.r = r;
    B.g = g;
    B.b = b;
    B.unused = a;
    return B;
}

SDL_Surface* screen;
const SDL_VideoInfo* videoInfo;
std::string resourcesDir, gameDataDir, parentDir;
float animationFactor, mouseTranslation[4] = { 1.0, 1.0, 0.0, 0.0 };