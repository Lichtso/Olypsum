//
//  ScriptParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptParticlesObject.h"

void ScriptParticlesObject::GetMaxParticles(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->maxParticles);
}

void ScriptParticlesObject::GetTexture(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(!filePackage) return;
    ScriptReturn(fileManager.getPathOfResource(filePackage, name).c_str());
}

void ScriptParticlesObject::SetTexture(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, stdStrOfV8(value));
    if(texture) objectPtr->texture = texture;
}

void ScriptParticlesObject::GetTransformAligned(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->transformAligned);
}

void ScriptParticlesObject::SetTransformAligned(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->transformAligned = value->BooleanValue();
}

void ScriptParticlesObject::GetSystemLife(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->systemLife);
}

void ScriptParticlesObject::SetSystemLife(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->systemLife = value->NumberValue();
}

void ScriptParticlesObject::GetLifeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->lifeMin);
}

void ScriptParticlesObject::SetLifeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->lifeMin = value->NumberValue();
}

void ScriptParticlesObject::GetLifeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->lifeMax);
}

void ScriptParticlesObject::SetLifeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->lifeMax = value->NumberValue();
}

void ScriptParticlesObject::GetSizeMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->sizeMin);
}

void ScriptParticlesObject::SetSizeMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->sizeMin = value->NumberValue();
}

void ScriptParticlesObject::GetSizeMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    ScriptReturn(objectPtr->sizeMax);
}

void ScriptParticlesObject::SetSizeMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    objectPtr->sizeMax = value->NumberValue();
}

void ScriptParticlesObject::AccessForce(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->force = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->force));
}

void ScriptParticlesObject::AccessPosMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->posMin = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->posMin));
}

void ScriptParticlesObject::AccessPosMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->posMax = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->posMax));
}

void ScriptParticlesObject::AccessDirMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->dirMin = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->dirMin));
}

void ScriptParticlesObject::AccessDirMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ParticlesObject* objectPtr = getDataOfInstance<ParticlesObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->dirMax = scriptVector3->getDataOfInstance(args[0]);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->dirMax));
}

ScriptParticlesObject::ScriptParticlesObject() :ScriptPhysicObject("ParticlesObject") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "maxParticles", GetMaxParticles, 0);
    ScriptAccessor(objectTemplate, "texture", GetTexture, SetTexture);
    ScriptAccessor(objectTemplate, "transformAligned", GetTransformAligned, SetTransformAligned);
    ScriptAccessor(objectTemplate, "systemLife", GetSystemLife, SetSystemLife);
    ScriptAccessor(objectTemplate, "lifeMin", GetLifeMin, SetLifeMin);
    ScriptAccessor(objectTemplate, "lifeMax", GetLifeMax, SetLifeMax);
    ScriptAccessor(objectTemplate, "sizeMin", GetSizeMin, SetSizeMin);
    ScriptAccessor(objectTemplate, "sizeMax", GetSizeMax, SetSizeMax);
    ScriptMethod(objectTemplate, "force", AccessForce);
    ScriptMethod(objectTemplate, "posMin", AccessPosMin);
    ScriptMethod(objectTemplate, "posMax", AccessPosMax);
    ScriptMethod(objectTemplate, "dirMin", AccessDirMin);
    ScriptMethod(objectTemplate, "dirMax", AccessDirMax);
    
    ScriptInherit(scriptPhysicObject);
}