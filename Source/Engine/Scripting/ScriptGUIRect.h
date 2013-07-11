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
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetPosX(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPosX(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetPosY(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPosY(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetVisible(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetVisible(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFocus(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFocus(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetParent(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> Remove(const v8::Arguments& args);
    static v8::Handle<v8::Value> UpdateContent(const v8::Arguments& args);
    protected:
    ScriptGUIRect(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args));
    public:
    template<typename T> static v8::Handle<v8::Value> GetSizeAlignment(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
        v8::HandleScope handleScope;
        T* objectPtr = getDataOfInstance<T>(info.This());
        switch(objectPtr->sizeAlignment) {
            case GUISizeAlignment::None:
                return handleScope.Close(v8::String::New("none"));
            case GUISizeAlignment::Width:
                return handleScope.Close(v8::String::New("width"));
            case GUISizeAlignment::Height:
                return handleScope.Close(v8::String::New("height"));
            case GUISizeAlignment::All:
                return handleScope.Close(v8::String::New("all"));
        }
    }
    template<typename T> static void SetSizeAlignment(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(info.This());
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
    template<typename T> static v8::Handle<v8::Value> GetOrientation(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
        v8::HandleScope handleScope;
        T* objectPtr = getDataOfInstance<T>(info.This());
        switch(objectPtr->orientation) {
            case GUIOrientation::Left:
                return handleScope.Close(v8::String::New("left"));
            case GUIOrientation::Right:
                return handleScope.Close(v8::String::New("right"));
            case GUIOrientation::Bottom:
                return handleScope.Close(v8::String::New("bottom"));
            case GUIOrientation::Top:
                return handleScope.Close(v8::String::New("top"));
            case GUIOrientation::Vertical:
                return handleScope.Close(v8::String::New("vertical"));
            case GUIOrientation::Horizontal:
                return handleScope.Close(v8::String::New("horizontal"));
        }
    }
    template<typename T> static void SetOrientation(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(info.This());
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
    template<typename T> static void SetOrientationDual(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
        v8::HandleScope handleScope;
        if(!value->IsString()) return;
        T* objectPtr = getDataOfInstance<T>(info.This());
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