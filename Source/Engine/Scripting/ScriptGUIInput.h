//
//  ScriptGUIInput.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIInput_h
#define ScriptGUIInput_h

#include "ScriptGUIOutput.h"

class ScriptGUISlider : public ScriptGUIRect {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetSteps(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUISlider(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUISlider();
};

class ScriptGUITextField : public ScriptGUIFramedView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetCursorX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUITextField(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUITextField();
};

extern ScriptGUISlider scriptGUISlider;
extern ScriptGUITextField scriptGUITextField;

#endif