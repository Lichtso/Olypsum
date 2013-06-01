//
//  ScriptLightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//
//

#include "ScriptLightObject.h"

v8::Handle<v8::Value> ScriptLightObject::GetRange(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    LightObject* objectPtr = getDataOfInstance<LightObject>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->getRange()));
}

v8::Handle<v8::Value> ScriptLightObject::AccessColor(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    LightObject* objectPtr = getDataOfInstance<LightObject>(args.This());
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        objectPtr->color = Color4(scriptVector3.getDataOfInstance(args[0]));
        return args[0];
    }else
        return handleScope.Close(scriptVector3.newInstance(objectPtr->color.getVector()));
}

ScriptLightObject::ScriptLightObject() :ScriptPhysicObject("LightObject") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("collisionShape"),
                                static_cast<v8::AccessorGetter>(NULL), static_cast<v8::AccessorSetter>(NULL));
    objectTemplate->SetAccessor(v8::String::New("range"), GetRange);
    objectTemplate->Set(v8::String::New("color"), v8::FunctionTemplate::New(AccessColor));
    
    functionTemplate->Inherit(scriptPhysicObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptDirectionalLight::GetBounds(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    DirectionalLight* objectPtr = getDataOfInstance<DirectionalLight>(info.This());
    return handleScope.Close(scriptVector3.newInstance(objectPtr->getBounds()));
}

v8::Handle<v8::Value> ScriptDirectionalLight::SetBounds(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 1 || scriptVector3.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("DirectionalLight setBounds: Invalid argument"));
    DirectionalLight* objectPtr = getDataOfInstance<DirectionalLight>(args.This());
    objectPtr->setBounds(scriptVector3.getDataOfInstance(args[0]));
    return args.This();
}

ScriptDirectionalLight::ScriptDirectionalLight() :ScriptPhysicObject("DirectionalLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("bounds"), GetBounds);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptSpotLight::GetCutoff(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->getCutoff()));
}

v8::Handle<v8::Value> ScriptSpotLight::SetBounds(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2 || !args[0]->IsNumber() || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("SpotLight setBounds: Invalid argument"));
    SpotLight* objectPtr = getDataOfInstance<SpotLight>(args.This());
    objectPtr->setBounds(args[0]->NumberValue(), args[1]->NumberValue());
    return args.This();
}

ScriptSpotLight::ScriptSpotLight() :ScriptPhysicObject("SpotLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("cutoff"), GetCutoff);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}



v8::Handle<v8::Value> ScriptPositionalLight::GetOmniDirectional(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(info.This());
    return handleScope.Close(v8::Number::New(objectPtr->getOmniDirectional()));
}

v8::Handle<v8::Value> ScriptPositionalLight::SetBounds(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 2 || !args[0]->IsBoolean() || !args[1]->IsNumber())
        return v8::ThrowException(v8::String::New("PositionalLight setBounds: Invalid argument"));
    PositionalLight* objectPtr = getDataOfInstance<PositionalLight>(args.This());
    objectPtr->setBounds(args[0]->BooleanValue(), args[1]->NumberValue());
    return args.This();
}

ScriptPositionalLight::ScriptPositionalLight() :ScriptPhysicObject("PositionalLight") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("omniDirectional"), GetOmniDirectional);
    objectTemplate->Set(v8::String::New("setBounds"), v8::FunctionTemplate::New(SetBounds));
    
    functionTemplate->Inherit(scriptLightObject.functionTemplate);
}



ScriptLightObject scriptLightObject;
ScriptDirectionalLight scriptDirectionalLight;
ScriptSpotLight scriptSpotLight;
ScriptPositionalLight scriptPositionalLight;