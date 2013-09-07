//
//  ScriptDisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptDisplayObject_h
#define ScriptDisplayObject_h

#include "ScriptObject.h"

class ScriptGraphicObject : public ScriptPhysicObject {
    static void GetIntegrity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AttachDecal(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptGraphicObject(const char* name) :ScriptPhysicObject(name) { }
    public:
    ScriptGraphicObject();
};

class ScriptRigidObject : public ScriptGraphicObject {
    static void GetModel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetMass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetKinematic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetKinematic(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AccessAngularVelocity(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessLinearVelocity(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessAngularFactor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessLinearFactor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAngularDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ApplyImpulseAtPoint(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ApplyAngularImpulse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void ApplyLinearImpulse(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetBoneByName(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessTextureAnimationTime(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptRigidObject();
};

class ScriptTerrainObject : public ScriptGraphicObject {
    static void GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetLength(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetBitDepth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void UpdateModel(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptTerrainObject();
};

extern ScriptGraphicObject scriptGraphicObject;
extern ScriptRigidObject scriptRigidObject;
extern ScriptTerrainObject scriptTerrainObject;

#endif