//
//  ScriptAnimation.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 16.05.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

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
    {"startTimer", ScriptAnimationStartTimer, ScriptMethodAttributes},
    {"stopTimer", ScriptAnimationStopTimer, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Animation, ScriptAnimationProperties, ScriptAnimationMethods);