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
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    protected:
    ScriptBaseLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args))
                    :ScriptBaseClass(name, constructor) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetBurstImpulse(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetCollisionDisabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAppliedForceObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAppliedTorqueObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAppliedForceObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAppliedTorqueObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    protected:
    ScriptPhysicLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args))
                    :ScriptBaseLink(name, constructor) { }
    public:
    ScriptPhysicLink();
};

class ScriptPointPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetPoint(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetPoint(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    public:
    ScriptPointPhysicLink();
};

class ScriptGearPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAxis(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAxis(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetRatio(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    public:
    ScriptGearPhysicLink();
};

class ScriptHingePhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetFrame(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    public:
    ScriptHingePhysicLink();
};

class ScriptSliderPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetFrame(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetSliderPos(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetLinearMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetLinearMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    public:
    ScriptSliderPhysicLink();
};

class ScriptDof6PhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetFrame(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void AccessSpringStiffness(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessSpringDamping(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessSpringEquilibrium(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessMotorEnabled(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessMotorVelocity(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessMotorForce(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessAngularLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessAngularLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessLinearLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessLinearLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptDof6PhysicLink();
};

class ScriptConeTwistPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetFrame(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetSwingSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetTwistSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetTwistAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    public:
    ScriptConeTwistPhysicLink();
};

class ScriptTransformLink : public ScriptBaseLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptTransformLink();
};

class ScriptBoneLink : public ScriptBaseLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetBone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& info);
    static void SetBone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& info);
    static void GetBoneChildren(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetRelativeMat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAbsoluteMat(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptBoneLink();
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
extern ScriptBoneLink scriptBoneLink;

#endif
