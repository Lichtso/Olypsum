//
//  FileManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FileManager.h"

FilePackage::FilePackage(const char* nameB) {
    if(nameB) name = nameB;
}

FilePackage::~FilePackage() {
    std::map<std::string, Texture*>::iterator textureIterator;
    for(textureIterator = textures.begin(); textureIterator != textures.end(); textureIterator ++)
        delete textureIterator->second;
    
    std::map<std::string, Model*>::iterator modelIterator;
    for(modelIterator = models.begin(); modelIterator != models.end(); modelIterator ++)
        delete modelIterator->second;
    /*
    std::map<std::string, SoundTrack*>::iterator soundTrackIterator;
    for(soundTrackIterator = soundTracks.begin(); soundTrackIterator != soundTracks.end(); soundTrackIterator ++)
        delete soundTrackIterator->second;*/
}

bool FilePackage::load() {
    if(name.size() == 0) {
        localization.loadLocalization("Languages/English.xml");
        return true;
    }
    
    std::string auxUrl, url = gameDataDir + "Packages/";
    
    //TODO: Load package info
    auxUrl = url + "Languages/English.xml";
    localization.loadLocalization(auxUrl.c_str());
    
    return true;
}

std::string FilePackage::getUrlOfFile(const char* groupName, const char* fileName) {
    std::string url;
    if(name.size() > 0) url = gameDataDir + "Packages/";
    url += std::string(groupName)+'/'+std::string(fileName);
    return url;
}

Texture* FilePackage::getTexture(const char* fileName) {
    std::map<std::string, Texture*>::iterator iterator = textures.find(std::string(fileName));
    if(iterator != textures.end()) {
        iterator->second->useCounter ++;
        return iterator->second;
    }
    Texture* texture = new Texture();
    std::string url = getUrlOfFile("Textures", fileName);
    if(texture->loadImageInRAM(url.c_str()))
        return texture;
    delete texture;
    return NULL;
}

Model* FilePackage::getModel(const char* fileName) {
    std::map<std::string, Model*>::iterator iterator = models.find(std::string(fileName));
    if(iterator != models.end()) {
        iterator->second->useCounter ++;
        return iterator->second;
    }
    Model* model = new Model();
    std::string url = getUrlOfFile("Models", fileName);
    if(model->loadCollada(url.c_str()))
        return model;
    delete model;
    return NULL;
}

//SoundTrack* FilePackage::getSoundTrack(const char* fileName);

bool FilePackage::releaseTexture(Texture* texture) {
    std::map<std::string, Texture*>::iterator iterator;
    for(iterator = textures.begin(); iterator != textures.end(); iterator ++)
        if(iterator->second == texture) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            textures.erase(iterator);
            return true;
        }
    return false;
}

bool FilePackage::releaseModel(Model* model) {
    std::map<std::string, Model*>::iterator iterator;
    for(iterator = models.begin(); iterator != models.end(); iterator ++)
        if(iterator->second == model) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            models.erase(iterator);
            return true;
        }
    return false;
}

//bool FilePackage::releaseSoundTrack(SoundTrack* soundTrack);



FileManager::FileManager() {
    
}

FileManager::~FileManager() {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        delete iterator->second;
}

bool FileManager::loadPackage(const char* nameStr) {
    FilePackage* package = new FilePackage(nameStr);
    if(!package->load()) {
        delete package;
        return false;
    }
    std::string name;
    if(nameStr) name = nameStr;
    filePackages[name] = package;
    return true;
}

FilePackage* FileManager::getPackage(const char* nameStr) {
    std::string name;
    if(nameStr) name = nameStr;
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) {
        FilePackage* package = new FilePackage(nameStr);
        if(!package->load()) {
            delete package;
            return NULL;
        }
        filePackages[name] = package;
        return package;
    }
    return iterator->second;
}

void FileManager::unloadPackage(const char* nameStr) {
    std::string name;
    if(nameStr) name = nameStr;
    std::map<std::string, FilePackage*>::iterator iterator = filePackages.find(name);
    if(iterator == filePackages.end()) return;
    delete iterator->second;
    filePackages.erase(iterator);
}

void FileManager::releaseTexture(Texture* texture) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseTexture(texture)) return;
    printf("Texture allready released: %p\n", texture);
}

void FileManager::releaseModel(Model* model) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseModel(model)) return;
    printf("Model allready released: %p\n", model);
}

//void FileManager::releaseSoundTrack(SoundTrack* soundTrack);

FileManager fileManager;