//
//  ScriptLinks.h
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//
//

#ifndef ScriptLinks_h
#define ScriptLinks_h

#include "ScriptObject.h"

class ScriptBaseLink : public ScriptBaseClass {
    static v8::Handle<v8::Value> GetObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    ScriptBaseLink(const char* name) :ScriptBaseClass(name) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    static v8::Handle<v8::Value> GetBurstImpulse(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    protected:
    static v8::Handle<v8::Value> GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    ScriptPhysicLink(const char* name) :ScriptBaseLink(name) { }
    public:
    ScriptPhysicLink();
};

class ScriptPointPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> GetPoint(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPoint(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptPointPhysicLink();
};

class ScriptGearPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> GetAxis(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAxis(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetRatio(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptGearPhysicLink();
};

class ScriptHingePhysicLink : public ScriptPhysicLink {
    public:
    ScriptHingePhysicLink();
};

class ScriptSliderPhysicLink : public ScriptPhysicLink {
    public:
    ScriptSliderPhysicLink();
};

class ScriptDof6PhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> AccessSpringStiffness(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessSpringDamping(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessSpringEquilibrium(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessMotorEnabled(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessMotorVelocity(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessMotorForce(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessAngularLimitMin(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessAngularLimitMax(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessLinearLimitMin(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessLinearLimitMax(const v8::Arguments& args);
    public:
    ScriptDof6PhysicLink();
};

class ScriptConeTwistPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> GetSwingSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTwistSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTwistAngle(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    public:
    ScriptConeTwistPhysicLink();
};

class ScriptTransformLink : public ScriptBaseLink {
    static v8::Handle<v8::Value> AccessTransformation(const v8::Arguments& args);
    protected:
    ScriptTransformLink(const char* name) :ScriptBaseLink(name) { }
    public:
    ScriptTransformLink();
};

extern ScriptBaseLink scriptBaseLink;
extern ScriptPhysicLink scriptPhysicLink;
extern ScriptPointPhysicLink scriptPointPhysicLink;
extern ScriptGearPhysicLink scriptGearPhysicLink;
extern ScriptHingePhysicLink scriptHingePhysicLink;
extern ScriptSliderPhysicLink scriptSliderPhysicLink;
extern ScriptDof6PhysicLink scriptDof6PhysicLink;
extern ScriptConeTwistPhysicLink scriptConeTwistPhysicLink;
extern ScriptTransformLink scriptTransformLink;

#endif
