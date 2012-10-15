//
//  Audio.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import "Vector3.h"

#ifndef Audio_h
#define Audio_h

class SoundTrack : public FilePackageResource {
    public:
    ALuint ALname;
    SoundTrack();
    ~SoundTrack();
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackageB, const std::string& name);
    float getLength();
};

class SoundSource {
    public:
    ALuint ALname;
    std::shared_ptr<SoundTrack> soundTrack;
    bool looping, autoDelete;
    Vector3 direction, position, velocity;
    SoundSource();
    ~SoundSource();
    void setSoundTrack(std::shared_ptr<SoundTrack> soundTrack);
    void play();
    void pause();
    void stop();
    bool isPlaying();
    void setTimeOffset(float timeOffset);
    float getTimeOffset();
    bool gameTick();
};

class SoundSourcesManager {
    ALCdevice* soundDevice;
    ALCcontext* soundContext;
    public:
    std::vector<SoundSource*> soundSources;
    SoundSourcesManager();
    ~SoundSourcesManager();
    void clear();
    void gameTick();
};

extern SoundSourcesManager soundSourcesManager;

#endif