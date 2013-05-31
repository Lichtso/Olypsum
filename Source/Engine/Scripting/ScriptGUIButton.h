//
//  ScriptGUIButton.h
//  Olypsum
//
//  Created by Alexander Meißner on 26.04.13.
//
//

#ifndef ScriptGUIButton_h
#define ScriptGUIButton_h

#include "ScriptGUIInput.h"

class ScriptGUIButton : public ScriptGUIFramedView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetPaddingX(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPaddingX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetPaddingY(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPaddingY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetState(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetState(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetType(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetType(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUIButton(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUIButton();
};

class ScriptGUICheckBox : public ScriptGUIButton {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    protected:
    ScriptGUICheckBox(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIButton(name, constructor) { }
    public:
    ScriptGUICheckBox();
};

class ScriptGUITabs : public ScriptGUIView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetSelected(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSelected(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetDeactivatable(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetDeactivatable(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUITabs(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUITabs();
};

extern ScriptGUIButton scriptGUIButton;
extern ScriptGUICheckBox scriptGUICheckBox;
extern ScriptGUITabs scriptGUITabs;

#endif