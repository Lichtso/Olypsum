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
    std::map<std::string, Model*>::iterator modelIterator;
    for(modelIterator = models.begin(); modelIterator != models.end(); modelIterator ++)
        delete modelIterator->second;
    models.clear();
    
    std::map<std::string, Texture*>::iterator textureIterator;
    for(textureIterator = textures.begin(); textureIterator != textures.end(); textureIterator ++)
        delete textureIterator->second;
    textures.clear();
    /*
    std::map<std::string, SoundTrack*>::iterator soundTrackIterator;
    for(soundTrackIterator = soundTracks.begin(); soundTrackIterator != soundTracks.end(); soundTrackIterator ++)
        delete soundTrackIterator->second;
    soundTracks.clear();*/
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

bool FilePackage::getTexture(Texture** texture, const char* fileName) {
    std::map<std::string, Texture*>::iterator iterator = textures.find(std::string(fileName));
    if(iterator != textures.end()) {
        *texture = iterator->second;
        (*texture)->useCounter ++;
        return false;
    }
    *texture = new Texture();
    std::string url = getUrlOfFile("Textures", fileName);
    if((*texture)->loadImageInRAM(url.c_str())) {
        textures[std::string(fileName)] = *texture;
        return true;
    }
    delete *texture;
    *texture = NULL;
    return false;
}

bool FilePackage::getModel(Model** model, const char* fileName) {
    std::map<std::string, Model*>::iterator iterator = models.find(std::string(fileName));
    if(iterator != models.end()) {
        *model = iterator->second;
        (*model)->useCounter ++;
        return false;
    }
    *model = new Model();
    std::string url = getUrlOfFile("Models", fileName);
    if((*model)->loadCollada(this, url.c_str())) {
        models[std::string(fileName)] = *model;
        return true;
    }
    delete *model;
    *model = NULL;
    return false;
}

bool FilePackage::getSoundTrack(SoundTrack** soundTrack, const char* fileName) {
    std::map<std::string, SoundTrack*>::iterator iterator = soundTracks.find(std::string(fileName));
    if(iterator != soundTracks.end()) {
        *soundTrack = iterator->second;
        (*soundTrack)->useCounter ++;
        return false;
    }
    *soundTrack = new SoundTrack();
    std::string url = getUrlOfFile("Sounds", fileName);
    if((*soundTrack)->loadOgg(url.c_str())) {
        soundTracks[std::string(fileName)] = *soundTrack;
        return true;
    }
    delete *soundTrack;
    *soundTrack = NULL;
    return false;
}

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

bool FilePackage::releaseSoundTrack(SoundTrack* soundTrack) {
    std::map<std::string, SoundTrack*>::iterator iterator;
    for(iterator = soundTracks.begin(); iterator != soundTracks.end(); iterator ++)
        if(iterator->second == soundTrack) {
            iterator->second->useCounter --;
            if(iterator->second->useCounter > 0) return true;
            delete iterator->second;
            soundTracks.erase(iterator);
            return true;
        }
    return false;
}



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

void FileManager::releaseSoundTrack(SoundTrack* soundTrack) {
    std::map<std::string, FilePackage*>::iterator iterator;
    for(iterator = filePackages.begin(); iterator != filePackages.end(); iterator ++)
        if(iterator->second->releaseSoundTrack(soundTrack)) return;
    printf("SoundTrack allready released: %p\n", soundTrack);
}

FileManager fileManager;