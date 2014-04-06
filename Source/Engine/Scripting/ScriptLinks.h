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
    ScriptDeclareMethod(Constructor);
    static void GetObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void GetObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    protected:
    ScriptBaseLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args)) :ScriptBaseClass(name, constructor) { }
    public:
    ScriptBaseLink();
};

class ScriptPhysicLink : public ScriptBaseLink {
    ScriptDeclareMethod(Constructor);
    static void GetBurstImpulse(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetCollisionDisabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(AppliedForceObjectA);
    ScriptDeclareMethod(AppliedTorqueObjectA);
    ScriptDeclareMethod(AppliedForceObjectB);
    ScriptDeclareMethod(AppliedTorqueObjectB);
    protected:
    ScriptPhysicLink(const char* name, void(constructor)(const v8::FunctionCallbackInfo<v8::Value>& args))
                    :ScriptBaseLink(name, constructor) { }
    public:
    ScriptPhysicLink();
};

class ScriptPointPhysicLink : public ScriptPhysicLink {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessPointA);
    ScriptDeclareMethod(AccessPointB);
    public:
    ScriptPointPhysicLink();
};

class ScriptGearPhysicLink : public ScriptPhysicLink {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessAxisA);
    ScriptDeclareMethod(AccessAxisB);
    static void GetRatio(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    public:
    ScriptGearPhysicLink();
};

class ScriptHingePhysicLink : public ScriptPhysicLink {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessFrameA);
    ScriptDeclareMethod(AccessFrameB);
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
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessFrameA);
    ScriptDeclareMethod(AccessFrameB);
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
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessFrameA);
    ScriptDeclareMethod(AccessFrameB);
    ScriptDeclareMethod(AccessSpringStiffness);
    ScriptDeclareMethod(AccessSpringDamping);
    ScriptDeclareMethod(AccessSpringEquilibrium);
    ScriptDeclareMethod(AccessMotorEnabled);
    ScriptDeclareMethod(AccessMotorVelocity);
    ScriptDeclareMethod(AccessMotorForce);
    ScriptDeclareMethod(AccessAngularLimitMin);
    ScriptDeclareMethod(AccessAngularLimitMax);
    ScriptDeclareMethod(AccessLinearLimitMin);
    ScriptDeclareMethod(AccessLinearLimitMax);
    public:
    ScriptDof6PhysicLink();
};

class ScriptConeTwistPhysicLink : public ScriptPhysicLink {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessFrameA);
    ScriptDeclareMethod(AccessFrameB);
    static void GetSwingSpanA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSwingSpanA(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetSwingSpanB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetSwingSpanB(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetTwistSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    static void GetTwistAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    public:
    ScriptConeTwistPhysicLink();
};

class ScriptTransformLink : public ScriptBaseLink {
    ScriptDeclareMethod(Constructor);
    ScriptDeclareMethod(AccessTransformation);
    public:
    ScriptTransformLink();
};

class ScriptBoneLink : public ScriptBaseLink {
    ScriptDeclareMethod(Constructor);
    static void GetBone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args);
    static void SetBone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args);
    ScriptDeclareMethod(GetBoneChildren);
    ScriptDeclareMethod(GetRelativeMat);
    ScriptDeclareMethod(GetAbsoluteMat);
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
