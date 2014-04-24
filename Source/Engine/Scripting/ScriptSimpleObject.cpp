//
//  ScriptSimpleObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptCamObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "CamObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(CamObject);

static JSValueRef ScriptCamObjectGetFov(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<CamObject>(instance);
    return JSValueMakeNumber(context, objectPtr->fov);
}

static bool ScriptCamObjectSetFov(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "CamObject setFov(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<CamObject>(instance)->fov = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptCamObjectGetNear(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<CamObject>(instance);
    return JSValueMakeNumber(context, objectPtr->nearPlane);
}

static bool ScriptCamObjectSetNear(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "CamObject setNear(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue > 0.0) {
        getDataOfInstance<CamObject>(instance)->nearPlane = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptCamObjectGetFar(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<CamObject>(instance);
    return JSValueMakeNumber(context, objectPtr->farPlane);
}

static bool ScriptCamObjectSetFar(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "CamObject setFar(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue > 0.0) {
        getDataOfInstance<CamObject>(instance)->farPlane = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptCamObjectGetIsMainCam(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<CamObject>(instance);
    return JSValueMakeBoolean(context, objectPtr == mainCam);
}

static bool ScriptCamObjectSetIsMainCam(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "CamObject setIsMainCam(): Expected Boolean");
        return false;
    }
    if(JSValueMakeBoolean(context, value))
        mainCam = getDataOfInstance<CamObject>(instance);
    return true;
}

static JSValueRef ScriptCamObjectGetViewRay(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 2 || !JSValueIsNumber(context, argv[0]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "CamObject getViewRay(): Expected Number, Number");
    auto objectPtr = getDataOfInstance<CamObject>(instance);
    Ray3 ray = objectPtr->getRayAt(JSValueToNumber(context, argv[0], NULL), JSValueToNumber(context, argv[1], NULL));
    JSObjectRef result = JSObjectMake(context, NULL, NULL);
    JSObjectSetProperty(context, result, ScriptStringOrigin.str, newScriptVector3(context, ray.origin), 0, NULL);
    JSObjectSetProperty(context, result, ScriptStringDirection.str, newScriptVector3(context, ray.direction), 0, NULL);
    return result;
}

JSStaticValue ScriptCamObjectProperties[] = {
    {"fov", ScriptCamObjectGetFov, ScriptCamObjectSetFov, kJSPropertyAttributeDontDelete},
    {"near", ScriptCamObjectGetNear, ScriptCamObjectSetNear, kJSPropertyAttributeDontDelete},
    {"far", ScriptCamObjectGetFar, ScriptCamObjectSetFar, kJSPropertyAttributeDontDelete},
    {"isMainCam", ScriptCamObjectGetIsMainCam, ScriptCamObjectSetIsMainCam, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptCamObjectMethods[] = {
    {"getViewRay", ScriptCamObjectGetViewRay, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(CamObject, ScriptCamObjectProperties, ScriptCamObjectMethods);



static JSObjectRef ScriptSoundObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "SoundObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(SoundObject);

static JSValueRef ScriptSoundObjectGetSoundTrack(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<SoundObject>(instance);
    std::string name;
    FilePackage* filePackage = fileManager.findResource<SoundTrack>(objectPtr->soundTrack, name);
    if(filePackage) {
        ScriptString strName(fileManager.getPathOfResource(filePackage, name));
        return strName.getJSStr(context);
    }else
        return ScriptException(context, exception, "SoundObject getSoundTrack(): Internal error");
}

static bool ScriptSoundObjectSetSoundTrack(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "SoundObject setSoundTrack(): Expected String");
        return false;
    }
    ScriptString strName(context, value);
    auto objectPtr = getDataOfInstance<SoundObject>(instance);
    auto soundTrack = fileManager.getResourceByPath<SoundTrack>(levelManager.levelPackage, strName.getStdStr());
    if(soundTrack) {
        objectPtr->setSoundTrack(soundTrack);
        return true;
    }else
        return false;
}

static JSValueRef ScriptSoundObjectGetTimeOffset(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<SoundObject>(instance)->getTimeOffset());
}

static bool ScriptSoundObjectSetTimeOffset(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SoundObject setTimeOffset(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    auto objectPtr = getDataOfInstance<SoundObject>(instance);
    if(objectPtr->soundTrack && numberValue >= 0.0 && numberValue <= objectPtr->soundTrack->getLength()) {
        objectPtr->setTimeOffset(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptSoundObjectGetVolume(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<SoundObject>(instance)->getVolume());
}

static bool ScriptSoundObjectSetVolume(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SoundObject setVolume(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0 && numberValue <= 1.0) {
        getDataOfInstance<SoundObject>(instance)->setVolume(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptSoundObjectGetIsPlaying(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<SoundObject>(instance)->getIsPlaying());
}

static bool ScriptSoundObjectSetIsPlaying(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "SoundObject setIsPlaying(): Expected Boolean");
        return false;
    }
    getDataOfInstance<SoundObject>(instance)->setIsPlaying(JSValueToBoolean(context, value));
    return true;
}

static JSValueRef ScriptSoundObjectGetMode(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<SoundObject>(instance);
    switch(objectPtr->mode) {
        case SoundObject::Mode::Looping:
            return ScriptStringLooping.getJSStr(context);
        case SoundObject::Mode::Hold:
            return ScriptStringHold.getJSStr(context);
        case SoundObject::Mode::Dispose:
            return ScriptStringDispose.getJSStr(context);
        default:
            return ScriptException(context, exception, "SoundObject getMode(): Internal error");
    }
}

static bool ScriptSoundObjectSetMode(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "SoundObject setSoundTrack(): Expected String");
        return false;
    }
    ScriptString strMode(context, value);
    std::string mode = strMode.getStdStr();
    auto objectPtr = getDataOfInstance<SoundObject>(instance);
    if(mode == "looping") {
        objectPtr->mode = SoundObject::Mode::Looping;
        return true;
    }else if(mode == "hold") {
        objectPtr->mode = SoundObject::Mode::Hold;
        return true;
    }else if(mode == "dispose") {
        objectPtr->mode = SoundObject::Mode::Dispose;
        return true;
    }else
        return false;
}

JSStaticValue ScriptSoundObjectProperties[] = {
    {"soundTrack", ScriptSoundObjectGetSoundTrack, ScriptSoundObjectSetSoundTrack, kJSPropertyAttributeDontDelete},
    {"timeOffset", ScriptSoundObjectGetTimeOffset, ScriptSoundObjectSetTimeOffset, kJSPropertyAttributeDontDelete},
    {"volume", ScriptSoundObjectGetVolume, ScriptSoundObjectSetVolume, kJSPropertyAttributeDontDelete},
    {"isPlaying", ScriptSoundObjectGetIsPlaying, ScriptSoundObjectSetIsPlaying, kJSPropertyAttributeDontDelete},
    {"mode", ScriptSoundObjectGetMode, ScriptSoundObjectSetMode, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

ScriptClassDefinition(SoundObject, ScriptSoundObjectProperties, NULL);