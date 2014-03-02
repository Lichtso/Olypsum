//
//  ScriptLightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptLightObject_h
#define ScriptLightObject_h

#include "ScriptParticlesObject.h"

class ScriptLightObject : public ScriptPhysicObject {
    static void GetRange(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void AccessColor(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptLightObject(const char* name) :ScriptPhysicObject(name) { }
    public:
    ScriptLightObject();
};

class ScriptDirectionalLight : public ScriptLightObject {
    static void AccessBounds(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptDirectionalLight();
};

class ScriptSpotLight : public ScriptLightObject {
    static void GetCutoff(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptSpotLight();
};

class ScriptPositionalLight : public ScriptLightObject {
    static void GetOmniDirectional(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBounds(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptPositionalLight();
};

extern std::unique_ptr<ScriptLightObject> scriptLightObject;
extern std::unique_ptr<ScriptDirectionalLight> scriptDirectionalLight;
extern std::unique_ptr<ScriptSpotLight> scriptSpotLight;
extern std::unique_ptr<ScriptPositionalLight> scriptPositionalLight;

#endif