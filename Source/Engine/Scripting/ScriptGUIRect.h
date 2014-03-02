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
        ScriptScope();
        T* objectPtr = getDataOfInstance<T>(args.This());
        switch(objectPtr->sizeAlignment) {
            case GUISizeAlignment::None:
                ScriptReturn(ScriptString("none"));
                return;
            case GUISizeAlignment::Width:
                ScriptReturn(ScriptString("width"));
                return;
            case GUISizeAlignment::Height:
                ScriptReturn(ScriptString("height"));
                return;
            case GUISizeAlignment::All:
                ScriptReturn(ScriptString("all"));
                return;
        }
    }
    template<typename T> static void SetSizeAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
        ScriptScope();
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
        ScriptScope();
        T* objectPtr = getDataOfInstance<T>(args.This());
        switch(objectPtr->orientation) {
            case GUIOrientation::Left:
                ScriptReturn(ScriptString("left"));
                return;
            case GUIOrientation::Right:
                ScriptReturn(ScriptString("right"));
                return;
            case GUIOrientation::Bottom:
                ScriptReturn(ScriptString("bottom"));
                return;
            case GUIOrientation::Top:
                ScriptReturn(ScriptString("top"));
                return;
            case GUIOrientation::Vertical:
                ScriptReturn(ScriptString("vertical"));
                return;
            case GUIOrientation::Horizontal:
                ScriptReturn(ScriptString("horizontal"));
                return;
        }
    }
    template<typename T> static void SetOrientation(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
        ScriptScope();
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
        ScriptScope();
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(args.This());
        const char* str = cStrOfV8(value);
        if(strcmp(str, "vertical") == 0)
            objectPtr->orientation = GUIOrientation::Vertical;
        else if(strcmp(str, "horizontal") == 0)
            objectPtr->orientation = GUIOrientation::Horizontal;
    }
    template<typename T> static T* getDataOfInstance(const v8::Local<v8::Value>& value) {
        ScriptScope();
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    static v8::Handle<v8::Value> initInstance(v8::Local<v8::Object> instance, GUIView* parent, GUIRect* child);
    ScriptGUIRect();
};

extern std::unique_ptr<ScriptGUIRect> scriptGUIRect;

#endif