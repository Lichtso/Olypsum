//
//  ScriptLinks.h
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ScriptLinks_h
#define ScriptLinks_h

#include "ScriptLightObject.h"

class ScriptBaseLink : public ScriptBaseClass {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    ScriptBaseLink(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args))
                    :ScriptBaseClass(name, constructor) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetBurstImpulse(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetCollisionDisabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAppliedForceObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAppliedTorqueObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAppliedForceObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAppliedTorqueObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    protected:
    ScriptPhysicLink(const char* name, v8::Handle<v8::Value>(constructor)(const v8::Arguments& args))
                    :ScriptBaseLink(name, constructor) { }
    public:
    ScriptPhysicLink();
};

class ScriptPointPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetPoint(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetPoint(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptPointPhysicLink();
};

class ScriptGearPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetAxis(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAxis(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetRatio(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptGearPhysicLink();
};

class ScriptHingePhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetHingeAngle(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularLimitMin(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularLimitMax(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorForce(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptHingePhysicLink();
};

class ScriptSliderPhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetHingeAngle(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSliderPos(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularLimitMin(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularLimitMax(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetAngularMotorForce(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearLimitMin(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearLimitMax(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearMotorEnabled(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearMotorVelocity(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetLinearMotorForce(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetLinearMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    public:
    ScriptSliderPhysicLink();
};

class ScriptDof6PhysicLink : public ScriptPhysicLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
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
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetSwingSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTwistSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    static void SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info);
    static v8::Handle<v8::Value> GetTwistAngle(v8::Local<v8::String> property, const v8::AccessorInfo& info);
    public:
    ScriptConeTwistPhysicLink();
};

class ScriptTransformLink : public ScriptBaseLink {
    static v8::Handle<v8::Value> Constructor(const v8::Arguments& args);
    static v8::Handle<v8::Value> AccessTransformation(const v8::Arguments& args);
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
