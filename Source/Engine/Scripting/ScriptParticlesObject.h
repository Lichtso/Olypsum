//
//  ScriptParticlesObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//
//

#include "ScriptSimpleObject.h"

#ifndef ScriptParticlesObject_h
#define ScriptParticlesObject_h

class ScriptParticlesObject : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetMaxParticles(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTexture(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTexture(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTransformAligned(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTransformAligned(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSystemLife(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSystemLife(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLifeMin(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLifeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLifeMax(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLifeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSizeMin(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSizeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSizeMax(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSizeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessForce(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessPosMin(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessPosMax(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessDirMin(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessDirMax(const v8::Arguments& args);
    public:
    ScriptParticlesObject();
};

extern ScriptParticlesObject scriptParticlesObject;

#endif
