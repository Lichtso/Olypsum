//
//  ScriptAnimation.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//
//

#include "ScriptManager.h"

AnimationFrame::AnimationFrame(float _acceleration, float _duration, v8::Handle<v8::Value> _value)
    :acceleration(_acceleration), duration(_duration),
    value(v8::Persistent<v8::Value>::New(v8::Isolate::GetCurrent(), _value)) {
    
}

AnimationTrack::AnimationTrack(v8::Handle<v8::Object> _object)
    :object(v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), _object)),
    time(0.0), looping(false) {
    
}

bool AnimationTrack::update(const char* property) {
    v8::HandleScope handleScope;
    if(frames.size() <= 1 || frames[0].value.IsEmpty() || frames[1].value.IsEmpty())
        return false;
    
    float t = time / frames[1].duration, t2 = t*t, t3 = t2*t;
    t = 3.0*t2-2.0*t3+(t3-2.0*t2+t)*frames[0].acceleration+(t3-t2)*frames[1].acceleration;
    
    v8::Handle<v8::Value> value;
    if(frames[0].value->IsNumber()) {
        double a = frames[0].value->NumberValue(), b = frames[1].value->NumberValue();
        value = v8::Number::New(a+(b-a)*t);
    }else if(frames[0].value->IsInt32()) {
        int a = frames[0].value->IntegerValue(), b = frames[1].value->IntegerValue();
        value = v8::Integer::New(a+(b-a)*t);
    }else if(frames[0].value->IsBoolean()) {
        bool a = frames[0].value->BooleanValue(), b = frames[1].value->BooleanValue();
        value = v8::Boolean::New((t < 0.5) ? a : b);
    }else if(scriptVector3.isCorrectInstance(*frames[0].value)) {
        btVector3 a = scriptVector3.getDataOfInstance(*frames[0].value),
                  b = scriptVector3.getDataOfInstance(*frames[1].value);
        value = scriptVector3.newInstance(a.lerp(b, t));
    }else if(scriptQuaternion.isCorrectInstance(*frames[0].value)) {
        btQuaternion a = scriptQuaternion.getDataOfInstance(*frames[0].value),
                     b = scriptQuaternion.getDataOfInstance(*frames[1].value);
        value = scriptQuaternion.newInstance(a.slerp(b, t));
    }else if(scriptMatrix4.isCorrectInstance(*frames[0].value)) {
        Matrix4 *a = scriptMatrix4.getDataOfInstance(*frames[0].value),
                *b = scriptMatrix4.getDataOfInstance(*frames[1].value);
        value = scriptMatrix4.newInstance(a->getInterpolation(*b, t));
    }else
        return false;
    
    v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(object->Get(v8::String::New(property)));
    if(!function.IsEmpty())
        function->CallAsFunction(object, 1, &value);
    else
        object->Set(v8::String::New(property), value);
    
    return true;
}

bool AnimationTrack::gameTick(const char* property) {
    if(!update(property)) return false;
    
    time += profiler.animationFactor;
    while(time >= frames[1].duration && frames.size() > 1) {
        time -= frames[1].duration;
        if(looping) {
            AnimationFrame frame = frames[0];
            frames.erase(frames.begin());
            frames.push_back(frame);
        }else
            frames.erase(frames.begin());
    }
    
    return (frames.size() <= 1);
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
    :function(v8::Persistent<v8::Function>::New(v8::Isolate::GetCurrent(), _function)),
    timeLength(_timeLength), timeNext(_timeLength+getTime()) {
    
}

bool AnimationTimer::gameTick(double timeNow) {
    v8::HandleScope handleScope;
    v8::TryCatch tryCatch;
    v8::Handle<v8::Value> result = function->CallAsFunction(function, 0, NULL);
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



v8::Handle<v8::Value> ScriptAnimation::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    return v8::ThrowException(v8::String::New("Animation Constructor: Class can't be instantiated"));
}

v8::Handle<v8::Value> ScriptAnimation::AddFrames(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 6)
        return v8::ThrowException(v8::String::New("addFrames(): Too few arguments"));
    if(!args[0]->IsObject() || !args[1]->IsString()|| !args[2]->IsBoolean() || !args[3]->IsArray() || !args[4]->IsArray() || !args[5]->IsArray())
        return v8::ThrowException(v8::String::New("addFrames(): Invalid argument"));
    
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
        return v8::ThrowException(v8::String::New("addFrames(): Invalid frames"));
    
    if(accelerations->Length() == 0)
        return v8::Undefined();
    
    v8::Handle<v8::Value> defaultValue = args[0]->ToObject()->Get(args[1]);
    if(defaultValue->IsFunction()) {
        v8::Handle<v8::Function> function = v8::Handle<v8::Function>::Cast(defaultValue);
        defaultValue = function->CallAsFunction(args[0]->ToObject(), 0, NULL);
    }
    if(defaultValue.IsEmpty())
        return v8::ThrowException(v8::String::New("addFrames(): Invalid property"));
    
    for(unsigned int i = 0; i < accelerations->Length(); i ++) {
        if(!accelerations->Get(i)->IsNumber() || !durations->Get(i)->IsNumber())
            return v8::ThrowException(v8::String::New("addFrames(): Invalid frame"));
        
        track->frames.push_back(AnimationFrame(accelerations->Get(i)->NumberValue(), durations->Get(i)->NumberValue(),
                                              (values->Get(i)->IsNull()) ? defaultValue : values->Get(i)));
    }
    track->update(property.c_str());
    
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptAnimation::RemoveFrames(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 4)
        return v8::ThrowException(v8::String::New("removeFrames(): Too few arguments"));
    if(!args[0]->IsObject() || !args[1]->IsString() ||
       !args[2]->IsUint32() || !args[3]->IsInt32() || args[3]->IntegerValue() <= 0)
        return v8::ThrowException(v8::String::New("removeFrames(): Invalid argument"));
    
    std::string property = stdStrOfV8(args[1]);
    auto iterator = scriptManager->animations.find(property);
    if(iterator == scriptManager->animations.end())
        return v8::ThrowException(v8::String::New("removeFrames(): Invalid track"));
    
    AnimationProperty* animationProperty = iterator->second;
    AnimationTrack* track = animationProperty->find(args[0]->ToObject());
    if(!track)
        return v8::ThrowException(v8::String::New("removeFrames(): Invalid track"));
    
    if(track->frames.size() <= args[2]->IntegerValue()+args[3]->IntegerValue())
        return v8::ThrowException(v8::String::New("removeFrames(): Out of bounds"));
    
    if(args[3]->IntegerValue() >= track->frames.size()-2) {
        animationProperty->tracks.erase(animationProperty->tracks.begin()+animationProperty->find(track));
        if(animationProperty->tracks.size() == 0)
            scriptManager->animations.erase(scriptManager->animations.find(property));
    }else{
        if(args[2]->IntegerValue() < 2) track->time = 0.0;
        track->frames.erase(track->frames.begin()+args[2]->IntegerValue(), track->frames.begin()+args[2]->IntegerValue()+args[3]->IntegerValue());
    }
    
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptAnimation::GetTrackInfo(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("getTrackInfo(): Too few arguments"));
    if(!args[0]->IsObject() || !args[1]->IsString())
        return v8::ThrowException(v8::String::New("getTrackInfo(): Invalid argument"));
    
    std::string property = stdStrOfV8(args[1]);
    auto iterator = scriptManager->animations.find(property);
    if(iterator == scriptManager->animations.end())
        return v8::Undefined();
    
    AnimationProperty* animationProperty = iterator->second;
    AnimationTrack* track = animationProperty->find(args[0]->ToObject());
    if(!track)
        return v8::Undefined();
    
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("frames"), v8::Integer::New(track->frames.size()));
    result->Set(v8::String::New("looping"), v8::Boolean::New(track->looping));
    result->Set(v8::String::New("time"), v8::Number::New(track->time));
    return handleScope.Close(result);
}

v8::Handle<v8::Value> ScriptAnimation::StartTimer(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("startTimer(): Too few arguments"));
    if(!args[0]->IsFunction() || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("startTimer(): Invalid argument"));
    
    AnimationTimer* interval = new AnimationTimer(v8::Handle<v8::Function>::Cast(args[0]), args[1]->NumberValue());
    scriptManager->timers.insert(interval);
    return handleScope.Close(interval->function);
}

v8::Handle<v8::Value> ScriptAnimation::StopTimer(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() == 0)
        return v8::ThrowException(v8::String::New("stopTimer(): Too few arguments"));
    if(!args[0]->IsFunction())
        return v8::ThrowException(v8::String::New("stopTimer(): Invalid argument"));
    
    for(auto interval : scriptManager->timers)
        if(interval->function == args[0]) {
            scriptManager->timers.erase(interval);
            delete interval;
            return v8::Boolean::New(true);
        }
    return v8::Boolean::New(false);
}

ScriptAnimation::ScriptAnimation() :ScriptClass("Animation", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("addFrames"), v8::FunctionTemplate::New(AddFrames));
    functionTemplate->Set(v8::String::New("removeFrames"), v8::FunctionTemplate::New(RemoveFrames));
    functionTemplate->Set(v8::String::New("getTrackInfo"), v8::FunctionTemplate::New(GetTrackInfo));
    functionTemplate->Set(v8::String::New("startTimer"), v8::FunctionTemplate::New(StartTimer));
    functionTemplate->Set(v8::String::New("stopTimer"), v8::FunctionTemplate::New(StopTimer));
}

ScriptAnimation scriptAnimation;