//
//  ScriptSimpleObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//
//

#include "ScriptManager.h"

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

ScriptCamObject::ScriptCamObject() :ScriptBaseObject("CamObject") {
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


v8::Handle<v8::Value> ScriptSoundObject::GetSoundTrack(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<SoundTrack>(objectPtr->soundTrack, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getResourcePath(filePackage, name).c_str()));
}

void ScriptSoundObject::SetSoundTrack(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    auto soundTrack = fileManager.initResource<SoundTrack>(scriptManager->stdStringOf(value->ToString()));
    if(soundTrack) objectPtr->setSoundTrack(soundTrack);
}

v8::Handle<v8::Value> ScriptSoundObject::GetTimeOffset(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->getTimeOffset()));
}

void ScriptSoundObject::SetTimeOffset(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    objectPtr->setTimeOffset(value->NumberValue());
}

v8::Handle<v8::Value> ScriptSoundObject::GetVolume(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->getVolume()));
}

void ScriptSoundObject::SetVolume(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    objectPtr->setVolume(value->NumberValue());
}

v8::Handle<v8::Value> ScriptSoundObject::GetPlaying(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    return handleScope.Close(v8::Boolean::New(objectPtr->getPlaying()));
}

void ScriptSoundObject::SetPlaying(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    objectPtr->setPlaying(value->BooleanValue());
}

v8::Handle<v8::Value> ScriptSoundObject::GetMode(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    switch(objectPtr->mode) {
        case SoundObject::Mode::Looping:
            return handleScope.Close(v8::String::New("looping"));
        case SoundObject::Mode::Hold:
            return handleScope.Close(v8::String::New("hold"));
        case SoundObject::Mode::Dispose:
            return handleScope.Close(v8::String::New("dispose"));
    }
}

void ScriptSoundObject::SetMode(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    SoundObject* objectPtr = getDataOfInstance<SoundObject>(info.This());
    const char* str = scriptManager->cStringOf(value->ToString());
    if(strcmp(str, "looping") == 0)
        objectPtr->mode = SoundObject::Mode::Looping;
    else if(strcmp(str, "hold") == 0)
        objectPtr->mode = SoundObject::Mode::Hold;
    else if(strcmp(str, "dispose") == 0)
        objectPtr->mode = SoundObject::Mode::Dispose;
}

ScriptSoundObject::ScriptSoundObject() :ScriptBaseObject("SoundObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("soundTrack"), GetSoundTrack, SetSoundTrack);
    objectTemplate->SetAccessor(v8::String::New("timeOffset"), GetTimeOffset, SetTimeOffset);
    objectTemplate->SetAccessor(v8::String::New("volume"), GetVolume, SetVolume);
    objectTemplate->SetAccessor(v8::String::New("playing"), GetPlaying, SetPlaying);
    objectTemplate->SetAccessor(v8::String::New("mode"), GetMode, SetMode);
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



ScriptCamObject scriptCamObject;
ScriptSoundObject scriptSoundObject;