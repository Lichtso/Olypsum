//
//  ScriptGUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIButton_h
#define ScriptGUIButton_h

#include "ScriptGUIInput.h"

class ScriptGUIButton : public ScriptGUIFramedView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetPaddingX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetPaddingY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUIButton(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUIButton();
};

class ScriptGUICheckBox : public ScriptGUIButton {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptGUICheckBox(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIButton(name, constructor) { }
    public:
    ScriptGUICheckBox();
};

class ScriptGUITabs : public ScriptGUIView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSelected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetDeactivatable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUITabs(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUITabs();
};

extern ScriptGUIButton scriptGUIButton;
extern ScriptGUICheckBox scriptGUICheckBox;
extern ScriptGUITabs scriptGUITabs;

#endif