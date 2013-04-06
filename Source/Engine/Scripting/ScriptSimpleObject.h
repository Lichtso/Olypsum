//
//  ScriptSimpleObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 05.04.13.
//
//

#include "ScriptDisplayObject.h"

#ifndef ScriptSimpleObject_h
#define ScriptSimpleObject_h

class ScriptCamObject : public ScriptBaseObject {
    static v8::Handle<v8::Value> GetFov(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFov(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetNear(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetNear(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetFar(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFar(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetWidth(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetWidth(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetHeight(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetHeight(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetViewRay(const v8::Arguments& args);
    static v8::Handle<v8::Value> SetMainCam(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetMainCam(const v8::Arguments& args);
    protected:
    ScriptCamObject(const char* name);
    public:
    ScriptCamObject();
};

extern ScriptCamObject scriptCamObject;

#endif
