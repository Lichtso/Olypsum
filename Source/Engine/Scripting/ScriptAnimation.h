//
//  ScriptAnimation.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
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
    std::vector<AnimationFrame*> frames;
    float time;
    bool looping;
    AnimationTrack(v8::Handle<v8::Object> object);
    ~AnimationTrack();
    bool update(const char* property);
    bool gameTick(const char* property);
};

class AnimationProperty {
    public:
    std::vector<AnimationTrack*> tracks;
    AnimationTrack* find(v8::Handle<v8::Object> object);
    ~AnimationProperty();
    int find(AnimationTrack* track);
    bool gameTick(const char* property);
};

class AnimationTimer {
    public:
    double timeNext, timeLength;
    v8::Persistent<v8::Function> function;
    AnimationTimer(v8::Handle<v8::Function> function, double timeLength);
    bool gameTick(double timeNow);
};

class ScriptAnimation : public ScriptClass {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AddFrames);
    ScriptDeclareMethod(RemoveFrames);
    ScriptDeclareMethod(GetTrackInfo);
    ScriptDeclareMethod(StartTimer);
    ScriptDeclareMethod(StopTimer);
    public:
    ScriptAnimation();
};

extern std::unique_ptr<ScriptAnimation> scriptAnimation;

#endif