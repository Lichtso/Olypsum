//
//  ScriptLinks.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptLinks.h"

static void activateConstraint(btTypedConstraint* constraint) {
    constraint->getRigidBodyA().activate();
    constraint->getRigidBodyB().activate();
}



void ScriptBaseLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        BaseLink* linkPtr = static_cast<BaseLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 2 &&
             scriptBaseObject->isCorrectInstance(args[0]) && scriptBaseObject->isCorrectInstance(args[1])) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject->getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject->getDataOfInstance<BaseObject>(args[1]);
        BaseLink* linkPtr = new BaseLink();
        if(linkPtr->init(initializer)) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("BaseLink Constructor: Invalid argument");
    }
    
    return ScriptException("BaseLink Constructor: Class can't be initialized");
}

void ScriptBaseLink::GetObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(args.This());
    ScriptReturn(linkPtr->a->scriptInstance);
}

void ScriptBaseLink::GetObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(args.This());
    ScriptReturn(linkPtr->b->scriptInstance);
}

ScriptBaseLink::ScriptBaseLink() :ScriptBaseLink("BaseLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "objectA", GetObjectA, 0);
    ScriptAccessor(objectTemplate, "objectB", GetObjectB, 0);
    
    ScriptInherit(scriptBaseClass);
}



void ScriptPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("PhysicLink Constructor: Class can't be instantiated");
}

void ScriptPhysicLink::GetBurstImpulse(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(args.This())->constraint;
    ScriptReturn(constraint->getBreakingImpulseThreshold());
}

void ScriptPhysicLink::SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(args.This())->constraint;
    constraint->setBreakingImpulseThreshold(value->NumberValue());
}

void ScriptPhysicLink::GetCollisionDisabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(getDataOfInstance<PhysicLink>(args.This())->isCollisionDisabled());
}

void ScriptPhysicLink::SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    
    getDataOfInstance<PhysicLink>(args.This())->setCollisionDisabled(value->BooleanValue());
}

void ScriptPhysicLink::AppliedForceObjectA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(scriptVector3->newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedForceBodyA));
}

void ScriptPhysicLink::AppliedTorqueObjectA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(scriptVector3->newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedTorqueBodyA));
}

void ScriptPhysicLink::AppliedForceObjectB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(scriptVector3->newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedForceBodyB));
}

void ScriptPhysicLink::AppliedTorqueObjectB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    ScriptReturn(scriptVector3->newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedTorqueBodyB));
}

ScriptPhysicLink::ScriptPhysicLink() :ScriptBaseLink("PhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "burstImpulse", GetBurstImpulse, SetBurstImpulse);
    ScriptAccessor(objectTemplate, "collisionDisabled", GetCollisionDisabled, SetCollisionDisabled);
    ScriptMethod(objectTemplate, "appliedForceObjectA", AppliedForceObjectA);
    ScriptMethod(objectTemplate, "appliedTorqueObjectA", AppliedTorqueObjectA);
    ScriptMethod(objectTemplate, "appliedForceObjectB", AppliedForceObjectB);
    ScriptMethod(objectTemplate, "appliedTorqueObjectB", AppliedTorqueObjectB);
    
    ScriptInherit(scriptBaseLink);
}



void ScriptPointPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptVector3->isCorrectInstance(args[2]) && scriptVector3->isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btPoint2PointConstraint(*a->getBody(), *b->getBody(),
                                                                  scriptVector3->getDataOfInstance(args[2]),
                                                                  scriptVector3->getDataOfInstance(args[3])))) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("PointPhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("PointPhysicLink Constructor: Class can't be initialized");
}

void ScriptPointPhysicLink::AccessPointA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 point = scriptVector3->getDataOfInstance(args[0]);
        constraint->setPivotA(point);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(constraint->getPivotInA()));
}

void ScriptPointPhysicLink::AccessPointB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 point = scriptVector3->getDataOfInstance(args[0]);
        constraint->setPivotB(point);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(constraint->getPivotInB()));
}

ScriptPointPhysicLink::ScriptPointPhysicLink() :ScriptPhysicLink("PointPhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptMethod(objectTemplate, "pointA", AccessPointA);
    ScriptMethod(objectTemplate, "pointB", AccessPointB);
    
    ScriptInherit(scriptPhysicLink);
}



void ScriptGearPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 5 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptVector3->isCorrectInstance(args[2]) && scriptVector3->isCorrectInstance(args[3]) &&
             args[4]->IsNumber()) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btGearConstraint(*a->getBody(), *b->getBody(),
                                                           scriptVector3->getDataOfInstance(args[2]),
                                                           scriptVector3->getDataOfInstance(args[3]),
                                                           args[4]->NumberValue()))) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("GearPhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("GearPhysicLink Constructor: Class can't be initialized");
}

void ScriptGearPhysicLink::AccessAxisA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 axis = scriptVector3->getDataOfInstance(args[0]);
        constraint->setAxisA(axis);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(constraint->getAxisA()));
}

void ScriptGearPhysicLink::AccessAxisB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 axis = scriptVector3->getDataOfInstance(args[0]);
        constraint->setAxisB(axis);
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptVector3->newInstance(constraint->getAxisB()));
}

void ScriptGearPhysicLink::GetRatio(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getRatio());
}

void ScriptGearPhysicLink::SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setRatio(value->NumberValue());
}

ScriptGearPhysicLink::ScriptGearPhysicLink() :ScriptPhysicLink("GearPhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "ratio", GetRatio, SetRatio);
    ScriptMethod(objectTemplate, "axisA", AccessAxisA);
    ScriptMethod(objectTemplate, "axisB", AccessAxisB);
    
    ScriptInherit(scriptPhysicLink);
}



void ScriptHingePhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptMatrix4->isCorrectInstance(args[2]) && scriptMatrix4->isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btHingeConstraint(*a->getBody(), *b->getBody(),
                                                            scriptMatrix4->getDataOfInstance(args[2])->getBTTransform(),
                                                            scriptMatrix4->getDataOfInstance(args[3])->getBTTransform(),
                                                            true))) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("HingePhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("HingePhysicLink Constructor: Class can't be initialized");
}

void ScriptHingePhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4->getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetA()));
}

void ScriptHingePhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4->getDataOfInstance(args[0])->getBTTransform());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetB()));
}

void ScriptHingePhysicLink::GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getHingeAngle());
}

void ScriptHingePhysicLink::GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getLowerLimit());
}

void ScriptHingePhysicLink::SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(value->NumberValue(), constraint->getUpperLimit());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getUpperLimit());
}

void ScriptHingePhysicLink::SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(constraint->getLowerLimit(), value->NumberValue());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getEnableAngularMotor());
}

void ScriptHingePhysicLink::SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(value->BooleanValue(),
                                   constraint->getMotorTargetVelosity(),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getMotorTargetVelosity());
}

void ScriptHingePhysicLink::SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   value->NumberValue(),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getMaxMotorImpulse());
}

void ScriptHingePhysicLink::SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   constraint->getMotorTargetVelosity(),
                                   value->NumberValue());
    activateConstraint(constraint);
}

ScriptHingePhysicLink::ScriptHingePhysicLink() :ScriptPhysicLink("HingePhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "hingeAngle", GetHingeAngle, 0);
    ScriptAccessor(objectTemplate, "angularLimitMin", GetAngularLimitMin, SetAngularLimitMin);
    ScriptAccessor(objectTemplate, "angularLimitMax", GetAngularLimitMax, SetAngularLimitMax);
    ScriptAccessor(objectTemplate, "angularMotorEnabled", GetAngularMotorEnabled, SetAngularMotorEnabled);
    ScriptAccessor(objectTemplate, "angularMotorVelocity", GetAngularMotorVelocity, SetAngularMotorVelocity);
    ScriptAccessor(objectTemplate, "angularMotorForce", GetAngularMotorForce, SetAngularMotorForce);
    ScriptMethod(objectTemplate, "frameA", AccessFrameA);
    ScriptMethod(objectTemplate, "frameB", AccessFrameB);
    
    ScriptInherit(scriptPhysicLink);
}



void ScriptSliderPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptMatrix4->isCorrectInstance(args[2]) && scriptMatrix4->isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btSliderConstraint(*a->getBody(), *b->getBody(),
                                                             scriptMatrix4->getDataOfInstance(args[2])->getBTTransform(),
                                                             scriptMatrix4->getDataOfInstance(args[3])->getBTTransform(),
                                                             true))) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("SliderPhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("SliderPhysicLink Constructor: Class can't be initialized");
}

void ScriptSliderPhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4->getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetA()));
}

void ScriptSliderPhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4->getDataOfInstance(args[0])->getBTTransform());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetB()));
}

void ScriptSliderPhysicLink::GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getAngularPos());
}

void ScriptSliderPhysicLink::GetSliderPos(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getLinearPos());
}

void ScriptSliderPhysicLink::GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getLowerAngLimit());
}

void ScriptSliderPhysicLink::SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLowerAngLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getUpperAngLimit());
}

void ScriptSliderPhysicLink::SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setUpperAngLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getPoweredAngMotor());
}

void ScriptSliderPhysicLink::SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setPoweredAngMotor(value->BooleanValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getTargetAngMotorVelocity());
}

void ScriptSliderPhysicLink::SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setTargetAngMotorVelocity(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getMaxAngMotorForce());
}

void ScriptSliderPhysicLink::SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setMaxAngMotorForce(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getLowerLinLimit());
}

void ScriptSliderPhysicLink::SetLinearLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLowerLinLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getUpperLinLimit());
}

void ScriptSliderPhysicLink::SetLinearLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setUpperLinLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getPoweredLinMotor());
}

void ScriptSliderPhysicLink::SetLinearMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsBoolean()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setPoweredLinMotor(value->BooleanValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getTargetLinMotorVelocity());
}

void ScriptSliderPhysicLink::SetLinearMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setTargetLinMotorVelocity(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getMaxLinMotorForce());
}

void ScriptSliderPhysicLink::SetLinearMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setMaxLinMotorForce(value->NumberValue());
    activateConstraint(constraint);
}

ScriptSliderPhysicLink::ScriptSliderPhysicLink() :ScriptPhysicLink("SliderPhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "hingeAngle", GetHingeAngle, 0);
    ScriptAccessor(objectTemplate, "sliderPos", GetSliderPos, 0);
    ScriptAccessor(objectTemplate, "angularLimitMin", GetAngularLimitMin, SetAngularLimitMin);
    ScriptAccessor(objectTemplate, "angularLimitMax", GetAngularLimitMax, SetAngularLimitMax);
    ScriptAccessor(objectTemplate, "angularMotorEnabled", GetAngularMotorEnabled, SetAngularMotorEnabled);
    ScriptAccessor(objectTemplate, "angularMotorVelocity", GetAngularMotorVelocity, SetAngularMotorVelocity);
    ScriptAccessor(objectTemplate, "angularMotorForce", GetAngularMotorForce, SetAngularMotorForce);
    ScriptAccessor(objectTemplate, "linearLimitMin", GetLinearLimitMin, SetLinearLimitMin);
    ScriptAccessor(objectTemplate, "linearLimitMax", GetLinearLimitMax, SetLinearLimitMax);
    ScriptAccessor(objectTemplate, "linearMotorEnabled", GetLinearMotorEnabled, SetLinearMotorEnabled);
    ScriptAccessor(objectTemplate, "linearMotorVelocity", GetLinearMotorVelocity, SetLinearMotorVelocity);
    ScriptAccessor(objectTemplate, "linearMotorForce", GetLinearMotorForce, SetLinearMotorForce);
    ScriptMethod(objectTemplate, "frameA", AccessFrameA);
    ScriptMethod(objectTemplate, "frameB", AccessFrameB);
    
    ScriptInherit(scriptPhysicLink);
}



btGeneric6DofSpringConstraint* ScriptDof6PhysicLink::EnableSpring(PhysicLink* link) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(link->constraint);
    activateConstraint(constraint);
    if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
        return static_cast<btGeneric6DofSpringConstraint*>(constraint);
    auto newConstraint = new btGeneric6DofSpringConstraint(constraint->getRigidBodyA(), constraint->getRigidBodyB(),
                                                           constraint->getFrameOffsetA(), constraint->getFrameOffsetB(),
                                                           true);
    delete constraint;
    link->constraint = newConstraint;
    return newConstraint;
}

btGeneric6DofSpringConstraint* ScriptDof6PhysicLink::DisableSpring(PhysicLink* link) {
    auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
    if(constraint->getConstraintType() == D6_CONSTRAINT_TYPE)
        return constraint;
    auto newConstraint = new btGeneric6DofConstraint(constraint->getRigidBodyA(), constraint->getRigidBodyB(),
                                                     constraint->getFrameOffsetA(), constraint->getFrameOffsetB(),
                                                     true);
    delete constraint;
    link->constraint = newConstraint;
    return static_cast<btGeneric6DofSpringConstraint*>(newConstraint);
}

void ScriptDof6PhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptMatrix4->isCorrectInstance(args[2]) && scriptMatrix4->isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        btTransform transA = scriptMatrix4->getDataOfInstance(args[2])->getBTTransform(),
                    transB = scriptMatrix4->getDataOfInstance(args[3])->getBTTransform();
        PhysicLink* linkPtr = new PhysicLink();
        btGeneric6DofConstraint* constraint = new btGeneric6DofConstraint(*a->getBody(), *b->getBody(), transA, transB, true);
        if(linkPtr->init(initializer, constraint)) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("Dof6PhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("Dof6PhysicLink Constructor: Class can't be initialized");
}

void ScriptDof6PhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4->getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetA()));
}

void ScriptDof6PhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4->getDataOfInstance(args[0])->getBTTransform());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetB()));
}

void ScriptDof6PhysicLink::AccessSpringStiffness(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto link = getDataOfInstance<PhysicLink>(args.This());
    auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        int index = args[0]->IntegerValue();
        btScalar stiffness = args[1]->NumberValue();
        if(stiffness > 0.0) {
            constraint = EnableSpring(link);
            constraint->enableSpring(index, true);
        }else if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE) {
            constraint->enableSpring(index, false);
            constraint->setStiffness(index, stiffness);
            bool disable = true;
            for(int i = 0; i < 6; i ++)
                if(constraint->getEnableSpring(i)) {
                    disable = false;
                    break;
                }
            if(disable)
                constraint = DisableSpring(link);
            activateConstraint(constraint);
        }
        ScriptReturn(args[1]);
    }else if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
        ScriptReturn(constraint->getStiffness(args[0]->IntegerValue()));
}

void ScriptDof6PhysicLink::AccessSpringDamping(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto link = getDataOfInstance<PhysicLink>(args.This());
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        auto constraint = EnableSpring(link);
        constraint->setDamping(args[0]->IntegerValue(), args[1]->NumberValue());
        ScriptReturn(args[1]);
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE) return;
        ScriptReturn(constraint->getDamping(args[0]->IntegerValue()));
    }
}

void ScriptDof6PhysicLink::AccessSpringEquilibrium(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto link = getDataOfInstance<PhysicLink>(args.This());
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        auto constraint = EnableSpring(link);
        constraint->setEquilibriumPoint(args[0]->IntegerValue(), args[1]->NumberValue());
        ScriptReturn(args[1]);
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE) return;
        ScriptReturn(constraint->getEquilibriumPoint(args[0]->IntegerValue()));
    }
}

void ScriptDof6PhysicLink::AccessMotorEnabled(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    
    bool* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_enableMotor[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_enableMotor;
    }
    
    if(args.Length() > 1 && args[1]->IsBoolean()) {
        activateConstraint(constraint);
        *value = args[1]->BooleanValue();
        ScriptReturn(args[1]);
    }else
        ScriptReturn(*value);
}

void ScriptDof6PhysicLink::AccessMotorVelocity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    
    float* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_targetVelocity[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_targetVelocity;
    }
    
    if(args.Length() > 1 && args[1]->IsNumber()) {
        activateConstraint(constraint);
        *value = args[1]->NumberValue();
        ScriptReturn(args[1]);
    }else
        ScriptReturn(*value);
}

void ScriptDof6PhysicLink::AccessMotorForce(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    
    float* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_maxMotorForce[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_maxMotorForce;
    }
    
    if(args.Length() > 1 && args[1]->IsNumber()) {
        activateConstraint(constraint);
        *value = args[1]->NumberValue();
        ScriptReturn(args[1]);
    }else
        ScriptReturn(*value);
}

void ScriptDof6PhysicLink::AccessAngularLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setAngularLowerLimit(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else{
        btVector3 value;
        constraint->getAngularLowerLimit(value);
        ScriptReturn(scriptVector3->newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessAngularLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setAngularUpperLimit(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else{
        btVector3 value;
        constraint->getAngularUpperLimit(value);
        ScriptReturn(scriptVector3->newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessLinearLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setLinearLowerLimit(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else{
        btVector3 value;
        constraint->getLinearLowerLimit(value);
        ScriptReturn(scriptVector3->newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessLinearLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setLinearUpperLimit(scriptVector3->getDataOfInstance(args[0]));
        ScriptReturn(args[0]);
    }else{
        btVector3 value;
        constraint->getLinearUpperLimit(value);
        ScriptReturn(scriptVector3->newInstance(value));
    }
}

ScriptDof6PhysicLink::ScriptDof6PhysicLink() :ScriptPhysicLink("Dof6PhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptMethod(objectTemplate, "frameA", AccessFrameA);
    ScriptMethod(objectTemplate, "frameB", AccessFrameB);
    ScriptMethod(objectTemplate, "springStiffness", AccessSpringStiffness);
    ScriptMethod(objectTemplate, "springDamping", AccessSpringDamping);
    ScriptMethod(objectTemplate, "springEquilibrium", AccessSpringEquilibrium);
    ScriptMethod(objectTemplate, "motorEnabled", AccessMotorEnabled);
    ScriptMethod(objectTemplate, "motorVelocity", AccessMotorVelocity);
    ScriptMethod(objectTemplate, "motorForce", AccessMotorForce);
    ScriptMethod(objectTemplate, "angularLimitMin", AccessAngularLimitMin);
    ScriptMethod(objectTemplate, "angularLimitMax", AccessAngularLimitMax);
    ScriptMethod(objectTemplate, "linearLimitMin", AccessLinearLimitMin);
    ScriptMethod(objectTemplate, "linearLimitMax", AccessLinearLimitMax);
    
    ScriptInherit(scriptPhysicLink);
}



void ScriptConeTwistPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject->isCorrectInstance(args[0]) && scriptRigidObject->isCorrectInstance(args[1]) &&
             scriptMatrix4->isCorrectInstance(args[2]) && scriptMatrix4->isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject->getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject->getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btConeTwistConstraint(*a->getBody(), *b->getBody(),
                                                             scriptMatrix4->getDataOfInstance(args[2])->getBTTransform(),
                                                             scriptMatrix4->getDataOfInstance(args[3])->getBTTransform()))) {
            linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("ConeTwistPhysicLink Constructor: Invalid argument");
    }
    
    return ScriptException("ConeTwistPhysicLink Constructor: Class can't be initialized");
}

void ScriptConeTwistPhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4->getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetA()));
}

void ScriptConeTwistPhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4->isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4->getDataOfInstance(args[0])->getBTTransform());
        ScriptReturn(args[0]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(constraint->getFrameOffsetB()));
}

void ScriptConeTwistPhysicLink::GetSwingSpanA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getSwingSpan1());
}

void ScriptConeTwistPhysicLink::SetSwingSpanA(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(5, value->NumberValue());
    activateConstraint(constraint);
}

void ScriptConeTwistPhysicLink::GetSwingSpanB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getSwingSpan2());
}

void ScriptConeTwistPhysicLink::SetSwingSpanB(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(4, value->NumberValue());
    activateConstraint(constraint);
}

void ScriptConeTwistPhysicLink::GetTwistSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getTwistSpan());
}

void ScriptConeTwistPhysicLink::SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(3, value->NumberValue());
    activateConstraint(constraint);
}

void ScriptConeTwistPhysicLink::GetTwistAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    ScriptReturn(constraint->getTwistAngle());
}

ScriptConeTwistPhysicLink::ScriptConeTwistPhysicLink() :ScriptPhysicLink("ConeTwistPhysicLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "swingSpanA", GetSwingSpanA, SetSwingSpanA);
    ScriptAccessor(objectTemplate, "swingSpanB", GetSwingSpanB, SetSwingSpanB);
    ScriptAccessor(objectTemplate, "twistSpan", GetTwistSpan, SetTwistSpan);
    ScriptAccessor(objectTemplate, "twistAngle", GetTwistAngle, 0);
    ScriptMethod(objectTemplate, "frameA", AccessFrameA);
    ScriptMethod(objectTemplate, "frameB", AccessFrameB);
    
    ScriptInherit(scriptPhysicLink);
}



void ScriptTransformLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        TransformLink* linkPtr = static_cast<TransformLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() > 2 &&
             scriptBaseObject->isCorrectInstance(args[0]) && scriptBaseObject->isCorrectInstance(args[1])) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject->getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject->getDataOfInstance<BaseObject>(args[1]);
        TransformLink* linkPtr = new TransformLink();
        std::vector<btTransform> transformations;
        for(unsigned int i = 2; i < args.Length(); i ++) {
            if(!scriptMatrix4->isCorrectInstance(args[i]))
                return ScriptException("TransformLink Constructor: Invalid argument");
            transformations.push_back(scriptMatrix4->getDataOfInstance(args[i])->getBTTransform());
        }
        if(linkPtr->init(initializer, transformations)) {
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("TransformLink Constructor: Invalid argument");
    }
    
    return ScriptException("TransformLink Constructor: Class can't be initialized");
}

void ScriptTransformLink::AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    TransformLink* linkPtr = getDataOfInstance<TransformLink>(args.This());
    if(args.Length() == 0)
        return ScriptException("TransformLink transformation(): Too few arguments");
    if(!args[0]->IsNumber() || args[0]->Uint32Value() >= linkPtr->transformations.size())
        return ScriptException("TransformLink transformation(): Invalid argument");
    btTransform& transform = linkPtr->transformations[args[0]->Uint32Value()];
    if(args.Length() == 2 && scriptMatrix4->isCorrectInstance(args[1])) {
        Matrix4* mat = scriptMatrix4->getDataOfInstance(args[1]);
        if(mat->isValid())
            transform = mat->getBTTransform();
        ScriptReturn(args[1]);
    }else
        ScriptReturn(scriptMatrix4->newInstance(Matrix4(transform)));
}

ScriptTransformLink::ScriptTransformLink() :ScriptBaseLink("TransformLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptMethod(objectTemplate, "transformation", AccessTransformation);
    
    ScriptInherit(scriptBaseLink);
}



void ScriptBoneLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        BoneLink* linkPtr = static_cast<BoneLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance.Reset(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        ScriptReturn(args.This());
        return;
    }else if(args.Length() == 3 &&
             scriptBaseObject->isCorrectInstance(args[0]) && scriptBaseObject->isCorrectInstance(args[1]) &&
             args[2]->IsString()) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject->getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject->getDataOfInstance<BaseObject>(args[1]);
        BoneLink* linkPtr = new BoneLink();
        if(linkPtr->init(initializer, linkPtr->getBoneByName(cStrOfV8(args[2])))) {
            args.This()->SetInternalField(0, v8::External::New(v8::Isolate::GetCurrent(), linkPtr));
            ScriptReturn(args.This());
            return;
        }else
            return ScriptException("BoneLink Constructor: Invalid argument");
    }
    
    return ScriptException("BoneLink Constructor: Class can't be initialized");
}

void ScriptBoneLink::GetBone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    ScriptReturn(linkPtr->bone->name.c_str());
}

void ScriptBoneLink::SetBone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    ScriptScope();
    if(!value->IsString()) return;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    Bone* bone = linkPtr->getBoneByName(cStrOfV8(value));
    if(bone) linkPtr->bone = bone;
}

void ScriptBoneLink::GetBoneChildren(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(v8::Isolate::GetCurrent(), linkPtr->bone->children.size());
    for(auto child : linkPtr->bone->children)
        objects->Set(i ++, ScriptString(child->name.c_str()));
    ScriptReturn(objects);
}

void ScriptBoneLink::GetRelativeMat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    ScriptReturn(scriptMatrix4->newInstance(Matrix4(linkPtr->bone->relativeMat)));
}

void ScriptBoneLink::GetAbsoluteMat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    ScriptReturn(scriptMatrix4->newInstance(Matrix4(linkPtr->bone->absoluteInv)));
}

ScriptBoneLink::ScriptBoneLink() :ScriptBaseLink("BoneLink", Constructor) {
    ScriptScope();
    
    v8::Local<v8::ObjectTemplate> objectTemplate = (*functionTemplate)->PrototypeTemplate();
    ScriptAccessor(objectTemplate, "bone", GetBone, SetBone);
    ScriptMethod(objectTemplate, "getBoneChildren", GetBoneChildren);
    ScriptMethod(objectTemplate, "getRelativeMat", GetRelativeMat);
    ScriptMethod(objectTemplate, "getAbsoluteMat", GetAbsoluteMat);
    
    ScriptInherit(scriptBaseLink);
}