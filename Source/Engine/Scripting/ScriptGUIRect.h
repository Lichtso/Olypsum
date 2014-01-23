//
//  ScriptGUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptGUIRect_h
#define ScriptGUIRect_h

#include "ScriptIO.h"

class ScriptGUIRect : public ScriptClass {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetPosX(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetPosX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetPosY(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetPosY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetWidth(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetHeight(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetVisible(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetVisible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetFocus(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetFocus(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetParent(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void Remove(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void UpdateContent(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptGUIRect(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args));
    public:
    template<typename T> static void GetSizeAlignment(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
        v8::HandleScope handleScope;
        T* objectPtr = getDataOfInstance<T>(args.This());
        switch(objectPtr->sizeAlignment) {
            case GUISizeAlignment::None:
                args.GetReturnValue().Set(v8::String::New("none"));
                return;
            case GUISizeAlignment::Width:
                args.GetReturnValue().Set(v8::String::New("width"));
                return;
            case GUISizeAlignment::Height:
                args.GetReturnValue().Set(v8::String::New("height"));
                return;
            case GUISizeAlignment::All:
                args.GetReturnValue().Set(v8::String::New("all"));
                return;
        }
    }
    template<typename T> static void SetSizeAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(args.This());
        const char* str = cStrOfV8(value);
        if(strcmp(str, "none") == 0)
            objectPtr->sizeAlignment = GUISizeAlignment::None;
        else if(strcmp(str, "width") == 0)
            objectPtr->sizeAlignment = GUISizeAlignment::Width;
        else if(strcmp(str, "height") == 0)
            objectPtr->sizeAlignment = GUISizeAlignment::Height;
        else if(strcmp(str, "all") == 0)
            objectPtr->sizeAlignment = GUISizeAlignment::All;
    }
    template<typename T> static void GetOrientation(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
        v8::HandleScope handleScope;
        T* objectPtr = getDataOfInstance<T>(args.This());
        switch(objectPtr->orientation) {
            case GUIOrientation::Left:
                args.GetReturnValue().Set(v8::String::New("left"));
                return;
            case GUIOrientation::Right:
                args.GetReturnValue().Set(v8::String::New("right"));
                return;
            case GUIOrientation::Bottom:
                args.GetReturnValue().Set(v8::String::New("bottom"));
                return;
            case GUIOrientation::Top:
                args.GetReturnValue().Set(v8::String::New("top"));
                return;
            case GUIOrientation::Vertical:
                args.GetReturnValue().Set(v8::String::New("vertical"));
                return;
            case GUIOrientation::Horizontal:
                args.GetReturnValue().Set(v8::String::New("horizontal"));
                return;
        }
    }
    template<typename T> static void SetOrientation(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(args.This());
        const char* str = cStrOfV8(value);
        if(strcmp(str, "left") == 0)
            objectPtr->orientation = GUIOrientation::Left;
        else if(strcmp(str, "right") == 0)
            objectPtr->orientation = GUIOrientation::Right;
        else if(strcmp(str, "bottom") == 0)
            objectPtr->orientation = GUIOrientation::Bottom;
        else if(strcmp(str, "top") == 0)
            objectPtr->orientation = GUIOrientation::Top;
        else if(strcmp(str, "vertical") == 0)
            objectPtr->orientation = GUIOrientation::Vertical;
        else if(strcmp(str, "horizontal") == 0)
            objectPtr->orientation = GUIOrientation::Horizontal;
    }
    template<typename T> static void SetOrientationDual(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(args.This());
        const char* str = cStrOfV8(value);
        if(strcmp(str, "vertical") == 0)
            objectPtr->orientation = GUIOrientation::Vertical;
        else if(strcmp(str, "horizontal") == 0)
            objectPtr->orientation = GUIOrientation::Horizontal;
    }
    template<typename T> static T* getDataOfInstance(const v8::Local<v8::Value>& value) {
        v8::HandleScope handleScope;
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    static v8::Handle<v8::Value> initInstance(v8::Local<v8::Object> instance, GUIView* parent, GUIRect* child);
    ScriptGUIRect();
};

extern ScriptGUIRect scriptGUIRect;

#endif