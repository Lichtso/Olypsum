//
//  Localization.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "XMLUtilities.h"

#ifndef Localization_h
#define Localization_h

class Localization {
    public:
    std::map<std::string, std::string> strings;
    std::string selected, title;
    bool getLocalizableLanguages(std::vector<std::string>& languages);
    bool loadLocalization(std::string filePath);
    std::string localizeString(const char* key);
};

extern Localization localization;

#endif
