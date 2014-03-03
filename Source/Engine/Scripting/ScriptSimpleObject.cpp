//
//  ScriptSimpleObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

ScriptSimpleObject::ScriptSimpleObject() :ScriptBaseObject("SimpleObject") {
    ScriptScope();
    
    ScriptInherit(scriptBaseClass);
}



void ScriptCamObject::GetFov(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    ScriptReturn(objectPtr->fov);
}

void ScriptCamObject::SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->fov = value->NumberValue();
}

void ScriptCamObject::GetNear(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    ScriptReturn(objectPtr->nearPlane);
}

void ScriptCamObject::SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->nearPlane = value->NumberValue();
}

void ScriptCamObject::GetFar(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    ScriptReturn(objectPtr->farPlane);
}

void ScriptCamObject::SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    objectPtr->farPlane = value->NumberValue();
}

void ScriptCamObject::GetViewRay(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2)
        return ScriptException("CamObject getViewRay(): Too few arguments");
    if(!args[0]->IsNumber() || !args[1]->IsNumber())
        return ScriptException("CamObject getViewRay(): Invalid argument");
    CamObject* objectPtr = getDataOfInstance<CamObject>(args.This());
    Ray3 ray = objectPtr->getRayAt(args[0]->NumberValue(), args[1]->NumberValue());
    
    v8::Handle<v8::Object> result = v8::Object::New(v8::Isolate::GetCurrent());
    result->Set(ScriptString("origin"), scriptVector3->newInstance(ray.origin));
    result->Set(ScriptString("direction"), scriptVector3->newInstance(ray.direction));
    ScriptReturn(result);
}

void ScriptCamObject::SetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    mainCam = getDataOfInstance<CamObject>(args.This());
}

void ScriptCamObject::GetMainCam(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    v8::Handle<v8::Object> result(*mainCam->scriptInstance);
    ScriptReturn(result);
}

ScriptCamObject::ScriptCamObject() :ScriptSimpleObject("CamObject") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("fov"), GetFov, SetFov);
    objectTemplate->SetAccessor(ScriptString("near"), GetNear, SetNear);
    objectTemplate->SetAccessor(ScriptString("far"), GetFar, SetFar);
    objectTemplate->Set(ScriptString("getViewRay"), ScriptMethod(GetViewRay));
    objectTemplate->Set(ScriptString("setMainCam"), ScriptMethod(SetMainCam));
    
    (*functionTemplate)->Set(ScriptString("getMainCam"), ScriptMethod(GetMainCam));
    ScriptInherit(scriptBaseObject);
}


void ScriptSoundObject::GetSoundTrack(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<SoundTrack>(objectPtr->soundTrack, name);
    if(filePackage)
        ScriptReturn(ScriptString(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptSoundObject::SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    auto soundTrack = fileManager.getResourceByPath<SoundTrack>(levelManager.levelPackage, stdStrOfV8(value));
    if(soundTrack) objectPtr->setSoundTrack(soundTrack);
}

void ScriptSoundObject::GetTimeOffset(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    ScriptReturn(objectPtr->getTimeOffset());
}

void ScriptSoundObject::SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setTimeOffset(value->NumberValue());
}

void ScriptSoundObject::GetVolume(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    ScriptReturn(objectPtr->getVolume());
}

void ScriptSoundObject::SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setVolume(value->NumberValue());
}

void ScriptSoundObject::GetPlaying(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    ScriptReturn(objectPtr->getPlaying());
}

void ScriptSoundObject::SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    objectPtr->setPlaying(value->BooleanValue());
}

void ScriptSoundObject::GetMode(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(args.This());
    switch(objectPtr->mode) {
        case SoundObject::Mode::Looping:
            ScriptReturn(ScriptString("looping"));
        case SoundObject::Mode::Hold:
            ScriptReturn(ScriptString("hold"));
        case SoundObject::Mode::Dispose:
            ScriptReturn(ScriptString("dispose"));
    }
}

void ScriptSoundObject::SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
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
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("soundTrack"), GetSoundTrack, SetSoundTrack);
    objectTemplate->SetAccessor(ScriptString("timeOffset"), GetTimeOffset, SetTimeOffset);
    objectTemplate->SetAccessor(ScriptString("volume"), GetVolume, SetVolume);
    objectTemplate->SetAccessor(ScriptString("playing"), GetPlaying, SetPlaying);
    objectTemplate->SetAccessor(ScriptString("mode"), GetMode, SetMode);
    
    ScriptInherit(scriptBaseObject);
}