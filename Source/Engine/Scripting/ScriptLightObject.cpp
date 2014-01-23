//
//  ScriptLightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptLightObject.h"

void ScriptLightObject::GetRange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    LightObject* objectPtr = getDataOfInstance<LightObject>(args.This());
    args.GetReturnValue().Set(objectPtr->getRange());
}

void ScriptLightObject::AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    LightObject* objectPtr = getDataOfInstance<LightObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptVector3.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(handleScope.Close(scriptVector3.newInstance(objectPtr->color.getVector())));
}

ScriptLightObject::ScriptLightObject() :ScriptPhysicObject("LightObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"),
                                static_cast<v8::AccessorGetterCallback>(NULL), static_cast<v8::AccessorSetterCallback>(NULL));
    objectTemplate->SetAccessor(v8::String::New("range"), GetRange);
    objectTemplate->Set(v8::String::New("color"), v8::FunctionTemplate::New(AccessColor));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



void ScriptDirectionalLight::GetBounds(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    DirectionalLight* objectPtr = getDataOfInstance<DirectionalLight>(args.This());
    args.GetReturnValue().Set(scriptVector3.newInstance(objectPtr->getBounds()));
}

void ScriptDirectionalLight::SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return args.ScriptException("DirectionalLight setBounds: Invalid argument");
    DirectionalLight* objectPtr = getDataOfInstance<DirectionalLight>(args.This());
    objectPtr->setBounds(scriptVector3.getDataOfInstance(args[0]));
    args.GetReturnValue().Set(args.This());
}

ScriptDirectionalLight::ScriptDirectionalLight() :ScriptLightObject("DirectionalLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("bounds"), GetBounds);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}



void ScriptSpotLight::GetCutoff(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(args.This());
    args.GetReturnValue().Set(objectPtr->getCutoff());
}

void ScriptSpotLight::SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsNumber())
        return args.ScriptException("SpotLight setBounds: Invalid argument");
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(args.This());
    objectPtr->setBounds(args[0]->NumberValue(), args[1]->NumberValue());
    args.GetReturnValue().Set(args.This());
}

ScriptSpotLight::ScriptSpotLight() :ScriptLightObject("SpotLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("cutoff"), GetCutoff);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}



void ScriptPositionalLight::GetOmniDirectional(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(args.This());
    args.GetReturnValue().Set(objectPtr->getOmniDirectional());
}

void ScriptPositionalLight::SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2 || !args[0]->IsBoolean() || !args[1]->IsNumber())
        return args.ScriptException("PositionalLight setBounds: Invalid argument");
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(args.This());
    objectPtr->setBounds(args[0]->BooleanValue(), args[1]->NumberValue());
    args.GetReturnValue().Set(args.This());
}

ScriptPositionalLight::ScriptPositionalLight() :ScriptLightObject("PositionalLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("omniDirectional"), GetOmniDirectional);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}