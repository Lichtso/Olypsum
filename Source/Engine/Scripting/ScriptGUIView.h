//
//  ScriptGUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//
//

#ifndef ScriptGUIView_h
#define ScriptGUIView_h

#include "ScriptGUIRect.h"

class ScriptGUIView : public ScriptGUIRect {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetChildCount(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetChild(uint32_t index, const v8::AccessorInfo& info);
    protected:
    ScriptGUIView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIView();
};

class ScriptGUIFramedView : public ScriptGUIView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessTopColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessBottomColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessBorderColor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetInnerShadow(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetCornerRadius(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUIFramedView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIFramedView();
};

class ScriptGUIScreenView : public ScriptGUIView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetModalView(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFocused(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    ScriptGUIScreenView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIScreenView();
};

class ScriptGUIScrollView : public ScriptGUIFramedView {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetSliderX(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSliderX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSliderY(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSliderY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetScrollX(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetScrollX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetScrollY(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetScrollY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetContentWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetContentWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetContentHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetContentHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    ScriptGUIScrollView(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUIScrollView();
};

extern ScriptGUIView scriptGUIView;
extern ScriptGUIFramedView scriptGUIFramedView;
extern ScriptGUIScreenView scriptGUIScreenView;
extern ScriptGUIScrollView scriptGUIScrollView;

#endif