//
//  ScriptAnimation.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef ScriptAnimation_h
#define ScriptAnimation_h

#include "ScriptLinearAlgebra.h"

/*class AnimationFrame {
    public:
    AnimationFrame(float acceleration, float duration, JSValueRef value);
    float acceleration, duration;
    JSValueRef value;
};

class AnimationTrack {
    public:
    JSObjectRef object;
    std::vector<AnimationFrame*> frames;
    float time;
    bool looping;
    AnimationTrack(JSObjectRef object);
    ~AnimationTrack();
    bool update(const std::string& property);
    bool gameTick(const std::string& property);
};

class AnimationProperty {
    public:
    std::vector<AnimationTrack*> tracks;
    AnimationTrack* find(JSObjectRef object);
    ~AnimationProperty();
    int find(AnimationTrack* track);
    bool gameTick(const std::string& property);
};*/

class AnimationTimer {
    public:
    double timeNext, timeLength;
    JSObjectRef function;
    AnimationTimer(JSObjectRef function, double timeLength);
    ~AnimationTimer();
    bool gameTick(double timeNow);
};

#endif