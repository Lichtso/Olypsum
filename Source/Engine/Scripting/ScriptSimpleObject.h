//
//  ScriptSimpleObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptSimpleObject_h
#define ScriptSimpleObject_h

#include "ScriptDisplayObject.h"

class ScriptCamObject : public ScriptBaseObject {
    static v8::Handle<v8::Value> GetFov(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetNear(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFar(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetViewRay(const v8::Arguments& args);
    static v8::Handle<v8::Value> SetMainCam(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetMainCam(const v8::Arguments& args);
    public:
    ScriptCamObject();
};

class ScriptSoundObject : public ScriptBaseObject {
    static v8::Handle<v8::Value> GetSoundTrack(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTimeOffset(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetVolume(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetPlaying(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetMode(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptSoundObject();
};

extern ScriptCamObject scriptCamObject;
extern ScriptSoundObject scriptSoundObject;

#endif
