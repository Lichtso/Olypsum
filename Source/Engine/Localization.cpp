//
//  Localization.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "Localization.h"

std::string LanguagesDir("Packages/Default/Languages/"), LanguagesExtension(".xml");

bool Localization::getLocalizableLanguages(std::vector<std::string>& languages) {
    DIR* dp = opendir(LanguagesDir.c_str());
    if(dp == NULL) {
        printf("Error: Languages directory not found.\n");
        return false;
    }
    
    dirent* ep;
    while((ep = readdir(dp))) {
        if(strlen(ep->d_name) < 4) continue;
        std::string fileName(ep->d_name);
        std::string fileExtension = fileName.substr(strlen(ep->d_name)-LanguagesExtension.size(), LanguagesExtension.size());
        if(fileExtension.compare(LanguagesExtension) != 0) continue;
        languages.push_back(fileName.substr(0, strlen(ep->d_name)-LanguagesExtension.size()));
    }
    
    closedir(dp);
    return true;
}

bool Localization::loadLocalization(std::string filePath) {
    rapidxml::xml_document<xmlUsedCharType> doc;
    unsigned int fileSize;
    char* fileData = readXmlFile(doc, filePath.c_str(), fileSize, false);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *titleNode, *localizationNode, *entryNode;
    rapidxml::xml_attribute<xmlUsedCharType> *entryKeyAttribute;
    
    titleNode = doc.first_node("title");
    if(titleNode) title = titleNode->value();
    
    localizationNode = doc.first_node("localization");
    if(!localizationNode) goto endParsingXML;
    
    entryNode = localizationNode->first_node("entry");
    while(entryNode) {
        entryKeyAttribute = entryNode->first_attribute("key");
        if(!entryKeyAttribute) goto endParsingXML;
        
        strings[std::string(entryKeyAttribute->value())] = std::string(entryNode->value());
        entryNode = entryNode->next_sibling("entry");
    }
    
    endParsingXML:
    doc.clear();
    delete [] fileData;
    return true;
}

const char* Localization::localizeString(const char* key) {
    std::map<std::string, std::string>::iterator iterator = strings.find(std::string(key));
    if(iterator == strings.end()) {
        printf("Error: No localization found for key: %s.\n", key);
        return key;
    }
    return iterator->second.c_str();
}

Localization localization;