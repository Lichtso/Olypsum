//
//  ScriptSimpleObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptSimpleObject_h
#define ScriptSimpleObject_h

#include "ScriptVisualObject.h"

class ScriptSimpleObject : public ScriptBaseObject {
    protected:
    ScriptSimpleObject(const char* name) :ScriptBaseObject(name) { }
    public:
    ScriptSimpleObject();
};

class ScriptCamObject : public ScriptSimpleObject {
    static void GetFov(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetNear(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetFar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(GetViewRay);
    ScriptDeclareMethod(SetMainCam);
    ScriptDeclareMethod(GetMainCam);
    public:
    ScriptCamObject();
};

class ScriptSoundObject : public ScriptSimpleObject {
    static void GetSoundTrack(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetTimeOffset(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetVolume(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetPlaying(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    public:
    ScriptSoundObject();
};

extern std::unique_ptr<ScriptSimpleObject> scriptSimpleObject;
extern std::unique_ptr<ScriptCamObject> scriptCamObject;
extern std::unique_ptr<ScriptSoundObject> scriptSoundObject;

#endif
