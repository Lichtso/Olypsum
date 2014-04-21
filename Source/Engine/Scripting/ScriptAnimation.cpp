//
//  ScriptAnimation.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

/*static JSValueRef ScriptCopyValue(JSContextRef context, JSValueRef value) {
    if(JSValueIsNumber(context, value)) {
        value = JSValueMakeNumber(context, JSValueToNumber(context, value, NULL));
    }else if(JSValueIsBoolean(context, value)) {
        value = JSValueMakeBoolean(context, JSValueToBoolean(context, value));
    }else if(JSValueIsObjectOfClass(context, value, ScriptClasses[ScriptVector3])) {
        value = newScriptVector3(context, getScriptVector3(context, value));
    }else if(JSValueIsObjectOfClass(context, value, ScriptClasses[ScriptQuaternion])) {
        value = newScriptQuaternion(context, getScriptQuaternion(context, value));
    }else if(JSValueIsObjectOfClass(context, value, ScriptClasses[ScriptMatrix4])) {
        value = newScriptMatrix4(context, *getDataOfInstance<Matrix4>(context, value));
    }else
        return NULL;
    
    return value;
}

static JSValueRef accessProperty(JSObjectRef object, const std::string& property, JSValueRef setValue) {
    ScriptString strKey(property);
    JSContextRef context = scriptManager->mainScript->context;
    JSValueRef returnValue, exception = NULL;
    JSObjectRef valueObject;
    std::smatch match;
    const std::regex regex("(.+)\\[([0-9]+)\\]");
    if(std::regex_match(property, match, regex)) {
        ScriptString strKey(match[0].str());
        returnValue = JSObjectGetProperty(context, object, strKey.str, NULL);
        valueObject = JSValueToObject(context, returnValue, NULL);
        unsigned int index = 0;
        sscanf(match[1].str().c_str(), "%d", &index);
        if(valueObject) {
            if(JSObjectIsFunction(context, valueObject)) {
                JSValueRef argv[] = { JSValueMakeNumber(context, index), setValue };
                returnValue = JSObjectCallAsFunction(context, valueObject, object, (setValue)?1:2, argv, &exception);
                if(exception) {
                    ScriptLogException(context, exception);
                    return NULL;
                }
            }else{
                returnValue = JSObjectGetPropertyAtIndex(context, valueObject, index, NULL);
                if(returnValue && setValue) {
                    JSObjectSetPropertyAtIndex(context, valueObject, index, setValue, NULL);
                    returnValue = setValue;
                }
            }
        }
    }else{
        returnValue = JSObjectGetProperty(context, object, strKey.str, NULL);
        valueObject = JSValueToObject(context, returnValue, NULL);
        if(valueObject && JSObjectIsFunction(context, valueObject)) {
            returnValue = JSObjectCallAsFunction(context, valueObject, object, (setValue)?0:1, &setValue, &exception);
            if(exception) {
                ScriptLogException(context, exception);
                return NULL;
            }
        }else if(setValue) {
            JSObjectSetProperty(context, object, strKey.str, setValue, 0, NULL);
            returnValue = setValue;
        }
    }
    return returnValue;
}

AnimationFrame::AnimationFrame(float _acceleration, float _duration, JSValueRef _value)
    :acceleration(_acceleration), duration(_duration), value(_value) {
    
}

AnimationTrack::AnimationTrack(JSObjectRef _object)
    :time(0.0), looping(false), object(_object) {
    JSValueProtect(scriptManager->mainScript->context, object);
}

AnimationTrack::~AnimationTrack() {
    JSContextRef context = scriptManager->mainScript->context;
    for(auto iterator : frames) {
        JSValueUnprotect(context, iterator->value);
        delete iterator;
    }
    JSValueUnprotect(context, object);
}

bool AnimationTrack::update(const std::string& property) {
    if(frames.size() < 2 || !frames[0]->value || !frames[1]->value)
        return false;
    
    float t = min(time / frames[1]->duration, 1.0F), t2 = t*t, t3 = t2*t;
    t = 3.0*t2-2.0*t3+(t3-2.0*t2+t)*frames[0]->acceleration+(t3-t2)*frames[1]->acceleration;
    
    JSValueRef value;
    JSContextRef context = scriptManager->mainScript->context;
    if(JSValueIsNumber(context, frames[0]->value)) {
        double a = JSValueToNumber(context, frames[0]->value, NULL), b = JSValueToNumber(context, frames[1]->value, NULL);
        value = JSValueMakeNumber(context, a+(b-a)*t);
    }else if(JSValueIsBoolean(context, frames[0]->value)) {
        bool a = JSValueToBoolean(context, frames[0]->value), b = JSValueToBoolean(context, frames[1]->value);
        value = JSValueMakeBoolean(context, (t < 0.5) ? a : b);
    }else if(JSValueIsObjectOfClass(context, frames[0]->value, ScriptClasses[ScriptVector3])) {
        btVector3 a = getScriptVector3(context, frames[0]->value),
                  b = getScriptVector3(context, frames[1]->value);
        value = newScriptVector3(context, a.lerp(b, t));
    }else if(JSValueIsObjectOfClass(context, frames[0]->value, ScriptClasses[ScriptQuaternion])) {
        btQuaternion a = getScriptQuaternion(context, frames[0]->value),
                     b = getScriptQuaternion(context, frames[1]->value);
        value = newScriptQuaternion(context, a.slerp(b, t));
    }else if(JSValueIsObjectOfClass(context, frames[0]->value, ScriptClasses[ScriptMatrix4])) {
        Matrix4 *a = getDataOfInstance<Matrix4>(context, frames[0]->value),
                *b = getDataOfInstance<Matrix4>(context, frames[1]->value);
        value = newScriptMatrix4(context, a->getInterpolation(*b, t));
    }else
        return false;
    
    return accessProperty(object, property, value);
}

bool AnimationTrack::gameTick(const std::string& property) {
    time += profiler.animationFactor;
    if(!update(property)) return false;
    
    while(time >= frames[1]->duration && frames.size() > 1) {
        time -= frames[1]->duration;
        AnimationFrame* frame = frames[0];
        if(looping) {
            frames.erase(frames.begin());
            frames.push_back(frame);
        }else{
            JSValueUnprotect(scriptManager->mainScript->context, frame->value);
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

AnimationTrack* AnimationProperty::find(JSObjectRef object) {
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

bool AnimationProperty::gameTick(const std::string& property) {
    for(unsigned int i = 0; i < tracks.size(); i ++)
        if(tracks[i]->gameTick(property)) {
            delete tracks[i];
            tracks.erase(tracks.begin()+i);
            i --;
        }
    return (tracks.size() == 0);
}*/



AnimationTimer::AnimationTimer(JSObjectRef _function, double _timeLength)
    :timeLength(_timeLength), timeNext(_timeLength+getTime()), function(_function) {
    JSValueProtect(scriptManager->mainScript->context, function);
}

AnimationTimer::~AnimationTimer() {
    JSValueUnprotect(scriptManager->mainScript->context, function);
}

bool AnimationTimer::gameTick(double timeNow) {
    JSContextRef context = scriptManager->mainScript->context;
    JSValueRef exception = NULL, result = JSObjectCallAsFunction(context, function, function, 0, NULL, &exception);
    if(exception) {
        ScriptLogException(context, exception);
        return true;
    }
    
    if(scriptManager->timers.find(this) == scriptManager->timers.end())
        return false;
    
    if(timeLength > 0.0 && JSValueToBoolean(context, result)) {
        if(timeNow-timeNext > timeLength)
            timeNext = timeNow+timeLength;
        else
            timeNext += timeLength;
        return false;
    }
    
    return true;
}



static JSValueRef ScriptAnimationGetFactor(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, profiler.animationFactor);
}

/*static JSValueRef ScriptAnimationAddFrames(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 6 && JSValueIsString(context, argv[1]) && JSValueIsBoolean(context, argv[2])) {
        JSObjectRef object = JSValueToObject(context, argv[0], NULL),
                    accelerations = JSValueToObject(context, argv[3], NULL),
                    durations = JSValueToObject(context, argv[4], NULL),
                    values = JSValueToObject(context, argv[5], NULL);
        
        if(object && accelerations && durations && values) {
            unsigned int lengthA = JSValueToNumber(context, JSObjectGetProperty(context, accelerations, ScriptStringLength.str, NULL), NULL),
                         lengthB = JSValueToNumber(context, JSObjectGetProperty(context, durations, ScriptStringLength.str, NULL), NULL),
                         lengthC = JSValueToNumber(context, JSObjectGetProperty(context, values, ScriptStringLength.str, NULL), NULL);
            
            if(lengthA == 0 || lengthA != lengthB || lengthB != lengthC)
                return ScriptException(context, exception, "Animation addFrames(): Invalid frame arrays");
            
            ScriptString strProperty(context, argv[1]);
            std::string property = strProperty.getStdStr();
            
            AnimationProperty* animationProperty;
            auto iterator = scriptManager->animations.find(property);
            if(iterator == scriptManager->animations.end()) {
                animationProperty = new AnimationProperty();
                scriptManager->animations[property] = animationProperty;
            }else
                animationProperty = iterator->second;
            
            AnimationTrack* track = animationProperty->find(object);
            if(!track) {
                track = new AnimationTrack(object);
                animationProperty->tracks.push_back(track);
            }
            track->looping = JSValueToBoolean(context, argv[2]);
            
            JSValueRef defaultValue = accessProperty(object, property, NULL);
            if(!defaultValue)
                return ScriptException(context, exception, "Animation addFrames(): Invalid property");
            
            for(unsigned int i = 0; i < lengthA; i ++) {
                JSValueRef acceleration = JSObjectGetPropertyAtIndex(context, accelerations, i, NULL),
                           duration = JSObjectGetPropertyAtIndex(context, durations, i, NULL),
                           value = JSObjectGetPropertyAtIndex(context, values, i, NULL);
                
                if(!JSValueIsNumber(context, acceleration) || !JSValueIsNumber(context, duration))
                    return ScriptException(context, exception, "Animation addFrames(): Invalid frame");
                
                track->frames.push_back(new AnimationFrame(JSValueToNumber(context, acceleration, NULL),
                                                           JSValueToNumber(context, duration, NULL),
                                                           (!value || JSValueIsNull(context, value)) ? defaultValue : value));
            }
            
            track->update(property.c_str());
            return JSValueMakeUndefined(context);
        }
    }
    return ScriptException(context, exception, "Animation addFrames(): Expected Object, String, Boolean, Array, Array, Array");
}

static JSValueRef ScriptAnimationDeleteFrames(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 4 && JSValueIsString(context, argv[1]) && JSValueIsNumber(context, argv[2]) && JSValueIsNumber(context, argv[3])) {
        JSObjectRef object = JSValueToObject(context, argv[0], NULL);
        if(object) {
            ScriptString strProperty(context, argv[1]);
            std::string property = strProperty.getStdStr();
            
            auto iterator = scriptManager->animations.find(property);
            if(iterator == scriptManager->animations.end())
                return ScriptException(context, exception, "Animation deleteFrames(): Invalid track");
            
            AnimationProperty* animationProperty = iterator->second;
            AnimationTrack* track = animationProperty->find(object);
            if(!track)
                return ScriptException(context, exception, "Animation deleteFrames(): Invalid track");
            
            unsigned int offset = JSValueToNumber(context, argv[2], NULL),
                         length = JSValueToNumber(context, argv[3], NULL),
                         endOffset = offset+length;
            
            if(offset+length > track->frames.size())
                return ScriptException(context, exception, "Animation deleteFrames(): Out of bounds");
            
            if(length >= track->frames.size()-2) {
                delete track;
                animationProperty->tracks.erase(animationProperty->tracks.begin()+animationProperty->find(track));
                if(animationProperty->tracks.size() == 0) {
                    delete animationProperty;
                    scriptManager->animations.erase(scriptManager->animations.find(property));
                }
            }else{
                if(offset < 2) track->time = 0.0;
                for(int i = offset; i < endOffset; i ++) {
                    JSValueUnprotect(context, track->frames[i]->value);
                    delete track->frames[i];
                }
                track->frames.erase(track->frames.begin()+offset, track->frames.begin()+endOffset);
            }
        }
    }
    return ScriptException(context, exception, "Animation deleteFrames(): Expected Object, String, Number, Number, Number");
}

static JSValueRef ScriptAnimationGetTrackInfo(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 2 && JSValueIsString(context, argv[1])) {
        JSObjectRef object = JSValueToObject(context, argv[0], NULL);
        if(object) {
            ScriptString strProperty(context, argv[1]);
            std::string property = strProperty.getStdStr();
            
            auto iterator = scriptManager->animations.find(property);
            if(iterator == scriptManager->animations.end())
                return JSValueMakeNull(context);
            
            AnimationProperty* animationProperty = iterator->second;
            AnimationTrack* track = animationProperty->find(object);
            if(!track)
                return JSValueMakeNull(context);
            
            JSObjectRef result = JSObjectMake(context, NULL, NULL);
            JSObjectSetProperty(context, result, ScriptStringFrames.str, JSValueMakeNumber(context, track->frames.size()), 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringLooping.str, JSValueMakeBoolean(context, track->looping), 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringTime.str, JSValueMakeNumber(context, track->time), 0, NULL);
            return result;
        }
    }
    return ScriptException(context, exception, "Animation getTrackInfo(): Expected Object, String");
}*/

static JSValueRef ScriptAnimationStartTimer(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        JSObjectRef function = JSValueToObject(context, argv[0], NULL);
        if(JSObjectIsFunction(context, function)) {
            AnimationTimer* interval = new AnimationTimer(function, JSValueToNumber(context, argv[1], NULL));
            scriptManager->timers.insert(interval);
            return function;
        }
    }
    return ScriptException(context, exception, "Animation startTimer(): Expected Function, Number");
}

static JSValueRef ScriptAnimationStopTimer(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 1) {
        JSObjectRef function = JSValueToObject(context, argv[0], NULL);
        if(JSObjectIsFunction(context, function)) {
            for(auto interval : scriptManager->timers)
                if(interval->function == function) {
                    delete interval;
                    scriptManager->timers.erase(interval);
                    return JSValueMakeBoolean(context, true);
                }
            return JSValueMakeBoolean(context, false);
        }
    }
    return ScriptException(context, exception, "Animation stopTimer(): Expected Function");
}

JSStaticValue ScriptAnimationProperties[] = {
    {"factor", ScriptAnimationGetFactor, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptAnimationMethods[] = {
    //{"addFrames", ScriptAnimationAddFrames, ScriptMethodAttributes},
    //{"deleteFrames", ScriptAnimationDeleteFrames, ScriptMethodAttributes},
    //{"getTrackInfo", ScriptAnimationGetTrackInfo, ScriptMethodAttributes},
    {"startTimer", ScriptAnimationStartTimer, ScriptMethodAttributes},
    {"stopTimer", ScriptAnimationStopTimer, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Animation, ScriptAnimationProperties, ScriptAnimationMethods);