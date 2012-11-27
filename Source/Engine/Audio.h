//
//  Audio.h
//  Olypsum
//
//  Created by Alexander Meißner on 28.05.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import <OpenAL/al.h>
#import <OpenAL/alc.h>
#import "Object.h"
#import "Texture.h"

#ifndef Audio_h
#define Audio_h

//! A SoundTrack used for audio playback
/*!
 A FilePackageResource which can be loaded form a OGG-file and can be played by a SoundSourceObject.
 */
class SoundTrack : public FilePackageResource {
    public:
    ALuint ALname; //!< The OpenAL identifier of this SoundTrack
    SoundTrack();
    ~SoundTrack();
    /*! Used by the engine to load a OGG-file
     @param filePackage The parent FilePackage
     @param name The file name
     @see FilePackage::getResource()
     */
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackage, const std::string& name);
    /*!
     @return The length of the OGG-file in seconds
     */
    float getLength();
};

//! A SimpleObject used for audio playback
/*!
 A SimpleObject which can be loaded form a OGG-file and can be played by a SoundSourceObject.
 */
class SoundSourceObject : public SimpleObject {
    btVector3 prevPosition, velocity; //!< Used by the engine to calculate the Doppler effect
    public:
    ALuint ALname; //!< The OpenAL identifier of this SoundSourceObject
    std::shared_ptr<SoundTrack> soundTrack; //!< The SoundTrack used for audio playback
    //! The playback state
    enum {
        SoundSource_looping, //!< Restart playing if the SoundTrack reaches its end
        SoundSource_hold, //!< Stops playing if the SoundTrack reaches its end
        SoundSource_disposable //!< Deletes this SoundSourceObject if the SoundTrack reaches its end
    } mode;
    SoundSourceObject();
    ~SoundSourceObject();
    void remove();
    /*! Adds a SoundTrack to this SoundSourceObject
     @param soundTrack The new SoundTrack
     */
    void setSoundTrack(std::shared_ptr<SoundTrack> soundTrack);
    //! Starts to play the SoundTrack or resumes it
    void play();
    //! Stops playing the SoundTrack but keeps the current playback position
    void pause();
    //! Stops playing the SoundTrack and resets the current playback position
    void stop();
    /*!
     @return Is this SoundSourceObject currently playing
     */
    bool isPlaying();
    /*! Sets the playback position
     @param timeOffset The new playback position in seconds
     */
    void setTimeOffset(float timeOffset);
    /*!
     @return The current playback position in seconds
     */
    float getTimeOffset();
    void gameTick();
};

#endif