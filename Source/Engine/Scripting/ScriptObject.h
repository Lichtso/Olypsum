//
//  ScriptObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptObject_h
#define ScriptObject_h

#include "ScriptLinearAlgebra.h"

class ScriptBaseClass : public ScriptClass {
    static void GetScriptClass(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetScriptClass(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void Delete(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    ScriptBaseClass(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args) = Constructor);
    public:
    template<typename T> static T* getDataOfInstance(const v8::Local<v8::Value>& value) {
        v8::HandleScope handleScope;
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    ScriptBaseClass();
};

class ScriptBaseObject : public ScriptBaseClass {
    static void AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetLink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetLinkCount(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetTransformUpLink(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetBoneUpLink(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptBaseObject(const char* name) :ScriptBaseClass(name) { }
    public:
    ScriptBaseObject();
};

class ScriptPhysicObject : public ScriptBaseObject {
    static void GetCollisionShape(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetCollisionShape(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetCollisionShapeInfo(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    static void GetAngularFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearFriction(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearFriction(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetRestitution(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetRestitution(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptPhysicObject(const char* name) :ScriptBaseObject(name) { }
    public:
    ScriptPhysicObject();
};

extern ScriptBaseClass scriptBaseClass;
extern ScriptBaseObject scriptBaseObject;
extern ScriptPhysicObject scriptPhysicObject;

#endif
