//
//  Localization.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Localization.h"

std::string LanguagesDir("Packages/Default/Languages/"), LanguagesExtension(".xml");

bool Localization::getLocalizableLanguages(std::vector<std::string>& languages) {
    DIR* dp = opendir(LanguagesDir.c_str());
    if(dp == NULL) {
        log(error_log, "Languages directory not found.");
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
    std::unique_ptr<char[]> fileData = readXmlFile(doc, filePath.c_str(), false);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *localizationNode, *titleNode, *entryNode;
    rapidxml::xml_attribute<xmlUsedCharType> *entryKeyAttribute;
    
    localizationNode = doc.first_node("Localization");
    if(!localizationNode) return false;
    
    titleNode = localizationNode->first_node("Title");
    if(titleNode) title = titleNode->value();
    
    entryNode = localizationNode->first_node("Entry");
    while(entryNode) {
        entryKeyAttribute = entryNode->first_attribute("key");
        if(!entryKeyAttribute) return false;
        
        strings[std::string(entryKeyAttribute->value())] = std::string(entryNode->value());
        entryNode = entryNode->next_sibling("Entry");
    }
    
    return true;
}

std::string Localization::localizeString(const char* key) {
    std::map<std::string, std::string>::iterator iterator = strings.find(std::string(key));
    if(iterator == strings.end()) {
        log(error_log, std::string("No localization found for key: ")+key);
        return key;
    }
    return iterator->second;
}

Localization localization;