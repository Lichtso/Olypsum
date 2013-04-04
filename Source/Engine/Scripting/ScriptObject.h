//
//  ScriptObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//
//

#include "ScriptLinearAlgebra.h"

#ifndef ScriptObject_h
#define ScriptObject_h

class ScriptBaseObject : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetTransformation(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTransformation(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetScriptClass(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinkNames(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetLinkedObject(const v8::Arguments& args);
    public:
    static BaseObject* getDataOfInstance(const v8::Local<v8::Value>& object);
    ScriptBaseObject();
};

class ScriptPhysicObject : public ScriptClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetCollisionShape(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetCollisionShapeInfo(const v8::Arguments& args);
    public:
    static PhysicObject* getDataOfInstance(const v8::Local<v8::Value>& object);
    ScriptPhysicObject();
};

extern ScriptBaseObject scriptBaseObject;
extern ScriptPhysicObject scriptPhysicObject;

#endif
