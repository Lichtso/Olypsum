//
//  ScriptLightObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//
//

#ifndef ScriptLightObject_h
#define ScriptLightObject_h

#include "ScriptParticlesObject.h"

class ScriptLightObject : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetRange(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> AccessColor(const v8::Arguments& args);
    public:
    ScriptLightObject();
};

class ScriptDirectionalLight : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetBounds(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> SetBounds(const v8::Arguments& args);
    public:
    ScriptDirectionalLight();
};

class ScriptSpotLight : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetCutoff(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> SetBounds(const v8::Arguments& args);
    public:
    ScriptSpotLight();
};

class ScriptPositionalLight : public ScriptPhysicObject {
    static v8::Handle<v8::Value> GetOmniDirectional(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> SetBounds(const v8::Arguments& args);
    public:
    ScriptPositionalLight();
};

extern ScriptLightObject scriptLightObject;
extern ScriptDirectionalLight scriptDirectionalLight;
extern ScriptSpotLight scriptSpotLight;
extern ScriptPositionalLight scriptPositionalLight;

#endif