//
//  ScriptObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//
//

#include "ScriptObject.h"

v8::Handle<v8::Value> ScriptBaseObject::Constructor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !args[0]->IsExternal())
        return v8::ThrowException(v8::String::New("BaseObject Constructor: Class can't be instantiated"));
    
    args.This()->SetInternalField(0, args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptBaseObject::SetTransformation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() != 1 || !scriptMatrix4.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("BaseObject setTransform: Invalid argument"));
    BaseObject* objectPtr = getDataOfInstance(args.This());
    objectPtr->setTransformation(scriptMatrix4.getDataOfInstance(args[0]).getBTTransform());
    return handleScope.Close(args.This());
}

v8::Handle<v8::Value> ScriptBaseObject::GetTransformation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    BaseObject* objectPtr = getDataOfInstance(args.This());
    Matrix4 transformation(objectPtr->getTransformation());
    return handleScope.Close(scriptMatrix4.newInstance(transformation));
}

BaseObject* ScriptBaseObject::getDataOfInstance(const v8::Local<v8::Value>& value) {
    v8::HandleScope handleScope;
    v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
    v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
    return static_cast<BaseObject*>(wrap->Value());
}

v8::Local<v8::Object> ScriptBaseObject::newInstance(BaseObject* objectPtr) {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(objectPtr);
    v8::Local<v8::Object> object = functionTemplate->GetFunction()->NewInstance(1, &external);
    return handleScope.Close(object);
}

ScriptBaseObject::ScriptBaseObject() :ScriptClass("BaseObject", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> instanceTemplate = functionTemplate->InstanceTemplate();
    instanceTemplate->SetInternalFieldCount(1);
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("setTransform"), v8::FunctionTemplate::New(SetTransformation));
    objectTemplate->Set(v8::String::New("getTransform"), v8::FunctionTemplate::New(GetTransformation));
}

ScriptBaseObject scriptBaseObject;