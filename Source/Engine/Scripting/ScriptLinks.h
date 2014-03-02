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
    static void GetObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    protected:
    ScriptBaseLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args))
                    :ScriptBaseClass(name, constructor) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetBurstImpulse(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetCollisionDisabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void AppliedForceObjectA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AppliedTorqueObjectA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AppliedForceObjectB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AppliedTorqueObjectB(const v8::FunctionCallbackInfo<v8::Value>& args);
    protected:
    ScriptPhysicLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args))
                    :ScriptBaseLink(name, constructor) { }
    public:
    ScriptPhysicLink();
};

class ScriptPointPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessPointA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessPointB(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptPointPhysicLink();
};

class ScriptGearPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessAxisA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessAxisB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetRatio(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    public:
    ScriptGearPhysicLink();
};

class ScriptHingePhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    public:
    ScriptHingePhysicLink();
};

class ScriptSliderPhysicLink : public ScriptPhysicLink {
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetSliderPos(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetLinearMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetLinearMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    public:
    ScriptSliderPhysicLink();
};

class ScriptDof6PhysicLink : public ScriptPhysicLink {
    static btGeneric6DofSpringConstraint* EnableSpring(PhysicLink* link);
    static btGeneric6DofSpringConstraint* DisableSpring(PhysicLink* link);
    static void Constructor(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args);
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
    static void AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetSwingSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetTwistSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetTwistAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
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
    static void GetBone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetBoneChildren(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetRelativeMat(const v8::FunctionCallbackInfo<v8::Value>& args);
    static void GetAbsoluteMat(const v8::FunctionCallbackInfo<v8::Value>& args);
    public:
    ScriptBoneLink();
};

extern std::unique_ptr<ScriptBaseLink> scriptBaseLink;
extern std::unique_ptr<ScriptPhysicLink> scriptPhysicLink;
extern std::unique_ptr<ScriptPointPhysicLink> scriptPointPhysicLink;
extern std::unique_ptr<ScriptGearPhysicLink> scriptGearPhysicLink;
extern std::unique_ptr<ScriptHingePhysicLink> scriptHingePhysicLink;
extern std::unique_ptr<ScriptSliderPhysicLink> scriptSliderPhysicLink;
extern std::unique_ptr<ScriptDof6PhysicLink> scriptDof6PhysicLink;
extern std::unique_ptr<ScriptConeTwistPhysicLink> scriptConeTwistPhysicLink;
extern std::unique_ptr<ScriptTransformLink> scriptTransformLink;
extern std::unique_ptr<ScriptBoneLink> scriptBoneLink;

#endif
