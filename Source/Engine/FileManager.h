//
//  FileManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Localization.h"

#ifndef FileManager_h
#define FileManager_h

class FilePackage {
    public:
    std::string path, name;
    std::map<std::string, Texture*> textures;
    std::map<std::string, Model*> models;
    std::map<std::string, SoundTrack*> soundTracks;
    FilePackage(std::string name);
    ~FilePackage();
    bool load();
    std::string getUrlOfFile(const char* groupName, const char* fileName);
    bool getTexture(Texture**, const char* fileName);
    bool getModel(Model**, const char* fileName);
    bool getSoundTrack(SoundTrack**, const char* fileName);
    bool releaseTexture(Texture* texture);
    bool releaseModel(Model* model);
    bool releaseSoundTrack(SoundTrack* soundTrack);
};

class FileManager {
    std::map<std::string, FilePackage*> filePackages;
    public:
    FileManager();
    ~FileManager();
    void loadOptions();
    void saveOptions();
    bool loadPackage(const char* name);
    FilePackage* getPackage(const char* name);
    void unloadPackage(const char* name);
    void releaseTexture(Texture* texture);
    void releaseModel(Model* model);
    void releaseSoundTrack(SoundTrack* soundTrack);
};

extern FileManager fileManager;

#endif