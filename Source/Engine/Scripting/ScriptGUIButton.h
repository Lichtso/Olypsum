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
    ScriptDeclareMethod(Constructor);
    static void GetPaddingX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetPaddingY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetState(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetType(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUIButton(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUIButton();
};

class ScriptGUICheckBox : public ScriptGUIButton {
    ScriptDeclareMethod(Constructor);
    protected:
    ScriptGUICheckBox(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIButton(name, constructor) { }
    public:
    ScriptGUICheckBox();
};

class ScriptGUITabs : public ScriptGUIView {
    ScriptDeclareMethod(Constructor);
    static void GetSelected(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetDeactivatable(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUITabs(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUITabs();
};

extern std::unique_ptr<ScriptGUIButton> scriptGUIButton;
extern std::unique_ptr<ScriptGUICheckBox> scriptGUICheckBox;
extern std::unique_ptr<ScriptGUITabs> scriptGUITabs;

#endif