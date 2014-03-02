//
//  ScriptLightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptLightObject.h"

void ScriptLightObject::GetRange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    LightObject* objectPtr = getDataOfInstance<LightObject>(args.This());
    ScriptReturn(objectPtr->getRange());
}

void ScriptLightObject::AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    LightObject* objectPtr = getDataOfInstance<LightObject>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->color.getVector()));
}

ScriptLightObject::ScriptLightObject() :ScriptPhysicObject("LightObject") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("collisionShape"),
                                static_cast<v8::AccessorGetterCallback>(NULL), static_cast<v8::AccessorSetterCallback>(NULL));
    objectTemplate->SetAccessor(ScriptString("range"), GetRange);
    objectTemplate->Set(ScriptString("color"), ScriptMethod(AccessColor));
    
    ScriptInherit(scriptPhysicObject);
}



void ScriptDirectionalLight::AccessBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    DirectionalLight* objectPtr = getDataOfInstance<DirectionalLight>(args.This());
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        objectPtr->setBounds(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(objectPtr->getBounds()));
}

ScriptDirectionalLight::ScriptDirectionalLight() :ScriptLightObject("DirectionalLight") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->Set(ScriptString("bounds"), ScriptMethod(AccessBounds));
    
    ScriptInherit(scriptLightObject);
}



void ScriptSpotLight::GetCutoff(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(args.This());
    ScriptReturn(objectPtr->getCutoff());
}

void ScriptSpotLight::SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsNumber())
        return ScriptException("SpotLight setBounds: Invalid argument");
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(args.This());
    objectPtr->setBounds(args[0]->NumberValue(), args[1]->NumberValue());
    ScriptReturn(args.This());
}

ScriptSpotLight::ScriptSpotLight() :ScriptLightObject("SpotLight") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("cutoff"), GetCutoff);
    objectTemplate->Set(ScriptString("setBounds"), ScriptMethod(SetBounds));
    
    ScriptInherit(scriptLightObject);
}



void ScriptPositionalLight::GetOmniDirectional(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(args.This());
    ScriptReturn(objectPtr->getOmniDirectional());
}

void ScriptPositionalLight::SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 2 || !args[0]->IsBoolean() || !args[1]->IsNumber())
        return ScriptException("PositionalLight setBounds: Invalid argument");
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(args.This());
    objectPtr->setBounds(args[0]->BooleanValue(), args[1]->NumberValue());
    ScriptReturn(args.This());
}

ScriptPositionalLight::ScriptPositionalLight() :ScriptLightObject("PositionalLight") {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    objectTemplate->SetAccessor(ScriptString("omniDirectional"), GetOmniDirectional);
    objectTemplate->Set(ScriptString("setBounds"), ScriptMethod(SetBounds));
    
    ScriptInherit(scriptLightObject);
}