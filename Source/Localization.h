//
//  Localization.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Cam.h"

#ifndef Localization_h
#define Localization_h

bool parseXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const char* filePath);

class Localization {
    public:
    std::map<std::string, std::string> strings;
    std::string title;
    std::vector<std::string> getLocalizableLanguages();
    bool loadLocalization(const char* filePath);
    const char* localizeString(const char* key);
};

extern Localization localization;

#endif
