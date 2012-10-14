//
//  Audio.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <Vorbis/vorbisfile.h>
#import "FileManager.h"

#ifdef LITTLE_ENDIAN
#define ENDIAN 0
#else
#define ENDIAN 1
#endif

SoundTrack::SoundTrack() :ALname(0) {
    
}

SoundTrack::~SoundTrack() {
    if(ALname) alDeleteBuffers(1, &ALname);
}

std::shared_ptr<FilePackageResource> SoundTrack::load(FilePackage* filePackageB, const std::string& name) {
    auto pointer = FilePackageResource::load(filePackageB, name);
    if(ALname) return NULL;
    
    std::string filePath = filePackageB->getUrlOfFile("Sounds", poolIndex->first);
    FILE* fp = fopen(filePath.c_str(), "r");
    if(!fp) {
        log(error_log, std::string("The file ")+filePath+" couldn't be found.");
        return NULL;
    }
    
    OggVorbis_File vf;
    if(ov_open_callbacks(fp, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
        log(error_log, std::string("The file ")+filePath+" is not a valid ogg file.");
        ov_clear(&vf);
        fclose(fp);
        return NULL;
    }
    
    int section = 0, bytesRead, bufferSize = ov_pcm_total(&vf, 0)*2*ov_info(&vf, 0)->channels;
    char *buffer = new char[bufferSize], *bufferPos = buffer;
    do {
        bytesRead = ov_read(&vf, bufferPos, bufferSize, ENDIAN, 2, 1, &section);
        bufferPos += bytesRead;
    } while (bytesRead);
    fclose(fp);
    
    alGenBuffers(1, &ALname);
    alBufferData(ALname, (ov_info(&vf, 0)->channels == 1) ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16, buffer, bufferSize, ov_info(&vf, 0)->rate);
    delete [] buffer;
    ov_clear(&vf);
    return pointer;
}

float SoundTrack::getLength() {
    if(!ALname) return 0.0;
    ALint bits, channels, freq, size;
    alGetBufferi(ALname, AL_BITS, &bits);
    alGetBufferi(ALname, AL_CHANNELS, &channels);
    alGetBufferi(ALname, AL_FREQUENCY, &freq);
    alGetBufferi(ALname, AL_SIZE, &size);
    bits = bits/8*channels*freq;
    return (float)size/bits;
}

SoundSource::SoundSource() {
    soundTrack = NULL;
    looping = false;
    autoDelete = true;
    direction = Vector3(0, 0, 1);
    position = Vector3(0, 0, 0);
    velocity = Vector3(0, 0, 0);
    alGenSources(1, &ALname);
    soundSourcesManager.soundSources.push_back(this);
}

SoundSource::~SoundSource() {
    alDeleteSources(1, &ALname);
    for(int s = 0; s < soundSourcesManager.soundSources.size(); s ++)
        if(soundSourcesManager.soundSources[s] == this) {
            soundSourcesManager.soundSources.erase(soundSourcesManager.soundSources.begin()+s);
            return;
        }
}

void SoundSource::setSoundTrack(std::shared_ptr<SoundTrack> soundTrackB) {
    soundTrack = soundTrackB;
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
}

void SoundSource::play() {
    if(!soundTrack || !soundTrack->ALname) return;
    alSourcei(ALname, AL_LOOPING, looping);
    alSource3f(ALname, AL_DIRECTION, direction.x, direction.y, direction.z);
    alSource3f(ALname, AL_POSITION, position.x, position.y, position.z);
    alSource3f(ALname, AL_VELOCITY, velocity.x, velocity.y, velocity.z);
    alSourcePlay(ALname);
}

void SoundSource::pause() {
    alSourcePause(ALname);
}

void SoundSource::stop() {
    alSourceStop(ALname);
}

bool SoundSource::isPlaying() {
    ALint state;
    alGetSourcei(ALname, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void SoundSource::setTimeOffset(float timeOffset) {
    alSourcef(ALname, AL_SEC_OFFSET, timeOffset);
}

float SoundSource::getTimeOffset() {
    ALfloat timeOffset;
    alGetSourcef(ALname, AL_SEC_OFFSET, &timeOffset);
    return timeOffset;
}

bool SoundSource::calculate() {
    if(!autoDelete || !soundTrack || looping || isPlaying()) return false;
    delete this;
    return true;
}

SoundSourcesManager::SoundSourcesManager() {
    soundDevice = alcOpenDevice(NULL);
    soundContext = alcCreateContext(soundDevice, NULL);
    alcMakeContextCurrent(soundContext);
    log(info_log, std::string("OpenAL, sound output ")+alcGetString(soundDevice, ALC_DEVICE_SPECIFIER));
}

SoundSourcesManager::~SoundSourcesManager() {
    clear();
    alcDestroyContext(soundContext);
    alcCloseDevice(soundDevice);
}

void SoundSourcesManager::clear() {
    for(unsigned int i = 0; i < soundSources.size(); i ++)
        delete soundSources[i];
    soundSources.clear();
}

void SoundSourcesManager::calculate() {
    for(unsigned int s = 0; s < soundSources.size(); s ++)
        if(soundSources[s]->calculate())
            s --;
}

SoundSourcesManager soundSourcesManager;