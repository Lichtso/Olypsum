//
//  FileManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Model.h"
#import "Audio.h"
#import "Localization.h"

#ifndef FileManager_h
#define FileManager_h

//! A bundle of resources in a level package
class FilePackage {
    public:
    std::string path, //!< The absolute path to its directory
                name; //!< The name
    std::map<std::string, std::weak_ptr<FilePackageResource>> resources; //!< All its loaded resources
    FilePackage(std::string name);
    ~FilePackage();
    //! Initialize
    bool init();
    /*! Ascertains the absolute path of a resource
     @param groupName A child directory which contains the resource
     @param fileName The file name of the resource
     @return The absolute path
     */
    std::string getPathOfFile(const char* groupName, const std::string& fileName);
    //! Loads and initializes a FilePackageResource
    template <class T> std::shared_ptr<T> getResource(const std::string& fileName) {
        auto iterator = resources.find(fileName);
        if(iterator != resources.end()) {
            std::shared_ptr<FilePackageResource> pointer = iterator->second.lock();
            if(!dynamic_cast<T*>(pointer.get())) {
                log(error_log, std::string("The resource ")+fileName+" in "+name+" is already used by another resource type.");
                return NULL;
            }
            return std::static_pointer_cast<T>(pointer);
        }
        return std::static_pointer_cast<T>((new T())->load(this, fileName));
    }
};

//! This class manages all FilePackages
class FileManager {
    std::map<std::string, FilePackage*> filePackages; //!< All FilePackages that are loaded
    public:
    ~FileManager();
    //! Deletes all FilePackages
    void clear();
    //! Loads the game options
    void loadOptions();
    //! Saves the game options
    void saveOptions();
    //! Finds a FilePackage and loads it if not already done
    FilePackage* getPackage(const char* name);
    //! Deletes a FilePackage
    void unloadPackage(const char* name);
};

extern FileManager fileManager;

#endif