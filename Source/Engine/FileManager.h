//
//  FileManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Model.h"
#include "Audio.h"
#include "Localization.h"

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
    //! Finds the name of a already loaded resource
    template <class T> std::string getNameOfResource(std::shared_ptr<T>& resource) {
        for(auto iterator : resources) {
            std::shared_ptr<FilePackageResource> ptr = iterator.second.lock();
            if(ptr == resource)
                return iterator.first;
        }
        return "";
    }
};

//! This class manages all FilePackages
class FileManager {
    public:
    std::map<std::string, FilePackage*> filePackages; //!< All FilePackages that are loaded
    ~FileManager();
    //! Deletes all FilePackages
    void clear();
    //! Finds a FilePackage and loads it if not already done
    FilePackage* getPackage(const std::string& name);
    //! Deletes a FilePackage
    void unloadPackage(const std::string& name);
    //! Reads a resources path
    bool readResource(const std::string& path, FilePackage*& filePackage, std::string& name);
    //! Reads a resources path
    std::string getResourcePath(FilePackage* filePackage, std::string name);
    //Writes a resource to rapidxml::xml_node and returns it
    rapidxml::xml_node<xmlUsedCharType>* writeResource(rapidxml::xml_document<xmlUsedCharType>& doc, const char* nodeName,
                                                       FilePackage* filePackage, const std::string& name);
    //! Initialize a resource from path
    template <class T> std::shared_ptr<T> initResource(const std::string path) {
        FilePackage* filePackage;
        std::string name;
        if(!readResource(path, filePackage, name)) {
            log(error_log, "Couldn't initialize resource.");
            return NULL;
        }
        return filePackage->getResource<T>(name);
    };
    //Finds a resource by searching through all FilePackages
    template <class T> FilePackage* findResource(std::shared_ptr<T>& resource, std::string& name) {
        for(auto iterator : filePackages) {
            name = iterator.second->getNameOfResource(resource);
            if(name.size() == 0) continue;
            return iterator.second;
        }
        return NULL;
    }
    //Writes a resource to rapidxml::xml_node and returns it
    template <class T> rapidxml::xml_node<xmlUsedCharType>* writeResource(rapidxml::xml_document<xmlUsedCharType>& doc,
                                                                          const char* nodeName, std::shared_ptr<T>& resource) {
        std::string name;
        FilePackage* filePackage = findResource<T>(resource, name);
        return writeResource(doc, nodeName, filePackage, name);
    }
};

//! This class manages the options
class OptionsState {
    public:
    float anisotropy = 1.0,
          screenBlurFactor = -1.0,
          globalVolume = 0.5,
          musicVolume = 0.5,
          mouseSensitivity = 0.005,
          mouseSmoothing = 0.5;
    bool edgeSmoothEnabled = false,
         cubemapsEnabled = false,
         vSyncEnabled = true;
    unsigned int videoWidth = 65535,
                 videoHeight = 65535,
                 videoScale = 1;
    unsigned char depthOfFieldQuality = 0,
                  surfaceQuality = 1,
                  shadowQuality = 1,
                  ssaoQuality = 0,
                  blendingQuality = 2,
                  particleCalcTarget = 2;
    //! Loads the game options
    void loadOptions();
    //! Saves the game options
    void saveOptions();
};

extern FileManager fileManager;
extern OptionsState optionsState, prevOptionsState;

#endif