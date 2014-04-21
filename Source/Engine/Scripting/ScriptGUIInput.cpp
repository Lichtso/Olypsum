//
//  ScriptGUIInput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"
/*
void ScriptGUISlider::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUISlider Constructor: Invalid argument");
    
    GUISlider* objectPtr = new GUISlider();
    objectPtr->onChange = [](GUISlider* objectPtr, bool changing) {
        v8::Handle<v8::Value> argv[] = { v8::Boolean::New(v8::Isolate::GetCurrent(), changing) };
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onchange", 1, argv);
    };
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUISlider::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    ScriptReturn(objectPtr->value);
}

void ScriptGUISlider::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->NumberValue()) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    objectPtr->value = clamp(value->NumberValue(), 0.0, 1.0);
}

void ScriptGUISlider::GetSteps(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    ScriptReturn(objectPtr->steps);
}

void ScriptGUISlider::SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    objectPtr->steps = value->IntegerValue();
}

void ScriptGUISlider::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    ScriptReturn(objectPtr->enabled);
}

void ScriptGUISlider::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(args.This());
    objectPtr->enabled = value->BooleanValue();
}

ScriptGUISlider::ScriptGUISlider() :ScriptGUIRect("GUISlider", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "orientation", GetOrientation<GUISlider>, SetOrientationDual<GUISlider>);
    ScriptAccessor(objectTemplate, "value", GetValue, SetValue);
    ScriptAccessor(objectTemplate, "steps", GetSteps, SetSteps);
    ScriptAccessor(objectTemplate, "enabled", GetEnabled, SetEnabled);
    
    ScriptInherit(scriptGUIRect);
}



void ScriptGUITextField::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() != 1 || !scriptGUIView->isCorrectInstance(args[0]))
        return ScriptException("GUITextField Constructor: Invalid argument");
    
    GUITextField* objectPtr = new GUITextField();
    objectPtr->onChange = [](GUITextField* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onchange", 0, NULL);
    };
    objectPtr->onFocus = [](GUITextField* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onfocus", 0, NULL);
    };
    objectPtr->onBlur = [](GUITextField* objectPtr) {
        callFunction(v8::Handle<v8::Object>(*objectPtr->scriptInstance), "onblur", 0, NULL);
    };
    ScriptReturn(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUITextField::GetCursorX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    ScriptReturn(objectPtr->getCursorX());
}

void ScriptGUITextField::SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    objectPtr->setCursorX(value->IntegerValue());
}

void ScriptGUITextField::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    ScriptReturn(objectPtr->enabled);
}

void ScriptGUITextField::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    objectPtr->enabled = value->BooleanValue();
}

void ScriptGUITextField::GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    GUILabel* label = static_cast<GUILabel*>(objectPtr->children[0]);
    ScriptReturn(label->text.c_str());
}

void ScriptGUITextField::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(args.This());
    GUILabel* label = static_cast<GUILabel*>(objectPtr->children[0]);
    label->text = stdStrOfV8(value);
}

ScriptGUITextField::ScriptGUITextField() :ScriptGUIFramedView("GUITextField", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "cursorX", GetCursorX, SetCursorX);
    ScriptAccessor(objectTemplate, "enabled", GetEnabled, SetEnabled);
    ScriptAccessor(objectTemplate, "text", GetText, SetText);
    
    ScriptInherit(scriptGUIFramedView);
}*/