//
//  ScriptGUIInput.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptGUIInput.h"

v8::Handle<v8::Value> ScriptGUISlider::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUISlider Constructor: Invalid argument"));
    
    GUISlider* objectPtr = new GUISlider();
    objectPtr->onChange = [](GUISlider* objectPtr, bool changing) {
        callFunction(objectPtr->scriptInstance, "onchange", { v8::Boolean::New(changing) });
    };
    return initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr);
}

v8::Handle<v8::Value> ScriptGUISlider::GetValue(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    return v8::Number::New(objectPtr->value);
}

void ScriptGUISlider::SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->NumberValue()) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    objectPtr->value = clamp(value->NumberValue(), 0.0, 1.0);
}

v8::Handle<v8::Value> ScriptGUISlider::GetSteps(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    return v8::Integer::New(objectPtr->steps);
}

void ScriptGUISlider::SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    objectPtr->steps = value->IntegerValue();
}

v8::Handle<v8::Value> ScriptGUISlider::GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUISlider* objectPtr = getDataOfInstance<GUISlider>(info.This());
    return v8::Boolean::New(objectPtr->enabled);
}

void ScriptGUISlider::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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



v8::Handle<v8::Value> ScriptGUITextField::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    
    if(args.Length() != 1 || !scriptGUIView.isCorrectInstance(args[0]))
        return v8::ThrowException(v8::String::New("GUITextField Constructor: Invalid argument"));
    
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
    return initInstance(args.This(), getDataOfInstance<GUIView>(args[0]), objectPtr);
}

v8::Handle<v8::Value> ScriptGUITextField::GetCursorX(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    return v8::Integer::New(objectPtr->getCursorX());
}

void ScriptGUITextField::SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    if(!value->IsInt32() || value->IntegerValue() < 0) return;
    objectPtr->setCursorX(value->IntegerValue());
}

v8::Handle<v8::Value> ScriptGUITextField::GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    return v8::Boolean::New(objectPtr->enabled);
}

void ScriptGUITextField::SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    objectPtr->enabled = value->BooleanValue();
}

v8::Handle<v8::Value> ScriptGUITextField::GetText(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    GUITextField* objectPtr = getDataOfInstance<GUITextField>(info.This());
    GUILabel* label = static_cast<GUILabel*>(objectPtr->children[0]);
    return v8::String::New(label->text.c_str());
}

void ScriptGUITextField::SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
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



ScriptGUISlider scriptGUISlider;
ScriptGUITextField scriptGUITextField;