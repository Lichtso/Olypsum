//
//  Localization.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Localization.h"

#define xmlUsedCharType char
std::string languagesDir("Languages/"), languagesExtension(".xml");

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

void loadLocalization(const char* language) {
    localization.name = std::string(language);
    
    std::string url(languagesDir);
    url += localization.name;
    url += languagesExtension;
    
    FILE* fp = fopen(url.c_str(), "r");
    if(!fp) {
        printf("Error: No localizations found for language %s.\n", language);
        return;
    }
    localization.strings.clear();
    
    fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize+1];
	fread(data, 1, dataSize, fp);
    fclose(fp);
    data[dataSize] = 0;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType> *rootNode, *titleNode, *localizationNode, *entryNode;
    rapidxml::xml_attribute<xmlUsedCharType> *entryKeyAttribute;
    doc.parse<0>(data);
    
    rootNode = doc.first_node("language");
    if(!rootNode) goto endParsingXML;
    
    titleNode = rootNode->first_node("title");
    if(!titleNode) goto endParsingXML;
    localization.title = std::string(titleNode->value());
    
    localizationNode = rootNode->first_node("localization");
    if(!localizationNode) goto endParsingXML;
    
    entryNode = localizationNode->first_node("entry");
    while(entryNode) {
        entryKeyAttribute = entryNode->first_attribute("key");
        if(!entryKeyAttribute) goto endParsingXML;
        
        localization.strings[std::string(entryKeyAttribute->value())] = std::string(entryNode->value());
        entryNode = entryNode->next_sibling("entry");
    }
    
    endParsingXML:
    doc.clear();
}

const char* localizeString(const char* key) {
    std::map<std::string, std::string>::iterator iterator = localization.strings.find(std::string(key));
    if(iterator == localization.strings.end()) {
        printf("Error: No localization found for key: %s.\n", key);
        return key;
    }
    return iterator->second.c_str();
}

Localization localization;