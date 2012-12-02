//
//  Audio.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <Vorbis/vorbisfile.h>
#import "ObjectManager.h"
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
    
    std::string filePath = filePackageB->getUrlOfFile("Sounds", name);
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




SoundSourceObject::SoundSourceObject() :soundTrack(NULL), mode(SoundSource_disposable), velocity(btVector3(0, 0, 0)) {
    alGenSources(1, &ALname);
    objectManager.soundSourceObjects.insert(this);
}

SoundSourceObject::~SoundSourceObject() {
    alDeleteSources(1, &ALname);
}

void SoundSourceObject::remove() {
    objectManager.soundSourceObjects.erase(this);
    delete this;
}

void SoundSourceObject::setSoundTrack(std::shared_ptr<SoundTrack> soundTrackB) {
    soundTrack = soundTrackB;
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
}

void SoundSourceObject::play() {
    if(!soundTrack || !soundTrack->ALname) return;
    alSourcei(ALname, AL_LOOPING, mode == SoundSource_looping);
    alSourcePlay(ALname);
}

void SoundSourceObject::pause() {
    alSourcePause(ALname);
}

void SoundSourceObject::stop() {
    alSourceStop(ALname);
}

bool SoundSourceObject::isPlaying() {
    if(!soundTrack) return false;
    ALint state;
    alGetSourcei(ALname, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void SoundSourceObject::setTimeOffset(float timeOffset) {
    alSourcef(ALname, AL_SEC_OFFSET, timeOffset);
}

float SoundSourceObject::getTimeOffset() {
    ALfloat timeOffset;
    alGetSourcef(ALname, AL_SEC_OFFSET, &timeOffset);
    return timeOffset;
}

void SoundSourceObject::gameTick() {
    btVector3 direction = transformation.getBasis().getColumn(2),
    position = transformation.getOrigin();
    velocity = position-prevPosition;
    alSource3f(ALname, AL_DIRECTION, direction.x(), direction.y(), direction.z());
    alSource3f(ALname, AL_POSITION, position.x(), position.y(), position.z());
    alSource3f(ALname, AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
    prevPosition = position;
}