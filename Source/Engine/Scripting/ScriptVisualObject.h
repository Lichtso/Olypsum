//
//  ScriptVisualObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptVisualObject_h
#define ScriptVisualObject_h

#include "ScriptObject.h"

class ScriptMatterObject : public ScriptPhysicObject {
    static void GetIntegrity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(AttachDecal);
    protected:
    ScriptMatterObject(const char* name) :ScriptPhysicObject(name) { }
    public:
    ScriptMatterObject();
};

class ScriptRigidObject : public ScriptMatterObject {
    static void GetModel(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetMass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetKinematic(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetKinematic(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearDamping(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(AccessAngularVelocity);
    ScriptDeclareMethod(AccessLinearVelocity);
    ScriptDeclareMethod(AccessAngularFactor);
    ScriptDeclareMethod(AccessLinearFactor);
    ScriptDeclareMethod(AccessTransformation);
    ScriptDeclareMethod(ApplyImpulseAtPoint);
    ScriptDeclareMethod(ApplyAngularImpulse);
    ScriptDeclareMethod(ApplyLinearImpulse);
    ScriptDeclareMethod(GetBoneByName);
    ScriptDeclareMethod(AccessTextureAnimationTime);
    public:
    ScriptRigidObject();
};

class ScriptTerrainObject : public ScriptMatterObject {
    static void GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetLength(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetBitDepth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(AccessCell);
    ScriptDeclareMethod(UpdateModel);
    public:
    ScriptTerrainObject();
};

extern std::unique_ptr<ScriptMatterObject> scriptMatterObject;
extern std::unique_ptr<ScriptRigidObject> scriptRigidObject;
extern std::unique_ptr<ScriptTerrainObject> scriptTerrainObject;

#endif