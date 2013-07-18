//
//  FileManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Matrix4.h"

#ifndef FileManager_h
#define FileManager_h

class FilePackage;
class FileResource;

//! A pointer referencing a ARC FileResource
template<class T> class FileResourcePtr {
    T* resource; //!< The referenced FileResource
    inline void decrementRetain() {
        if(resource) {
            resource->retainCount --;
            if(resource->retainCount == 0 && resource->filePackage->name != "Default")
                resource->remove();
        }
    }
    inline void incrementRetain() {
        if(resource)
            resource->retainCount ++;
    }
    public:
    FileResourcePtr() :resource(NULL) { }
    FileResourcePtr(T* resource_) :resource(resource_) {
        incrementRetain();
    }
    template<class U> FileResourcePtr(U* resource_) :FileResourcePtr(static_cast<T*>(resource_)) { }
    template<class U> FileResourcePtr(const FileResourcePtr<U>& ptr) :FileResourcePtr(*ptr) { }
    FileResourcePtr(const FileResourcePtr<T>& ptr) :FileResourcePtr(*ptr) { }
    FileResourcePtr(FileResourcePtr<T>&& ptr) :FileResourcePtr(*ptr) { }
    ~FileResourcePtr() {
        decrementRetain();
    }
    T* operator*() const {
        return resource;
    }
    T* operator->() const {
        return resource;
    }
    operator bool() const {
        return resource != NULL;
    }
    FileResourcePtr<T>& operator=(const FileResourcePtr<T>& ptr) {
        decrementRetain();
        resource = *ptr;
        incrementRetain();
        return *this;
    }
};

//! A resource represented by a file in a FilePackage
class FileResource {
    public:
    FilePackage* filePackage; //!< The parent FilePackage
    std::string name; //!< The name of the file
    unsigned int retainCount; //!< How many objects use this FileResource
    FileResource() :filePackage(NULL), retainCount(0) { }
    virtual ~FileResource() { };
    //! Removes this FileResource correctly from its FilePackage
    void remove();
    /*! Loads the file
     @param filePackage The FilePackage as parent
     @param name The file name to be loaded
     */
    virtual FileResourcePtr<FileResource> load(FilePackage* filePackage, const std::string& name);
};

//! A bundle of resources in a level package
class FilePackage {
    public:
    std::string path, //!< The absolute path to its directory
                name; //!< The name
    std::map<std::string, FileResource*> resources; //!< All its loaded resources
    FilePackage(std::string name_) :name(name_) { };
    ~FilePackage();
    //! Initialize, returns Success
    bool init();
    /*! Ascertains the absolute path of a resource
     @param groupName A child directory which contains the resource
     @param fileName The file name of the resource
     @return The absolute path
     */
    std::string getPathOfFile(const char* groupName, const std::string& fileName);
    //! Loads and initializes a FilePackageResource
    template <class T> FileResourcePtr<T> getResource(const std::string& fileName) {
        auto iterator = resources.find(fileName);
        if(iterator != resources.end()) {
            if(!dynamic_cast<T*>(iterator->second)) {
                log(error_log, std::string("The resource ")+fileName+" in "+name+" is already used by another resource type.");
                return FileResourcePtr<T>();
            }
            return iterator->second;
        }
        return (new T())->load(this, fileName);
    }
    //! Finds the name of a already loaded resource
    template <class T> std::string getNameOfResource(const FileResourcePtr<T>& resource) {
        for(auto iterator : resources)
            if(iterator.second == *resource)
                return iterator.first;
        return "";
    }
};

//! This class manages all FilePackages
class FileManager {
    public:
    std::map<std::string, FilePackage*> filePackages; //!< All FilePackages that are loaded
    //! Deletes all FilePackages
    void clear();
    /*! Loads a FilePackage if not already done
     @param name The name of the FilePackage
     @return The desired FilePackage or NULL on failure
     */
    FilePackage* loadPackage(const std::string& name);
    //! Deletes a FilePackage by name
    void unloadPackage(const std::string& name);
    //! Reads a resources path
    bool readResourcePath(const std::string& path, FilePackage*& filePackage, std::string& name);
    //! Returns a resources path
    std::string getPathOfResource(FilePackage* filePackage, std::string name);
    //Writes a resource to rapidxml::xml_node and returns it
    rapidxml::xml_node<xmlUsedCharType>* writeResource(rapidxml::xml_document<xmlUsedCharType>& doc, const char* nodeName,
                                                       FilePackage* filePackage, const std::string& name);
    //! Gets a resource from a given path
    template <class T> FileResourcePtr<T> getResourceByPath(const std::string path) {
        FilePackage* filePackage;
        std::string name;
        if(!readResourcePath(path, filePackage, name)) {
            log(error_log, "Couldn't initialize resource.");
            return NULL;
        }
        return filePackage->getResource<T>(name);
    };
    //Finds a resource by searching through all FilePackages
    template <class T> FilePackage* findResource(const FileResourcePtr<T>& resource, std::string& name) {
        for(auto iterator : filePackages) {
            name = iterator.second->getNameOfResource(resource);
            if(name.size() == 0) continue;
            return iterator.second;
        }
        return NULL;
    }
    //Writes a resource to rapidxml::xml_node and returns it
    template <class T> rapidxml::xml_node<xmlUsedCharType>* writeResource(rapidxml::xml_document<xmlUsedCharType>& doc,
                                                                          const char* nodeName, const FileResourcePtr<T>& resource) {
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
          mouseSensitivity = 1.0,
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