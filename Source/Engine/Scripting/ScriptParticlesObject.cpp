//
//  ScriptParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptParticlesObject.h"

void ScriptParticlesObject::GetMaxParticles(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->maxParticles);
}

void ScriptParticlesObject::GetTexture(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(!filePackage) return;
    args.GetReturnValue().Set(v8::String::New(fileManager.getPathOfResource(filePackage, name).c_str()));
}

void ScriptParticlesObject::SetTexture(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(value));
    if(texture) objectPtr->texture = texture;
}

void ScriptParticlesObject::GetTransformAligned(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->transformAligned);
}

void ScriptParticlesObject::SetTransformAligned(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->transformAligned = value->BooleanValue();
}

void ScriptParticlesObject::GetSystemLife(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->systemLife);
}

void ScriptParticlesObject::SetSystemLife(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->systemLife = value->NumberValue();
}

void ScriptParticlesObject::GetLifeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->lifeMin);
}

void ScriptParticlesObject::SetLifeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->lifeMin = value->NumberValue();
}

void ScriptParticlesObject::GetLifeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->lifeMax);
}

void ScriptParticlesObject::SetLifeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->lifeMax = value->NumberValue();
}

void ScriptParticlesObject::GetSizeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->sizeMin);
}

void ScriptParticlesObject::SetSizeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->sizeMin = value->NumberValue();
}

void ScriptParticlesObject::GetSizeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    args.GetReturnValue().Set(objectPtr->sizeMax);
}

void ScriptParticlesObject::SetSizeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->sizeMax = value->NumberValue();
}

void ScriptParticlesObject::AccessForce(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->force = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->force));
}

void ScriptParticlesObject::AccessPosMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->posMin = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->posMin));
}

void ScriptParticlesObject::AccessPosMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->posMax = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->posMax));
}

void ScriptParticlesObject::AccessDirMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->dirMin = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->dirMin));
}

void ScriptParticlesObject::AccessDirMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->dirMax = scriptVector3.getDataOfInstance(args[0]);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->dirMax));
}

ScriptParticlesObject::ScriptParticlesObject() :ScriptPhysicObject("ParticlesObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("maxParticles"), GetMaxParticles);
    objectTemplate->SetAccessor(v8::String::New("texture"), GetTexture, SetTexture);
    objectTemplate->SetAccessor(v8::String::New("transformAligned"), GetTransformAligned, SetTransformAligned);
    objectTemplate->SetAccessor(v8::String::New("systemLife"), GetSystemLife, SetSystemLife);
    objectTemplate->SetAccessor(v8::String::New("lifeMin"), GetLifeMin, SetLifeMin);
    objectTemplate->SetAccessor(v8::String::New("lifeMax"), GetLifeMax, SetLifeMax);
    objectTemplate->SetAccessor(v8::String::New("sizeMin"), GetSizeMin, SetSizeMin);
    objectTemplate->SetAccessor(v8::String::New("sizeMax"), GetSizeMax, SetSizeMax);
    objectTemplate->Set(v8::String::New("force"), v8::FunctionTemplate::New(AccessForce));
    objectTemplate->Set(v8::String::New("posMin"), v8::FunctionTemplate::New(AccessPosMin));
    objectTemplate->Set(v8::String::New("posMax"), v8::FunctionTemplate::New(AccessPosMax));
    objectTemplate->Set(v8::String::New("dirMin"), v8::FunctionTemplate::New(AccessDirMin));
    objectTemplate->Set(v8::String::New("dirMax"), v8::FunctionTemplate::New(AccessDirMax));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}