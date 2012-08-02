//
//  Audio.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Cam.h"

#ifndef Audio_h
#define Audio_h
#import <Vorbis/vorbisfile.h>

class FilePackage;

class SoundTrack {
    public:
    unsigned int useCounter;
    ALuint ALname;
    SoundTrack();
    ~SoundTrack();
    bool loadOgg(const char* filePath);
    float getLength();
};

class SoundSource {
    public:
    ALuint ALname;
    SoundTrack* soundTrack;
    bool looping, autoDelete;
    Vector3 direction, position, velocity;
    SoundSource();
    ~SoundSource();
    void setSoundTrack(SoundTrack* soundTrack);
    void play();
    void pause();
    void stop();
    bool isPlaying();
    void setTimeOffset(float timeOffset);
    float getTimeOffset();
    bool calculate();
};

class SoundSourcesManager {
    ALCdevice* soundDevice;
    ALCcontext* soundContext;
    public:
    std::vector<SoundSource*> soundSources;
    SoundSourcesManager();
    ~SoundSourcesManager();
    void calculate();
};

extern SoundSourcesManager soundSourcesManager;

#endif