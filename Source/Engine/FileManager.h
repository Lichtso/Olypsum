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

class FilePackage {
    public:
    std::string path, name;
    std::map<std::string, std::weak_ptr<FilePackageResource>> resources;
    FilePackage(std::string name);
    ~FilePackage();
    bool load();
    std::string getUrlOfFile(const char* groupName, const std::string& fileName);
    template <class T> std::shared_ptr<T> getResource(const std::string& name);
};

template <class T> std::shared_ptr<T> FilePackage::getResource(const std::string& fileName) {
    auto iterator = resources.find(fileName);
    if(iterator != resources.end()) {
        std::shared_ptr<FilePackageResource> pointer = iterator->second.lock();
        if(!dynamic_cast<T*>(pointer.get())) {
            log(error_log, std::string("The resource ")+fileName+" in "+name+" is also used by another resource type.");
            return NULL;
        }
        return std::static_pointer_cast<T>(pointer);
    }
    return std::static_pointer_cast<T>((new T())->load(this, fileName));
}

class FileManager {
    std::map<std::string, FilePackage*> filePackages;
    public:
    ~FileManager();
    void clear();
    void loadOptions();
    void saveOptions();
    FilePackage* loadPackage(const char* name);
    FilePackage* getPackage(const char* name);
    void unloadPackage(const char* name);
};

extern FileManager fileManager;

#endif