//
//  ScriptGUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//
//

#include "ScriptGUIRect.h"

#ifndef ScriptGUIView_h
#define ScriptGUIView_h

class ScriptGUIView : public ScriptGUIRect {
    static v8::Handle<v8::Value> GetChildCount(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetChild(uint32_t index, const v8::AccessorInfo& info);
    protected:
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    ScriptGUIView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIView();
};

class ScriptGUIFramedView : public ScriptGUIView {
    static v8::Handle<v8::Value> AccessTopColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessBottomColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessBorderColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetInnerShadow(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetCornerRadius(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    ScriptGUIFramedView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIFramedView();
};

class ScriptGUIScreenView : public ScriptGUIView {
    static v8::Handle<v8::Value> GetModalView(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFocused(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    ScriptGUIScreenView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIScreenView();
};

extern ScriptGUIView scriptGUIView;
extern ScriptGUIFramedView scriptGUIFramedView;
extern ScriptGUIScreenView scriptGUIScreenView;

#endif