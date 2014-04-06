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
    ScriptDeclareMethod(Constructor);
    static void GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetSteps(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUISlider(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUISlider();
};

class ScriptGUITextField : public ScriptGUIFramedView {
    ScriptDeclareMethod(Constructor);
    static void GetCursorX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUITextField(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUITextField();
};

extern std::unique_ptr<ScriptGUISlider> scriptGUISlider;
extern std::unique_ptr<ScriptGUITextField> scriptGUITextField;

#endif