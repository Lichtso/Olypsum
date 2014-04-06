//
//  ScriptGUIOutput.h
//  Olypsum
//
//  Created by Alexander Meißner on 21.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIOutput_h
#define ScriptGUIOutput_h

#include "ScriptGUIView.h"

class ScriptGUILabel : public ScriptGUIRect {
    ScriptDeclareMethod(Constructor);
    static void GetTextAlignment(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetFontHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetFont(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(AccessColor);
    protected:
    ScriptGUILabel(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUILabel();
};

class ScriptGUIProgressBar : public ScriptGUIRect {
    ScriptDeclareMethod(Constructor);
    static void GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUIProgressBar(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIProgressBar();
};

class ScriptGUIImage : public ScriptGUIRect {
    ScriptDeclareMethod(Constructor);
    static void GetImage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetImage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUIImage(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIImage();
};

extern std::unique_ptr<ScriptGUILabel> scriptGUILabel;
extern std::unique_ptr<ScriptGUIProgressBar> scriptGUIProgressBar;
extern std::unique_ptr<ScriptGUIImage> scriptGUIImage;

#endif