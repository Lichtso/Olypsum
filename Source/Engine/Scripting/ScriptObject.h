//
//  ScriptObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//
//

#ifndef ScriptObject_h
#define ScriptObject_h

#include "ScriptLinearAlgebra.h"

class ScriptBaseClass : public ScriptClass {
    static v8::Handle<v8::Value> GetScriptClass(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    ScriptBaseClass(const char* name);
    public:
    template<typename T> static T* getDataOfInstance(const v8::Local<v8::Value>& value) {
        v8::HandleScope handleScope;
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    ScriptBaseClass() :ScriptBaseClass("BaseClass") { };
};

class ScriptBaseObject : public ScriptBaseClass {
    static v8::Handle<v8::Value> AccessTransformation(const v8::Arguments& args);
    static v8::Handle<v8::Value> RemoveLink(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetLink(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetLinkNames(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetLinkedObject(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetPath(const v8::Arguments& args);
    protected:
    ScriptBaseObject(const char* name) :ScriptBaseClass(name) { }
    public:
    ScriptBaseObject();
};

class ScriptPhysicObject : public ScriptBaseObject {
    static v8::Handle<v8::Value> GetCollisionShape(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetCollisionShapeInfo(const v8::Arguments& args);
    protected:
    ScriptPhysicObject(const char* name) :ScriptBaseObject(name) { }
    public:
    ScriptPhysicObject();
};

extern ScriptBaseClass scriptBaseClass;
extern ScriptBaseObject scriptBaseObject;
extern ScriptPhysicObject scriptPhysicObject;

#endif
