//
//  Audio.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "Scripting/ScriptManager.h"

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

FileResourcePtr<FileResource> SoundTrack::load(FilePackage* _filePackage, const std::string& name) {
    auto pointer = FileResource::load(_filePackage, name);
    if(ALname) return NULL;
    
    std::string filePath = filePackage->getPathOfFile("Sounds/", name);
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



SoundObject::SoundObject() :velocity(btVector3(0, 0, 0)) {
    alGenSources(1, &ALname);
    objectManager.simpleObjects.insert(this);
}

SoundObject::SoundObject(SoundTrack* _soundTrack, Mode _mode) :SoundObject() {
    soundTrack = _soundTrack;
    mode = _mode;
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
    setIsPlaying(true);
}

SoundObject::SoundObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) :SoundObject() {
    SimpleObject::init(node, levelLoader);
    
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
    setSoundTrack(fileManager.getResourceByPath<SoundTrack>(levelLoader->filePackage, attribute->value()));
    
    parameterNode = node->first_node("Mode");
    if(!parameterNode) {
        log(error_log, "Tried to construct SoundObject without \"Mode\"-node.");
        return;
    }
    attribute = parameterNode->first_attribute("value");
    if(!attribute) {
        log(error_log, "Found \"Mode\"-node without \"value\"-attribute.");
        return;
    }
    if(strcmp(attribute->value(), "looping") == 0)
        mode = Looping;
    else if(strcmp(attribute->value(), "hold") == 0)
        mode = Hold;
    else if(strcmp(attribute->value(), "dispose") == 0)
        mode = Dispose;
    else{
        log(error_log, "Found \"Mode\"-node with invalid \"value\"-attribute.");
        return;
    }
    
    parameterNode = node->first_node("Volume");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("value");
        if(!attribute) {
            log(error_log, "Found \"Volume\"-node without \"value\"-attribute.");
            return;
        }
        float volume;
        sscanf(attribute->value(), "%f", &volume);
        setVolume(volume);
    }
    
    if(node->first_node("IsPlaying"))
        setIsPlaying(true);
    
    parameterNode = node->first_node("TimeOffset");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("value");
        if(!attribute) {
            log(error_log, "Found \"TimeOffset\"-node without \"value\"-attribute.");
            return;
        }
        float time;
        sscanf(attribute->value(), "%f", &time);
        setTimeOffset(time);
    }
    
    ScriptInstance(ScriptSoundObject);
}

SoundObject::~SoundObject() {
    alDeleteSources(1, &ALname);
}

bool SoundObject::gameTick() {
    if(mode == Dispose && !getIsPlaying()) {
        removeClean();
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
    return SimpleObject::gameTick();
}

rapidxml::xml_node<xmlUsedCharType>* SoundObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = SimpleObject::write(doc, levelSaver);
    
    node->name("SoundObject");
    node->append_node(fileManager.writeResource(doc, "SoundTrack", soundTrack));
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Mode");
    node->append_node(parameterNode);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("value");
    parameterNode->append_attribute(attribute);
    switch(mode) {
        case Looping:
            attribute->value("looping");
            break;
        case Hold:
            attribute->value("hold");
            break;
        case Dispose:
            attribute->value("dispose");
            break;
    }
    
    if(!soundTrack->isStereo()) {
        float volume = getVolume();
        if(volume != 1.0) {
            parameterNode = doc.allocate_node(rapidxml::node_element);
            parameterNode->name("Volume");
            node->append_node(parameterNode);
            attribute = doc.allocate_attribute();
            attribute->name("value");
            attribute->value(doc.allocate_string(stringOf(volume).c_str()));
            parameterNode->append_attribute(attribute);
        }
    }
    
    if(getIsPlaying()) {
        parameterNode = doc.allocate_node(rapidxml::node_element);
        parameterNode->name("IsPlaying");
        node->append_node(parameterNode);
    }
    
    parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("TimeOffset");
    node->append_node(parameterNode);
    attribute = doc.allocate_attribute();
    attribute->name("value");
    attribute->value(doc.allocate_string(stringOf(getTimeOffset()).c_str()));
    parameterNode->append_attribute(attribute);
    
    return node;
}

void SoundObject::setSoundTrack(FileResourcePtr<SoundTrack> soundTrackB) {
    soundTrack = soundTrackB;
    alSourcei(ALname, AL_BUFFER, soundTrack->ALname);
}

void SoundObject::setIsPlaying(bool playing) {
    if(!soundTrack || !soundTrack->ALname) return;
    alSourcei(ALname, AL_LOOPING, mode == Looping);
    if(playing)
        alSourcePlay(ALname);
    else
        alSourcePause(ALname);
}

bool SoundObject::getIsPlaying() {
    if(!soundTrack) return false;
    ALint state;
    alGetSourcei(ALname, AL_SOURCE_STATE, &state);
    return (state == AL_PLAYING);
}

void SoundObject::setTimeOffset(float timeOffset) {
    alSourcef(ALname, AL_SEC_OFFSET, timeOffset);
}

float SoundObject::getTimeOffset() {
    if(!soundTrack) return -1.0;
    ALfloat timeOffset;
    alGetSourcef(ALname, AL_SEC_OFFSET, &timeOffset);
    return timeOffset;
}

void SoundObject::setVolume(float volume) {
    alSourcef(ALname, AL_GAIN, volume);
}

float SoundObject::getVolume() {
    ALfloat volume;
    alGetSourcef(ALname, AL_GAIN, &volume);
    return volume;
}