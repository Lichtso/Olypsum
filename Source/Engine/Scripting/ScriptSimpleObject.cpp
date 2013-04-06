//
//  ScriptSimpleObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//
//

#include "ScriptSimpleObject.h"

v8::Handle<v8::Value> ScriptCamObject::GetFov(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->fov));
}

void ScriptCamObject::SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    objectPtr->fov = value->NumberValue();
}

v8::Handle<v8::Value> ScriptCamObject::GetNear(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->near));
}

void ScriptCamObject::SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    objectPtr->near = value->NumberValue();
}

v8::Handle<v8::Value> ScriptCamObject::GetFar(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->far));
}

void ScriptCamObject::SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    objectPtr->far = value->NumberValue();
}

v8::Handle<v8::Value> ScriptCamObject::GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->width));
}

void ScriptCamObject::SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    objectPtr->width = value->NumberValue();
}

v8::Handle<v8::Value> ScriptCamObject::GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->height));
}

void ScriptCamObject::SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(info.This());
    objectPtr->height = value->NumberValue();
}

v8::Handle<v8::Value> ScriptCamObject::GetViewRay(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return v8::ThrowException(v8::String::New("getViewRay(): To less arguments"));
    if(!args[0]->IsNumber() || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("getViewRay(): Invalid argument"));
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    Ray3 ray = objectPtr->getRayAt(args[0]->NumberValue(), args[1]->NumberValue());
    
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("origin"), scriptVector3.newInstance(ray.origin));
    result->Set(v8::String::New("direction"), scriptVector3.newInstance(ray.direction));
    return handleScope.Close(result);
}

v8::Handle<v8::Value> ScriptCamObject::SetMainCam(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    mainCam = getDataOfInstance<CamObject>(args.This());
    return v8::Undefined();
}

v8::Handle<v8::Value> ScriptCamObject::GetMainCam(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> result = mainCam->scriptInstance;
    return handleScope.Close(result);
}

ScriptCamObject::ScriptCamObject(const char* name) :ScriptBaseObject(name) {
    
}

ScriptCamObject::ScriptCamObject() :ScriptCamObject("CamObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("fov"), GetFov, SetFov);
    objectTemplate->SetAccessor(v8::String::New("near"), GetNear, SetNear);
    objectTemplate->SetAccessor(v8::String::New("far"), GetFar, SetFar);
    objectTemplate->SetAccessor(v8::String::New("width"), GetWidth, SetWidth);
    objectTemplate->SetAccessor(v8::String::New("height"), GetHeight, SetHeight);
    objectTemplate->Set(v8::String::New("getViewRay"), v8::FunctionTemplate::New(GetViewRay));
    objectTemplate->Set(v8::String::New("setMainCam"), v8::FunctionTemplate::New(SetMainCam));
    
    functionTemplate->Set(v8::String::New("getMainCam"), v8::FunctionTemplate::New(GetMainCam));
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



ScriptCamObject scriptCamObject;