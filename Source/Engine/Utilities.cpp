//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Utilities.h"

char* parseXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const char* filePath, unsigned int& fileSize) {
    FILE* fp = fopen(filePath, "r");
    if(!fp) {
        printf("The file %s couldn't be found.", filePath);
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

void msleep(unsigned long microsec) {
    struct timespec time;
    time.tv_sec = 0;
    time.tv_nsec = microsec * 1000;
    while(nanosleep(&time, &time) == -1) continue;
}

float animationFactor;
std::string resourcesDir, gameDataDir, parentDir;