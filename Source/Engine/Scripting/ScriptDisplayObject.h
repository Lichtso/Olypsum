//
//  ScriptDisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//
//

#include "ScriptObject.h"

#ifndef ScriptDisplayObject_h
#define ScriptDisplayObject_h

class ScriptModelObject : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetIntegrity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetModel(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTextureAnimation(const v8::Arguments& args);
    static v8::Handle<v8::Value> SetTextureAnimation(const v8::Arguments& args);
    protected:
    ScriptModelObject(const char* name);
    public:
    ScriptModelObject();
};

class ScriptRigidObject : public ScriptModelObject {
    static v8::Handle<v8::Value> GetMass(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularFactor(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearFactor(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearFactor(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> ApplyImpulseAtPoint(const v8::Arguments& args);
    static v8::Handle<v8::Value> ApplyAngularImpulse(const v8::Arguments& args);
    static v8::Handle<v8::Value> ApplyLinearImpulse(const v8::Arguments& args);
    public:
    ScriptRigidObject();
};

extern ScriptModelObject scriptModelObject;
extern ScriptRigidObject scriptRigidObject;

#endif