//
//  Localization.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Cam.h"
#import <map>
#import <vector>
#import <sys/types.h>
#import <dirent.h>

#ifndef Localization_h
#define Localization_h

std::vector<std::string> getLocalizableLanguages();
void loadLocalization(char* language);
const char* localizeString(char* key);

extern std::map<std::string, std::string> localizationStrings;

#endif
