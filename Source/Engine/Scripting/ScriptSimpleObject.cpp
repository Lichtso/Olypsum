//
//  ScriptSimpleObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

ScriptSimpleObject::ScriptSimpleObject() :ScriptBaseObject("SimpleObject") {
    v8::HandleScope handleScope;
    
    //v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    functionTemplate->Inherit(scriptBaseClass.functionTemplate);
}



void ScriptCamObject::GetFov(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    args.GetReturnValue().Set(objectPtr->fov);
}

void ScriptCamObject::SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->fov = value->NumberValue();
}

void ScriptCamObject::GetNear(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    args.GetReturnValue().Set(objectPtr->near);
}

void ScriptCamObject::SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->near = value->NumberValue();
}

void ScriptCamObject::GetFar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    args.GetReturnValue().Set(objectPtr->far);
}

void ScriptCamObject::SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->far = value->NumberValue();
}

void ScriptCamObject::GetViewRay(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2)
        return args.ScriptException("CamObject getViewRay(): Too few arguments");
    if(!args[0]->IsNumber() || !args[1]->IsNumber())
        return args.ScriptException("CamObject getViewRay(): Invalid argument");
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    Ray3 ray = objectPtr->getRayAt(args[0]->NumberValue(), args[1]->NumberValue());
    
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("origin"), scriptVector3.newInstance(ray.origin));
    result->Set(v8::String::New("direction"), scriptVector3.newInstance(ray.direction));
    args.GetReturnValue().Set(result);
}

void ScriptCamObject::SetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    mainCam = getDataOfInstance<CamObject>(args.This());
}

void ScriptCamObject::GetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    v8::Handle<v8::Object> result = mainCam->scriptInstance;
    args.GetReturnValue().Set(result);
}

ScriptCamObject::ScriptCamObject() :ScriptSimpleObject("CamObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("fov"), GetFov, SetFov);
    objectTemplate->SetAccessor(v8::String::New("near"), GetNear, SetNear);
    objectTemplate->SetAccessor(v8::String::New("far"), GetFar, SetFar);
    objectTemplate->Set(v8::String::New("getViewRay"), v8::FunctionTemplate::New(GetViewRay));
    objectTemplate->Set(v8::String::New("setMainCam"), v8::FunctionTemplate::New(SetMainCam));
    
    functionTemplate->Set(v8::String::New("getMainCam"), v8::FunctionTemplate::New(GetMainCam));
    functionTemplate->Inherit(scriptBaseObject.functionTemplate);
}


void ScriptSoundObject::GetSoundTrack(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<SoundTrack>(objectPtr->soundTrack, name);
    if(filePackage)
        args.GetReturnValue().Set(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptSoundObject::SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    auto soundTrack = fileManager.getResourceByPath<SoundTrack>(levelManager.levelPackage, stdStrOfV8(value));
    if(soundTrack) objectPtr->setSoundTrack(soundTrack);
}

void ScriptSoundObject::GetTimeOffset(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    args.GetReturnValue().Set(objectPtr->getTimeOffset());
}

void ScriptSoundObject::SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setTimeOffset(value->NumberValue());
}

void ScriptSoundObject::GetVolume(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    args.GetReturnValue().Set(objectPtr->getVolume());
}

void ScriptSoundObject::SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setVolume(value->NumberValue());
}

void ScriptSoundObject::GetPlaying(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    args.GetReturnValue().Set(objectPtr->getPlaying());
}

void ScriptSoundObject::SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setPlaying(value->BooleanValue());
}

void ScriptSoundObject::GetMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    switch(objectPtr->mode) {
        case SoundObject::Mode::Looping:
            args.GetReturnValue().Set(v8::String::New("looping"));
        case SoundObject::Mode::Hold:
            args.GetReturnValue().Set(v8::String::New("hold"));
        case SoundObject::Mode::Dispose:
            args.GetReturnValue().Set(v8::String::New("dispose"));
    }
}

void ScriptSoundObject::SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    const char* str = cStrOfV8(value);
    if(strcmp(str, "looping") == 0)
        objectPtr->mode = SoundObject::Mode::Looping;
    else if(strcmp(str, "hold") == 0)
        objectPtr->mode = SoundObject::Mode::Hold;
    else if(strcmp(str, "dispose") == 0)
        objectPtr->mode = SoundObject::Mode::Dispose;
}

ScriptSoundObject::ScriptSoundObject() :ScriptSimpleObject("SoundObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("soundTrack"), GetSoundTrack, SetSoundTrack);
    objectTemplate->SetAccessor(v8::String::New("timeOffset"), GetTimeOffset, SetTimeOffset);
    objectTemplate->SetAccessor(v8::String::New("volume"), GetVolume, SetVolume);
    objectTemplate->SetAccessor(v8::String::New("playing"), GetPlaying, SetPlaying);
    objectTemplate->SetAccessor(v8::String::New("mode"), GetMode, SetMode);
    
    functionTemplate->Inherit(scriptBaseObject.functionTemplate);
}