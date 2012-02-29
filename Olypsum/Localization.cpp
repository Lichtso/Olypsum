//
//  Localization.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Localization.h"

std::string languagesDir("Languages/"), languagesExtension(".txt");

std::vector<std::string> getLocalizableLanguages() {
    std::vector<std::string> languages;
    
    DIR* dp = opendir(languagesDir.c_str());
    if(dp == NULL) {
        printf("Error: Languages directory not found.\n");
        return languages;
    }
    
    dirent* ep;
    while((ep = readdir(dp))) {
        if(strlen(ep->d_name) < languagesExtension.length()) continue;
        std::string fileName(ep->d_name);
        std::string fileExtension = fileName.substr(strlen(ep->d_name)-languagesExtension.length(), languagesExtension.length());
        if(fileExtension != languagesExtension) continue;
        languages.push_back(fileName.substr(0, strlen(ep->d_name)-languagesExtension.length()));
    }
    
    closedir(dp);
    return languages;
}

void loadLocalization(char* language) {
    std::string url(languagesDir);
    url += std::string(language);
    url += languagesExtension;
    
    FILE* fp = fopen(url.c_str(), "r");
    if(!fp) {
        printf("Error: No localizations found for language %s.\n", language);
        return;
    }
    
    fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize];
	fread(data, 1, dataSize, fp);
    fclose(fp);
    
    localizationStrings.clear();
    
    unsigned long line = 0;
    char *lastPos = data, *key = NULL;
    for(char* pos = data; pos < data+dataSize; pos ++) {
        if(*pos != '\n') continue;
        if(pos-lastPos < 3) {
            printf("Warning: Too short key or value in language file %s, line %lu.\n", language, line);
            continue;
        }
        *pos = 0;
        line ++;
        if(key == NULL) {
            key = lastPos;
        }else{
            localizationStrings[std::string(key)] = std::string(lastPos+1);
            key = NULL;
        }
        lastPos = pos+1;
    }
}

const char* localizeString(char* key) {
    std::map<std::string, std::string>::iterator iterator = localizationStrings.find(std::string(key));
    if(iterator == localizationStrings.end()) {
        printf("Error: No localization found for key: %s.\n", key);
        return key;
    }
    return iterator->second.c_str();
}

std::map<std::string, std::string> localizationStrings;