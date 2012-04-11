//
//  FileManager.h
//  Olypsum
//
//  Created by Alexander Meißner on 07.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Model.h"

#ifndef FileManager_h
#define FileManager_h

class FilePackage {
    public:
    std::string name;
    std::map<std::string, Texture*> textures;
    std::map<std::string, Model*> models;
    //std::map<std::string, SoundTrack*> soundTracks;
    FilePackage(const char* name);
    ~FilePackage();
    bool load();
    std::string getUrlOfFile(const char* groupName, const char* fileName);
    Texture* getTexture(const char* fileName);
    Model* getModel(const char* fileName);
    //SoundTrack* getSoundTrack(const char* fileName);
    bool releaseTexture(Texture* texture);
    bool releaseModel(Model* model);
    //bool releaseSoundTrack(SoundTrack* soundTrack);
};

class FileManager {
    std::map<std::string, FilePackage*> filePackages;
    public:
    FileManager();
    ~FileManager();
    bool loadPackage(const char* name);
    FilePackage* getPackage(const char* name);
    void unloadPackage(const char* name);
    void releaseTexture(Texture* texture);
    void releaseModel(Model* model);
    //void releaseSoundTrack(SoundTrack* soundTrack);
};

extern FileManager fileManager;

#endif