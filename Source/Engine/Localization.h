//
//  Localization.h
//  Olypsum
//
//  Created by Alexander Meißner on 25.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ObjectManager.h"

#ifndef Localization_h
#define Localization_h

//! A map of strings used for language localization
class Localization {
    public:
    std::map<std::string, std::string> strings; //!< The strings which are mapped for the current language
    std::string selected, //!< The identifier of the current language
                title; //!< The display name of the current language
    /*! Ascertains all available languages
     @param languages A reference to a std::vector which will be used to store the result
     @return Success
     */
    bool getLocalizableLanguages(std::vector<std::string>& languages);
    /*! Loads a localization map from a XML file
     @param filePath The file path
     @return Success
     */
    bool loadLocalization(FilePackage* filePackage);
    //! Convertes a tag into a localized string
    std::string localizeString(const std::string& key);
};

extern Localization localization;

#endif
