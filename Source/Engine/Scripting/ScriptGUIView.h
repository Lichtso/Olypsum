//
//  ScriptGUIView.h
//  Olypsum
//
//  Created by Alexander Meißner on 18.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIView_h
#define ScriptGUIView_h

#include "ScriptGUIRect.h"

class ScriptGUIView : public ScriptGUIRect {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetChildCount(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetChild(uint32_t index, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void Adopt(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptGUIView(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIRect(name, constructor) { }
    public:
    ScriptGUIView();
};

class ScriptGUIFramedView : public ScriptGUIView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessTopColor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessBottomColor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessBorderColor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetInnerShadow(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetInnerShadow(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetCornerRadius(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetCornerRadius(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUIFramedView(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIFramedView();
};

class ScriptGUIScreenView : public ScriptGUIView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetModalView(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetModalView(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetFocused(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    protected:
    ScriptGUIScreenView(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIView(name, constructor) { }
    public:
    ScriptGUIScreenView();
};

class ScriptGUIScrollView : public ScriptGUIFramedView {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSliderX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSliderX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetSliderY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSliderY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetScrollX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetScrollX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetScrollY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetScrollY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetContentWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetContentWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetContentHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetContentHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    protected:
    ScriptGUIScrollView(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptGUIFramedView(name, constructor) { }
    public:
    ScriptGUIScrollView();
};

extern std::unique_ptr<ScriptGUIView> scriptGUIView;
extern std::unique_ptr<ScriptGUIFramedView> scriptGUIFramedView;
extern std::unique_ptr<ScriptGUIScreenView> scriptGUIScreenView;
extern std::unique_ptr<ScriptGUIScrollView> scriptGUIScrollView;

#endif