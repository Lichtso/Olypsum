//
//  Audio.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include <Vorbis/vorbisfile.h>
#include "LevelManager.h"

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
    
    std::string filePath = filePackageB->getPathOfFile("Sounds", name);
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

bool SoundTrack::isStereo() {
    if(!ALname) return 0.0;
    ALint channels;
    alGetBufferi(ALname, AL_CHANNELS, &channels);
    return (channels == 2);
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



SoundObject::SoundObject(SoundTrack* soundTrackB) :soundTrack(soundTrackB), mode(SoundObject_disposable), velocity(btVector3(0, 0, 0)) {
    objectManager.simpleObjects.insert(this);
    alGenSources(1, &ALname);
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
    play();
}

SoundObject::SoundObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :mode(SoundObject_looping), velocity(btVector3(0, 0, 0)) {
    objectManager.simpleObjects.insert(this);
    alGenSources(1, &ALname);
    BaseObject::init(node, levelLoader);
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("SoundTrack");
    if(!parameterNode) {
        log(error_log, "Tried to construct SoundObject without \"SoundTrack\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("src");
    if(!attribute) {
        log(error_log, "Found \"SoundTrack\"-node without \"src\"-attribute.");
        return;
    }
    setSoundTrack(fileManager.initResource<SoundTrack>(attribute->value()));
    play();
}

SoundObject::~SoundObject() {
    alDeleteSources(1, &ALname);
}

void SoundObject::remove() {
    objectManager.simpleObjects.erase(this);
    BaseObject::remove();
}

void SoundObject::setSoundTrack(std::shared_ptr<SoundTrack> soundTrackB) {
    soundTrack = soundTrackB;
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
}

void SoundObject::play() {
    if(!soundTrack || !soundTrack->ALname) return;
    alSourcei(ALname, AL_LOOPING, mode == SoundObject_looping);
    alSourcePlay(ALname);
}

void SoundObject::pause() {
    alSourcePause(ALname);
}

void SoundObject::stop() {
    alSourceStop(ALname);
}

bool SoundObject::isPlaying() {
    if(!soundTrack) return false;
    ALint state;
    alGetSourcei(ALname, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void SoundObject::setTimeOffset(float timeOffset) {
    alSourcef(ALname, AL_SEC_OFFSET, timeOffset);
}

float SoundObject::getTimeOffset() {
    ALfloat timeOffset;
    alGetSourcef(ALname, AL_SEC_OFFSET, &timeOffset);
    return timeOffset;
}

bool SoundObject::gameTick() {
    if(mode == SoundObject_disposable && !isPlaying()) {
        remove();
        return false;
    }
    btVector3 direction = transformation.getBasis().getColumn(2),
    position = transformation.getOrigin();
    velocity = (position-prevPosition)/profiler.animationFactor;
    if(soundTrack->isStereo())
        alSourcef(ALname, AL_GAIN, optionsState.musicVolume);
    alSource3f(ALname, AL_DIRECTION, direction.x(), direction.y(), direction.z());
    alSource3f(ALname, AL_POSITION, position.x(), position.y(), position.z());
    alSource3f(ALname, AL_VELOCITY, velocity.x(), velocity.y(), velocity.z());
    prevPosition = position;
    return true;
}

rapidxml::xml_node<xmlUsedCharType>* SoundObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("SoundObject");
    node->append_node(fileManager.writeResource(doc, "SoundTrack", soundTrack));
    return node;
}