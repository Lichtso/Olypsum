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
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetTextAlignment(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTextAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetText(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFontHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFontHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFont(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFont(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessColor(const v8::Arguments& args);
    protected:
    ScriptGUILabel(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUILabel();
};

class ScriptGUIProgressBar : public ScriptGUIRect {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetValue(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUIProgressBar(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIProgressBar();
};

class ScriptGUIImage : public ScriptGUIRect {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetImage(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetImage(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUIImage(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIImage();
};

extern ScriptGUILabel scriptGUILabel;
extern ScriptGUIProgressBar scriptGUIProgressBar;
extern ScriptGUIImage scriptGUIImage;

#endif