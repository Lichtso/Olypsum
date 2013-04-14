//
//  ScriptParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//
//

#include "ScriptManager.h"

v8::Handle<v8::Value> ScriptParticlesObject::GetMaxParticles(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Integer::New(objectPtr->maxParticles));
}

v8::Handle<v8::Value> ScriptParticlesObject::GetTexture(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(!filePackage) return v8::Undefined();
    return handleScope.Close(v8::String::New(fileManager.getResourcePath(filePackage, name).c_str()));
}

void ScriptParticlesObject::SetTexture(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    auto texture = fileManager.initResource<Texture>(scriptManager->stdStringOf(value->ToString()));
    if(texture) objectPtr->texture = texture;
}

v8::Handle<v8::Value> ScriptParticlesObject::GetTransformAligned(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Boolean::New(objectPtr->transformAligned));
}

void ScriptParticlesObject::SetTransformAligned(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->transformAligned = value->BooleanValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetSystemLife(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->systemLife));
}

void ScriptParticlesObject::SetSystemLife(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->systemLife = value->NumberValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetLifeMin(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->lifeMin));
}

void ScriptParticlesObject::SetLifeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->lifeMin = value->NumberValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetLifeMax(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->lifeMax));
}

void ScriptParticlesObject::SetLifeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->lifeMax = value->NumberValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetSizeMin(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->sizeMin));
}

void ScriptParticlesObject::SetSizeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->sizeMin = value->NumberValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetSizeMax(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->sizeMax));
}

void ScriptParticlesObject::SetSizeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    objectPtr->sizeMax = value->NumberValue();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetForce(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->force));
}

v8::Handle<v8::Value> ScriptParticlesObject::SetForce(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("ParticlesObject setForce: Invalid argument"));
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->force = scriptVector3.getDataOfInstance(args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetPosMin(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->posMin));
}

v8::Handle<v8::Value> ScriptParticlesObject::SetPosMin(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("ParticlesObject setPosMin: Invalid argument"));
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->posMin = scriptVector3.getDataOfInstance(args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetPosMax(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->posMax));
}

v8::Handle<v8::Value> ScriptParticlesObject::SetPosMax(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("ParticlesObject setPosMax: Invalid argument"));
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->posMax = scriptVector3.getDataOfInstance(args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetDirMin(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->dirMin));
}

v8::Handle<v8::Value> ScriptParticlesObject::SetDirMin(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("ParticlesObject setDirMin: Invalid argument"));
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->dirMin = scriptVector3.getDataOfInstance(args[0]);
    return args.This();
}

v8::Handle<v8::Value> ScriptParticlesObject::GetDirMax(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->dirMax));
}

v8::Handle<v8::Value> ScriptParticlesObject::SetDirMax(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("ParticlesObject setDirMax: Invalid argument"));
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->dirMax = scriptVector3.getDataOfInstance(args[0]);
    return args.This();
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
    objectTemplate->SetAccessor(v8::String::New("force"), GetForce);
    objectTemplate->Set(v8::String::New("setForce"), v8::FunctionTemplate::New(SetForce));
    objectTemplate->SetAccessor(v8::String::New("posMin"), GetPosMin);
    objectTemplate->Set(v8::String::New("setPosMin"), v8::FunctionTemplate::New(SetPosMin));
    objectTemplate->SetAccessor(v8::String::New("posMax"), GetPosMax);
    objectTemplate->Set(v8::String::New("setPosMax"), v8::FunctionTemplate::New(SetPosMax));
    objectTemplate->SetAccessor(v8::String::New("dirMin"), GetDirMin);
    objectTemplate->Set(v8::String::New("setDirMin"), v8::FunctionTemplate::New(SetDirMin));
    objectTemplate->SetAccessor(v8::String::New("dirMax"), GetDirMax);
    objectTemplate->Set(v8::String::New("setDirMax"), v8::FunctionTemplate::New(SetDirMax));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}

ScriptParticlesObject scriptParticlesObject;