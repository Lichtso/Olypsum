//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Utilities.h"

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
        printf("The file %s couldn't be found.", filePath.c_str());
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

void msleep(unsigned long microsec) {
    struct timespec time;
    time.tv_sec = 0;
    time.tv_nsec = microsec * 1000;
    while(nanosleep(&time, &time) == -1) continue;
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