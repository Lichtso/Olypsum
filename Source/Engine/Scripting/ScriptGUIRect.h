//
//  ScriptGUIRect.h
//  Olypsum
//
//  Created by Alexander Meißner on 16.04.13.
//
//

#include "ScriptIO.h"

#ifndef ScriptGUIRect_h
#define ScriptGUIRect_h

class ScriptGUIRect : public ScriptClass {
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
    static v8::Handle<v8::Value> MoveToParent(const v8::Arguments& args);
    static v8::Handle<v8::Value> UpdateContent(const v8::Arguments& args);
    protected:
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    ScriptGUIRect(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args));
    public:
    template<typename T> static T* getDataOfInstance(const v8::Local<v8::Value>& value) {
        v8::HandleScope handleScope;
        v8::Local<v8::Object> object = v8::Local<v8::Object>::Cast(value);
        v8::Local<v8::External> wrap = v8::Local<v8::External>::Cast(object->GetInternalField(0));
        return static_cast<T*>(wrap->Value());
    }
    ScriptGUIRect();
};

extern ScriptGUIRect scriptGUIRect;

#endif