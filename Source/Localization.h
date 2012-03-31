//
//  Localization.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Cam.h"
#import "rapidxml.hpp"
#import <map>
#import <vector>
#import <dirent.h>

#ifndef Localization_h
#define Localization_h

std::vector<std::string> getLocalizableLanguages();
void loadLocalization(const char* language);
const char* localizeString(const char* key);

struct Localization {
    std::map<std::string, std::string> strings;
    std::string name, title;
};

extern Localization localization;

#endif
