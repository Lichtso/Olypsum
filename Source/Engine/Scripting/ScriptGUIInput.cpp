//
//  ScriptGUIInput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIInput.h"

void ScriptGUISlider::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUISlider Constructor: Invalid argument");
    
    GUISlider* objectPtr = new GUISlider();
    objectPtr->onChange = [](GUISlider* objectPtr, bool changing) {
        callFunction(objectPtr->scriptInstance, "onchange", { v8::Boolean::New(changing) });
    };
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUISlider::GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    info.GetReturnValue().Set(objectPtr->value);
}

void ScriptGUISlider::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->NumberValue()) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    objectPtr->value = clamp(value->NumberValue(), 0.0, 1.0);
}

void ScriptGUISlider::GetSteps(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    info.GetReturnValue().Set(objectPtr->steps);
}

void ScriptGUISlider::SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    objectPtr->steps = value->IntegerValue();
}

void ScriptGUISlider::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    info.GetReturnValue().Set(objectPtr->enabled);
}

void ScriptGUISlider::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    objectPtr->enabled = value->BooleanValue();
}

ScriptGUISlider::ScriptGUISlider() :ScriptGUIRect("GUISlider", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("orientation"), GetOrientation<GUISlider>, SetOrientationDual<GUISlider>);
    objectTemplate->SetAccessor(v8::String::New("value"), GetValue, SetValue);
    objectTemplate->SetAccessor(v8::String::New("steps"), GetSteps, SetSteps);
    objectTemplate->SetAccessor(v8::String::New("enabled"), GetEnabled, SetEnabled);
    
    functionTemplate->Inherit(scriptGUIRect.functionTemplate);
}



void ScriptGUITextField::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return args.ScriptException("GUITextField Constructor: Invalid argument");
    
    GUITextField* objectPtr = new GUITextField();
    objectPtr->onChange = [](GUITextField* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onchange", { });
    };
    objectPtr->onFocus = [](GUITextField* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onfocus", { });
    };
    objectPtr->onBlur = [](GUITextField* objectPtr) {
        callFunction(objectPtr->scriptInstance, "onblur", { });
    };
    args.GetReturnValue().Set(initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr));
}

void ScriptGUITextField::GetCursorX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    info.GetReturnValue().Set(objectPtr->getCursorX());
}

void ScriptGUITextField::SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    objectPtr->setCursorX(value->IntegerValue());
}

void ScriptGUITextField::GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    info.GetReturnValue().Set(objectPtr->enabled);
}

void ScriptGUITextField::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    objectPtr->enabled = value->BooleanValue();
}

void ScriptGUITextField::GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    GUILabel* label = static_cast<GUILabel*>(objectPtr->children[0]);
    info.GetReturnValue().Set(v8::String::New(label->text.c_str()));
}

void ScriptGUITextField::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    GUILabel* label = static_cast<GUILabel*>(objectPtr->children[0]);
    label->text = stdStrOfV8(value);
}

ScriptGUITextField::ScriptGUITextField() :ScriptGUIFramedView("GUITextField", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("cursorX"), GetCursorX, SetCursorX);
    objectTemplate->SetAccessor(v8::String::New("enabled"), GetEnabled, SetEnabled);
    objectTemplate->SetAccessor(v8::String::New("text"), GetText, SetText);
    
    functionTemplate->Inherit(scriptGUIFramedView.functionTemplate);
}