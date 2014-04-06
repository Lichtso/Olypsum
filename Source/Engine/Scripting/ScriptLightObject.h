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
    ScriptDeclareMethod(AccessColor);
    protected:
    ScriptLightObject(const char* name) :ScriptPhysicObject(name) { }
    public:
    ScriptLightObject();
};

class ScriptDirectionalLight : public ScriptLightObject {
    ScriptDeclareMethod(AccessBounds);
    public:
    ScriptDirectionalLight();
};

class ScriptSpotLight : public ScriptLightObject {
    static void GetCutoff(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    ScriptDeclareMethod(SetBounds);
    public:
    ScriptSpotLight();
};

class ScriptPositionalLight : public ScriptLightObject {
    static void GetOmniDirectional(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    ScriptDeclareMethod(SetBounds);
    public:
    ScriptPositionalLight();
};

extern std::unique_ptr<ScriptLightObject> scriptLightObject;
extern std::unique_ptr<ScriptDirectionalLight> scriptDirectionalLight;
extern std::unique_ptr<ScriptSpotLight> scriptSpotLight;
extern std::unique_ptr<ScriptPositionalLight> scriptPositionalLight;

#endif