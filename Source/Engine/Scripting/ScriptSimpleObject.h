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
    static void GetFov(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetNear(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetFar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetViewRay(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void SetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptCamObject();
};

class ScriptSoundObject : public ScriptSimpleObject {
    static void GetSoundTrack(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetTimeOffset(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetVolume(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetPlaying(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    public:
    ScriptSoundObject();
};

extern ScriptSimpleObject scriptSimpleObject;
extern ScriptCamObject scriptCamObject;
extern ScriptSoundObject scriptSoundObject;

#endif
