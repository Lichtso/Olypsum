//
//  ScriptDisplayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//
//

#ifndef ScriptDisplayObject_h
#define ScriptDisplayObject_h

#include "ScriptObject.h"

class ScriptModelObject : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetIntegrity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetIntegrity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetModel(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetModel(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessTextureAnimation(const v8::Arguments& args);
    static v8::Handle<v8::Value> FindBoneByPath(const v8::Arguments& args);
    protected:
    ScriptModelObject(const char* name) :ScriptPhysicObject(name) { }
    public:
    ScriptModelObject();
};

class ScriptRigidObject : public ScriptModelObject {
    static v8::Handle<v8::Value> GetCollisionShape(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetMass(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetMass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetKinematic(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetKinematic(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessAngularVelocity(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessLinearVelocity(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessAngularFactor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessLinearFactor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetAngularDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearDamping(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearDamping(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessTransformation(const v8::Arguments& args);
    static v8::Handle<v8::Value> ApplyImpulseAtPoint(const v8::Arguments& args);
    static v8::Handle<v8::Value> ApplyAngularImpulse(const v8::Arguments& args);
    static v8::Handle<v8::Value> ApplyLinearImpulse(const v8::Arguments& args);
    public:
    ScriptRigidObject();
};

class ScriptTerrainObject : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLength(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetBitDepth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetBitDepth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> UpdateModel(const v8::Arguments& args);
    public:
    ScriptTerrainObject();
};

extern ScriptModelObject scriptModelObject;
extern ScriptRigidObject scriptRigidObject;
extern ScriptTerrainObject scriptTerrainObject;

#endif