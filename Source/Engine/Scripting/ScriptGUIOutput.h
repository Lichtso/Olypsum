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
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetTextAlignment(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetText(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetFontHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetFont(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptGUILabel(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUILabel();
};

class ScriptGUIProgressBar : public ScriptGUIRect {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetValue(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUIProgressBar(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIProgressBar();
};

class ScriptGUIImage : public ScriptGUIRect {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetImage(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetImage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    protected:
    ScriptGUIImage(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIImage();
};

extern ScriptGUILabel scriptGUILabel;
extern ScriptGUIProgressBar scriptGUIProgressBar;
extern ScriptGUIImage scriptGUIImage;

#endif