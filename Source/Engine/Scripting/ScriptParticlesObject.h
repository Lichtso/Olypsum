//
//  ScriptParticlesObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptParticlesObject_h
#define ScriptParticlesObject_h

#include "ScriptSimpleObject.h"

class ScriptParticlesObject : public ScriptPhysicObject {
    static void GetMaxParticles(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetTexture(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetTexture(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetTransformAligned(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetTransformAligned(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetSystemLife(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSystemLife(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLifeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLifeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLifeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLifeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetSizeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSizeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetSizeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSizeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AccessForce(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessPosMin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessPosMax(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessDirMin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessDirMax(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptParticlesObject();
};

extern ScriptParticlesObject scriptParticlesObject;

#endif
