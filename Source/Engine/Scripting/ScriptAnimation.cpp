//
//  ScriptAnimation.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static v8::Handle<v8::Value> accessProperty(v8::Handle<v8::Object> object, const char* property, v8::Handle<v8::Value> setValue) {
    v8::Handle<v8::Value> returnValue, key = ScriptString(property);
    std::smatch match;
    const std::regex regex("(.+)\\[([0-9]+)\\]");
    if(std::regex_match(std::string(property), match, regex)) {
        returnValue = object->Get(*match[0].str().c_str());
        unsigned int index = 0;
        sscanf(match[1].str().c_str(), "%d", &index);
        if(returnValue->IsArray()) {
            v8::Handle<v8::Array> array = v8::Handle<v8::Array>::Cast(returnValue);
            returnValue = array->Get(index);
            if(!setValue.IsEmpty())
                array->Set(index, setValue);
        }else if(returnValue->IsFunction()) {
            v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(returnValue);
            v8::Handle<v8::Value> arguments[] = { v8::Uint32::New(v8::Isolate::GetCurrent(), index), setValue };
            returnValue = function->CallAsFunction(object, (setValue.IsEmpty())?1:2, arguments);
        }
    }else{
        returnValue = object->Get(key);
        if(returnValue->IsFunction()) {
            v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(returnValue);
            returnValue = function->CallAsFunction(object, (setValue.IsEmpty())?0:1, &setValue);
        }else if(!setValue.IsEmpty())
            object->Set(key, setValue);
    }
    return returnValue;
}

AnimationFrame::AnimationFrame(float _acceleration, float _duration, v8::Handle<v8::Value> _value)
    :acceleration(_acceleration), duration(_duration) {
    value.Reset(v8::Isolate::GetCurrent(), _value);
}

AnimationTrack::AnimationTrack(v8::Handle<v8::Object> _object)
    :time(0.0), looping(false) {
    object.Reset(v8::Isolate::GetCurrent(), _object);
}

AnimationTrack::~AnimationTrack() {
    for(auto iterator : frames)
        delete iterator;
}

bool AnimationTrack::update(const char* property) {
    ScriptScope();
    if(frames.size() <= 1 || frames[0]->value.IsEmpty() || frames[1]->value.IsEmpty())
        return false;
    
    float t = min(time / frames[1]->duration, 1.0F), t2 = t*t, t3 = t2*t;
    t = 3.0*t2-2.0*t3+(t3-2.0*t2+t)*frames[0]->acceleration+(t3-t2)*frames[1]->acceleration;
    
    v8::Handle<v8::Value> value;
    if((*frames[0]->value)->IsNumber()) {
        double a = (*frames[0]->value)->NumberValue(), b = (*frames[1]->value)->NumberValue();
        value = v8::Number::New(v8::Isolate::GetCurrent(), a+(b-a)*t);
    }else if((*frames[0]->value)->IsInt32()) {
        int a = (*frames[0]->value)->IntegerValue(), b = (*frames[1]->value)->IntegerValue();
        value = v8::Integer::New(v8::Isolate::GetCurrent(), a+(b-a)*t);
    }else if((*frames[0]->value)->IsBoolean()) {
        bool a = (*frames[0]->value)->BooleanValue(), b = (*frames[1]->value)->BooleanValue();
        value = v8::Boolean::New(v8::Isolate::GetCurrent(), (t < 0.5) ? a : b);
    }else if(scriptVector3->isCorrectInstance(*frames[0]->value)) {
        btVector3 a = scriptVector3->getDataOfInstance(*frames[0]->value),
                  b = scriptVector3->getDataOfInstance(*frames[1]->value);
        value = scriptVector3->newInstance(a.lerp(b, t));
    }else if(scriptQuaternion->isCorrectInstance(*frames[0]->value)) {
        btQuaternion a = scriptQuaternion->getDataOfInstance(*frames[0]->value),
                     b = scriptQuaternion->getDataOfInstance(*frames[1]->value);
        value = scriptQuaternion->newInstance(a.slerp(b, t));
    }else if(scriptMatrix4->isCorrectInstance(*frames[0]->value)) {
        Matrix4 *a = scriptMatrix4->getDataOfInstance(*frames[0]->value),
                *b = scriptMatrix4->getDataOfInstance(*frames[1]->value);
        value = scriptMatrix4->newInstance(a->getInterpolation(*b, t));
    }else
        return false;
    
    return !accessProperty(v8::Handle<v8::Object>(*object), property, value).IsEmpty();
}

bool AnimationTrack::gameTick(const char* property) {
    time += profiler.animationFactor;
    if(!update(property)) return false;
    
    while(time >= frames[1]->duration && frames.size() > 1) {
        time -= frames[1]->duration;
        AnimationFrame* frame = frames[0];
        if(looping) {
            frames.erase(frames.begin());
            frames.push_back(frame);
        }else{
            delete frame;
            frames.erase(frames.begin());
        }
    }
    
    return (frames.size() <= 1);
}



AnimationProperty::~AnimationProperty() {
    for(auto iterator : tracks)
        delete iterator;
}

AnimationTrack* AnimationProperty::find(v8::Handle<v8::Object> object) {
    for(unsigned int i = 0; i < tracks.size(); i ++)
        if(tracks[i]->object == object)
            return tracks[i];
    return NULL;
}

int AnimationProperty::find(AnimationTrack* track) {
    for(unsigned int i = 0; i < tracks.size(); i ++)
        if(tracks[i] == track)
            return i;
    return -1;
}

bool AnimationProperty::gameTick(const char* property) {
    for(unsigned int i = 0; i < tracks.size(); i ++)
        if(tracks[i]->gameTick(property)) {
            delete tracks[i];
            tracks.erase(tracks.begin()+i);
            i --;
        }
    return (tracks.size() == 0);
}



AnimationTimer::AnimationTimer(v8::Handle<v8::Function> _function, double _timeLength)
    :timeLength(_timeLength), timeNext(_timeLength+getTime()) {
    function.Reset(v8::Isolate::GetCurrent(), _function);
}

bool AnimationTimer::gameTick(double timeNow) {
    ScriptScope();
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = (*function)->CallAsFunction(v8::Handle<v8::Value>(*function), 0, NULL);
    scriptManager->tryCatch(&tryCatch);
    
    if(scriptManager->timers.find(this) == scriptManager->timers.end())
        return false;
    
    if(timeLength > 0.0 && result->IsBoolean() && result->BooleanValue()) {
        if(timeNow-timeNext > timeLength)
            timeNext = timeNow+timeLength;
        else
            timeNext += timeLength;
        return false;
    }
    
    return true;
}



void ScriptAnimation::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("Animation Constructor: Class can't be instantiated");
}

void ScriptAnimation::AddFrames(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 6)
        return ScriptException("Animation addFrames(): Too few arguments");
    if(!args[0]->IsObject() || !args[1]->IsString()|| !args[2]->IsBoolean() || !args[3]->IsArray() || !args[4]->IsArray() || !args[5]->IsArray())
        return ScriptException("Animation addFrames(): Invalid argument");
    
    std::string property = stdStrOfV8(args[1]);
    AnimationProperty* animationProperty;
    auto iterator = scriptManager->animations.find(property);
    if(iterator == scriptManager->animations.end()) {
        animationProperty = new AnimationProperty();
        scriptManager->animations[property] = animationProperty;
    }else
        animationProperty = iterator->second;
    
    AnimationTrack* track = animationProperty->find(args[0]->ToObject());
    if(!track) {
        track = new AnimationTrack(args[0]->ToObject());
        animationProperty->tracks.push_back(track);
    }
    track->looping = args[2]->BooleanValue();
    
    v8::Handle<v8::Array> accelerations = v8::Handle<v8::Array>::Cast(args[3]),
                          durations = v8::Handle<v8::Array>::Cast(args[4]),
                          values = v8::Handle<v8::Array>::Cast(args[5]);
    
    if(accelerations->Length() != durations->Length() || accelerations->Length() != values->Length())
        return ScriptException("Animation addFrames(): Invalid frames");
    
    if(accelerations->Length() == 0)
        return;
    
    v8::Handle<v8::Value> defaultValue = accessProperty(args[0]->ToObject(), cStrOfV8(args[1]), v8::Undefined(v8::Isolate::GetCurrent()));
    if(defaultValue.IsEmpty())
        return ScriptException("Animation addFrames(): Invalid property");
    
    for(unsigned int i = 0; i < accelerations->Length(); i ++) {
        if(!accelerations->Get(i)->IsNumber() || !durations->Get(i)->IsNumber())
            return ScriptException("Animation addFrames(): Invalid frame");
        
        track->frames.push_back(new AnimationFrame(accelerations->Get(i)->NumberValue(), durations->Get(i)->NumberValue(),
                                                  (values->Get(i)->IsNull()) ? defaultValue : values->Get(i)));
    }
    track->update(property.c_str());
}

void ScriptAnimation::RemoveFrames(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 4)
        return ScriptException("Animation removeFrames(): Too few arguments");
    if(!args[0]->IsObject() || !args[1]->IsString() ||
       !args[2]->IsUint32() || !args[3]->IsInt32() || args[3]->IntegerValue() <= 0)
        return ScriptException("Animation removeFrames(): Invalid argument");
    
    std::string property = stdStrOfV8(args[1]);
    auto iterator = scriptManager->animations.find(property);
    if(iterator == scriptManager->animations.end())
        return ScriptException("Animation removeFrames(): Invalid track");
    
    AnimationProperty* animationProperty = iterator->second;
    AnimationTrack* track = animationProperty->find(args[0]->ToObject());
    if(!track)
        return ScriptException("Animation removeFrames(): Invalid track");
    
    if(track->frames.size() <= args[2]->IntegerValue()+args[3]->IntegerValue())
        return ScriptException("Animation removeFrames(): Out of bounds");
    
    if(args[3]->IntegerValue() >= track->frames.size()-2) {
        delete track;
        animationProperty->tracks.erase(animationProperty->tracks.begin()+animationProperty->find(track));
        if(animationProperty->tracks.size() == 0) {
            delete animationProperty;
            scriptManager->animations.erase(scriptManager->animations.find(property));
        }
    }else{
        if(args[2]->IntegerValue() < 2) track->time = 0.0;
        auto from = args[2]->IntegerValue(), to = args[2]->IntegerValue()+args[3]->IntegerValue();
        for(int i = from; i < to; i ++)
            delete track->frames[i];
        track->frames.erase(track->frames.begin()+from, track->frames.begin()+to);
    }
}

void ScriptAnimation::GetTrackInfo(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2)
        return ScriptException("Animation getTrackInfo(): Too few arguments");
    if(!args[0]->IsObject() || !args[1]->IsString())
        return ScriptException("Animation getTrackInfo(): Invalid argument");
    
    std::string property = stdStrOfV8(args[1]);
    auto iterator = scriptManager->animations.find(property);
    if(iterator == scriptManager->animations.end())
        return;
    
    AnimationProperty* animationProperty = iterator->second;
    AnimationTrack* track = animationProperty->find(args[0]->ToObject());
    if(!track)
        return;
    
    v8::Handle<v8::Object> result = v8::Object::New(v8::Isolate::GetCurrent());
    result->Set(ScriptString("frames"), v8::Integer::New(v8::Isolate::GetCurrent(), track->frames.size()));
    result->Set(ScriptString("looping"), v8::Boolean::New(v8::Isolate::GetCurrent(), track->looping));
    result->Set(ScriptString("time"), v8::Number::New(v8::Isolate::GetCurrent(), track->time));
    ScriptReturn(result);
}

void ScriptAnimation::StartTimer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2)
        return ScriptException("Animation startTimer(): Too few arguments");
    if(!args[0]->IsFunction() || !args[1]->IsNumber())
        return ScriptException("Animation startTimer(): Invalid argument");
    
    AnimationTimer* interval = new AnimationTimer(v8::Handle<v8::Function>::Cast(args[0]), args[1]->NumberValue());
    scriptManager->timers.insert(interval);
    ScriptReturn(interval->function);
}

void ScriptAnimation::StopTimer(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() == 0)
        return ScriptException("Animation stopTimer(): Too few arguments");
    if(!args[0]->IsFunction())
        return ScriptException("Animation stopTimer(): Invalid argument");
    
    for(auto interval : scriptManager->timers)
        if(interval->function == args[0]) {
            delete interval;
            scriptManager->timers.erase(interval);
            ScriptReturn(true);
        }
    ScriptReturn(false);
}

ScriptAnimation::ScriptAnimation() :ScriptClass("Animation", Constructor) {
    ScriptScope();
    
    (*functionTemplate)->Set(ScriptString("addFrames"), ScriptMethod(AddFrames));
    (*functionTemplate)->Set(ScriptString("removeFrames"), ScriptMethod(RemoveFrames));
    (*functionTemplate)->Set(ScriptString("getTrackInfo"), ScriptMethod(GetTrackInfo));
    (*functionTemplate)->Set(ScriptString("startTimer"), ScriptMethod(StartTimer));
    (*functionTemplate)->Set(ScriptString("stopTimer"), ScriptMethod(StopTimer));
}