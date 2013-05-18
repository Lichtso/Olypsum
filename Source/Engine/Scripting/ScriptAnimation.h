//
//  ScriptAnimation.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//
//

#ifndef ScriptAnimation_h
#define ScriptAnimation_h

#include "ScriptIntersection.h"

class AnimationFrame {
    public:
    AnimationFrame(float acceleration, float duration, v8::Handle<v8::Value> value);
    float acceleration, duration;
    v8::Persistent<v8::Value> value;
};

class AnimationTrack {
    public:
    v8::Persistent<v8::Object> object;
    std::vector<AnimationFrame> frames;
    float time;
    bool looping;
    AnimationTrack(v8::Handle<v8::Object> object);
    bool gameTick(const char* property);
};

class AnimationProperty {
    public:
    std::vector<AnimationTrack*> tracks;
    AnimationTrack* find(v8::Handle<v8::Object> object);
    int find(AnimationTrack* track);
    bool gameTick(const char* property);
};

class ScriptAnimation : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AddFrames(const v8::Arguments& args);
    static v8::Handle<v8::Value> RemoveFrames(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetTrackInfo(const v8::Arguments& args);
    public:
    ScriptAnimation();
};

extern ScriptAnimation scriptAnimation;

#endif