//
//  ScriptGUIInput.h
//  Olypsum
//
//  Created by Alexander Meißner on 24.04.13.
//
//

#ifndef ScriptGUIInput_h
#define ScriptGUIInput_h

#include "ScriptGUIOutput.h"

class ScriptGUISlider : public ScriptGUIRect {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetValue(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetValue(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSteps(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSteps(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUISlider(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUISlider();
};

class ScriptGUITextField : public ScriptGUIFramedView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetCursorX(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCursorX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetText(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetText(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUITextField(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUITextField();
};

extern ScriptGUISlider scriptGUISlider;
extern ScriptGUITextField scriptGUITextField;

#endif