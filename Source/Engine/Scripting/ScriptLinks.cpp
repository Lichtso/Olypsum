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
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        BaseLink* linkPtr = static_cast<BaseLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 2 &&
             scriptBaseObject.isCorrectInstance(args[0]) && scriptBaseObject.isCorrectInstance(args[1])) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject.getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject.getDataOfInstance<BaseObject>(args[1]);
        BaseLink* linkPtr = new BaseLink();
        if(linkPtr->init(initializer)) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("BaseLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("BaseLink Constructor: Class can't be initialized");
}

void ScriptBaseLink::GetObjectA(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(args.This());
    args.GetReturnValue().Set(linkPtr->a->scriptInstance);
}

void ScriptBaseLink::GetObjectB(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(args.This());
    args.GetReturnValue().Set(linkPtr->b->scriptInstance);
}

ScriptBaseLink::ScriptBaseLink() :ScriptBaseLink("BaseLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("objectA"), GetObjectA);
    objectTemplate->SetAccessor(v8::String::New("objectB"), GetObjectB);
    
    functionTemplate->Inherit(scriptBaseClass.functionTemplate);
}



void ScriptPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    return args.ScriptException("PhysicLink Constructor: Class can't be instantiated");
}

void ScriptPhysicLink::GetBurstImpulse(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(args.This())->constraint;
    args.GetReturnValue().Set(constraint->getBreakingImpulseThreshold());
}

void ScriptPhysicLink::SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(args.This())->constraint;
    constraint->setBreakingImpulseThreshold(value->NumberValue());
}

void ScriptPhysicLink::GetCollisionDisabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(getDataOfInstance<PhysicLink>(args.This())->isCollisionDisabled());
}

void ScriptPhysicLink::SetCollisionDisabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    
    getDataOfInstance<PhysicLink>(args.This())->setCollisionDisabled(value->BooleanValue());
}

void ScriptPhysicLink::AppliedForceObjectA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(scriptVector3.newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedForceBodyA));
}

void ScriptPhysicLink::AppliedTorqueObjectA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(scriptVector3.newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedTorqueBodyA));
}

void ScriptPhysicLink::AppliedForceObjectB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(scriptVector3.newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedForceBodyB));
}

void ScriptPhysicLink::AppliedTorqueObjectB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    args.GetReturnValue().Set(scriptVector3.newInstance(getDataOfInstance<PhysicLink>(args.This())->constraint->m_appliedTorqueBodyB));
}

ScriptPhysicLink::ScriptPhysicLink() :ScriptBaseLink("PhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("burstImpulse"), GetBurstImpulse, SetBurstImpulse);
    objectTemplate->SetAccessor(v8::String::New("collisionDisabled"), GetCollisionDisabled, SetCollisionDisabled);
    objectTemplate->Set(v8::String::New("appliedForceObjectA"), v8::FunctionTemplate::New(AppliedForceObjectA));
    objectTemplate->Set(v8::String::New("appliedTorqueObjectA"), v8::FunctionTemplate::New(AppliedTorqueObjectA));
    objectTemplate->Set(v8::String::New("appliedForceObjectB"), v8::FunctionTemplate::New(AppliedForceObjectB));
    objectTemplate->Set(v8::String::New("appliedTorqueObjectB"), v8::FunctionTemplate::New(AppliedTorqueObjectB));
    
    functionTemplate->Inherit(scriptBaseLink.functionTemplate);
}



void ScriptPointPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptVector3.isCorrectInstance(args[2]) && scriptVector3.isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btPoint2PointConstraint(*a->getBody(), *b->getBody(),
                                                                  scriptVector3.getDataOfInstance(args[2]),
                                                                  scriptVector3.getDataOfInstance(args[3])))) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("PointPhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("PointPhysicLink Constructor: Class can't be initialized");
}

void ScriptPointPhysicLink::AccessPointA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 point = scriptVector3.getDataOfInstance(args[0]);
        constraint->setPivotA(point);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(constraint->getPivotInA()));
}

void ScriptPointPhysicLink::AccessPointB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 point = scriptVector3.getDataOfInstance(args[0]);
        constraint->setPivotB(point);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(constraint->getPivotInB()));
}

ScriptPointPhysicLink::ScriptPointPhysicLink() :ScriptPhysicLink("PointPhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("pointA"), v8::FunctionTemplate::New(AccessPointA));
    objectTemplate->Set(v8::String::New("pointB"), v8::FunctionTemplate::New(AccessPointB));
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
}



void ScriptGearPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 5 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptVector3.isCorrectInstance(args[2]) && scriptVector3.isCorrectInstance(args[3]) &&
             args[4]->IsNumber()) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btGearConstraint(*a->getBody(), *b->getBody(),
                                                           scriptVector3.getDataOfInstance(args[2]),
                                                           scriptVector3.getDataOfInstance(args[3]),
                                                           args[4]->NumberValue()))) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("GearPhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("GearPhysicLink Constructor: Class can't be initialized");
}

void ScriptGearPhysicLink::AccessAxisA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 axis = scriptVector3.getDataOfInstance(args[0]);
        constraint->setAxisA(axis);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(constraint->getAxisA()));
}

void ScriptGearPhysicLink::AccessAxisB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        btVector3 axis = scriptVector3.getDataOfInstance(args[0]);
        constraint->setAxisB(axis);
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptVector3.newInstance(constraint->getAxisB()));
}

void ScriptGearPhysicLink::GetRatio(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getRatio());
}

void ScriptGearPhysicLink::SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setRatio(value->NumberValue());
}

ScriptGearPhysicLink::ScriptGearPhysicLink() :ScriptPhysicLink("GearPhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("axisA"), v8::FunctionTemplate::New(AccessAxisA));
    objectTemplate->Set(v8::String::New("axisB"), v8::FunctionTemplate::New(AccessAxisB));
    objectTemplate->SetAccessor(v8::String::New("ratio"), GetRatio, SetRatio);
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
}



void ScriptHingePhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptMatrix4.isCorrectInstance(args[2]) && scriptMatrix4.isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btHingeConstraint(*a->getBody(), *b->getBody(),
                                                            scriptMatrix4.getDataOfInstance(args[2])->getBTTransform(),
                                                            scriptMatrix4.getDataOfInstance(args[3])->getBTTransform(),
                                                            true))) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("HingePhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("HingePhysicLink Constructor: Class can't be initialized");
}

void ScriptHingePhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4.getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetA()));
}

void ScriptHingePhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4.getDataOfInstance(args[0])->getBTTransform());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetB()));
}

void ScriptHingePhysicLink::GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getHingeAngle());
}

void ScriptHingePhysicLink::GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getLowerLimit());
}

void ScriptHingePhysicLink::SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(value->NumberValue(), constraint->getUpperLimit());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getUpperLimit());
}

void ScriptHingePhysicLink::SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(constraint->getLowerLimit(), value->NumberValue());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getEnableAngularMotor());
}

void ScriptHingePhysicLink::SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(value->BooleanValue(),
                                   constraint->getMotorTargetVelosity(),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getMotorTargetVelosity());
}

void ScriptHingePhysicLink::SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   value->NumberValue(),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
}

void ScriptHingePhysicLink::GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getMaxMotorImpulse());
}

void ScriptHingePhysicLink::SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btHingeConstraint* constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   constraint->getMotorTargetVelosity(),
                                   value->NumberValue());
    activateConstraint(constraint);
}

ScriptHingePhysicLink::ScriptHingePhysicLink() :ScriptPhysicLink("HingePhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("frameA"), v8::FunctionTemplate::New(AccessFrameA));
    objectTemplate->Set(v8::String::New("frameB"), v8::FunctionTemplate::New(AccessFrameB));
    objectTemplate->SetAccessor(v8::String::New("hingeAngle"), GetHingeAngle);
    objectTemplate->SetAccessor(v8::String::New("angularLimitMin"), GetAngularLimitMin, SetAngularLimitMin);
    objectTemplate->SetAccessor(v8::String::New("angularLimitMax"), GetAngularLimitMax, SetAngularLimitMax);
    objectTemplate->SetAccessor(v8::String::New("angularMotorEnabled"), GetAngularMotorEnabled, SetAngularMotorEnabled);
    objectTemplate->SetAccessor(v8::String::New("angularMotorVelocity"), GetAngularMotorVelocity, SetAngularMotorVelocity);
    objectTemplate->SetAccessor(v8::String::New("angularMotorForce"), GetAngularMotorForce, SetAngularMotorForce);
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
}



void ScriptSliderPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptMatrix4.isCorrectInstance(args[2]) && scriptMatrix4.isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btSliderConstraint(*a->getBody(), *b->getBody(),
                                                             scriptMatrix4.getDataOfInstance(args[2])->getBTTransform(),
                                                             scriptMatrix4.getDataOfInstance(args[3])->getBTTransform(),
                                                             true))) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("SliderPhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("SliderPhysicLink Constructor: Class can't be initialized");
}

void ScriptSliderPhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4.getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetA()));
}

void ScriptSliderPhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4.getDataOfInstance(args[0])->getBTTransform());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetB()));
}

void ScriptSliderPhysicLink::GetHingeAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getAngularPos());
}

void ScriptSliderPhysicLink::GetSliderPos(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getLinearPos());
}

void ScriptSliderPhysicLink::GetAngularLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getLowerAngLimit());
}

void ScriptSliderPhysicLink::SetAngularLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLowerAngLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getUpperAngLimit());
}

void ScriptSliderPhysicLink::SetAngularLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setUpperAngLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getPoweredAngMotor());
}

void ScriptSliderPhysicLink::SetAngularMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setPoweredAngMotor(value->BooleanValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getTargetAngMotorVelocity());
}

void ScriptSliderPhysicLink::SetAngularMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setTargetAngMotorVelocity(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetAngularMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getMaxAngMotorForce());
}

void ScriptSliderPhysicLink::SetAngularMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setMaxAngMotorForce(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearLimitMin(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getLowerLinLimit());
}

void ScriptSliderPhysicLink::SetLinearLimitMin(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLowerLinLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearLimitMax(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getUpperLinLimit());
}

void ScriptSliderPhysicLink::SetLinearLimitMax(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setUpperLinLimit(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorEnabled(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getPoweredLinMotor());
}

void ScriptSliderPhysicLink::SetLinearMotorEnabled(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsBoolean()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setPoweredLinMotor(value->BooleanValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorVelocity(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getTargetLinMotorVelocity());
}

void ScriptSliderPhysicLink::SetLinearMotorVelocity(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setTargetLinMotorVelocity(value->NumberValue());
    activateConstraint(constraint);
}

void ScriptSliderPhysicLink::GetLinearMotorForce(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getMaxLinMotorForce());
}

void ScriptSliderPhysicLink::SetLinearMotorForce(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    btSliderConstraint* constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setMaxLinMotorForce(value->NumberValue());
    activateConstraint(constraint);
}

ScriptSliderPhysicLink::ScriptSliderPhysicLink() :ScriptPhysicLink("SliderPhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("frameA"), v8::FunctionTemplate::New(AccessFrameA));
    objectTemplate->Set(v8::String::New("frameB"), v8::FunctionTemplate::New(AccessFrameB));
    objectTemplate->SetAccessor(v8::String::New("hingeAngle"), GetHingeAngle);
    objectTemplate->SetAccessor(v8::String::New("sliderPos"), GetSliderPos);
    objectTemplate->SetAccessor(v8::String::New("angularLimitMin"), GetAngularLimitMin, SetAngularLimitMin);
    objectTemplate->SetAccessor(v8::String::New("angularLimitMax"), GetAngularLimitMax, SetAngularLimitMax);
    objectTemplate->SetAccessor(v8::String::New("angularMotorEnabled"), GetAngularMotorEnabled, SetAngularMotorEnabled);
    objectTemplate->SetAccessor(v8::String::New("angularMotorVelocity"), GetAngularMotorVelocity, SetAngularMotorVelocity);
    objectTemplate->SetAccessor(v8::String::New("angularMotorForce"), GetAngularMotorForce, SetAngularMotorForce);
    objectTemplate->SetAccessor(v8::String::New("linearLimitMin"), GetLinearLimitMin, SetLinearLimitMin);
    objectTemplate->SetAccessor(v8::String::New("linearLimitMax"), GetLinearLimitMax, SetLinearLimitMax);
    objectTemplate->SetAccessor(v8::String::New("linearMotorEnabled"), GetLinearMotorEnabled, SetLinearMotorEnabled);
    objectTemplate->SetAccessor(v8::String::New("linearMotorVelocity"), GetLinearMotorVelocity, SetLinearMotorVelocity);
    objectTemplate->SetAccessor(v8::String::New("linearMotorForce"), GetLinearMotorForce, SetLinearMotorForce);
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
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
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptMatrix4.isCorrectInstance(args[2]) && scriptMatrix4.isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        btTransform transA = scriptMatrix4.getDataOfInstance(args[2])->getBTTransform(),
                    transB = scriptMatrix4.getDataOfInstance(args[3])->getBTTransform();
        PhysicLink* linkPtr = new PhysicLink();
        btGeneric6DofConstraint* constraint = new btGeneric6DofConstraint(*a->getBody(), *b->getBody(), transA, transB, true);
        if(linkPtr->init(initializer, constraint)) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("Dof6PhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("Dof6PhysicLink Constructor: Class can't be initialized");
}

void ScriptDof6PhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4.getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetA()));
}

void ScriptDof6PhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4.getDataOfInstance(args[0])->getBTTransform());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetB()));
}

void ScriptDof6PhysicLink::AccessSpringStiffness(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
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
        args.GetReturnValue().Set(args[1]);
    }else if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
        args.GetReturnValue().Set(constraint->getStiffness(args[0]->IntegerValue()));
}

void ScriptDof6PhysicLink::AccessSpringDamping(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto link = getDataOfInstance<PhysicLink>(args.This());
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        auto constraint = EnableSpring(link);
        constraint->setDamping(args[0]->IntegerValue(), args[1]->NumberValue());
        args.GetReturnValue().Set(args[1]);
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE) return;
        args.GetReturnValue().Set(constraint->getDamping(args[0]->IntegerValue()));
    }
}

void ScriptDof6PhysicLink::AccessSpringEquilibrium(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto link = getDataOfInstance<PhysicLink>(args.This());
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return;
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        auto constraint = EnableSpring(link);
        constraint->setEquilibriumPoint(args[0]->IntegerValue(), args[1]->NumberValue());
        args.GetReturnValue().Set(args[1]);
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE) return;
        args.GetReturnValue().Set(constraint->getEquilibriumPoint(args[0]->IntegerValue()));
    }
}

void ScriptDof6PhysicLink::AccessMotorEnabled(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
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
        args.GetReturnValue().Set(args[1]);
    }else
        args.GetReturnValue().Set(*value);
}

void ScriptDof6PhysicLink::AccessMotorVelocity(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
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
        args.GetReturnValue().Set(args[1]);
    }else
        args.GetReturnValue().Set(*value);
}

void ScriptDof6PhysicLink::AccessMotorForce(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
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
        args.GetReturnValue().Set(args[1]);
    }else
        args.GetReturnValue().Set(*value);
}

void ScriptDof6PhysicLink::AccessAngularLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setAngularLowerLimit(scriptVector3.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else{
        btVector3 value;
        constraint->getAngularLowerLimit(value);
        args.GetReturnValue().Set(scriptVector3.newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessAngularLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setAngularUpperLimit(scriptVector3.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else{
        btVector3 value;
        constraint->getAngularUpperLimit(value);
        args.GetReturnValue().Set(scriptVector3.newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessLinearLimitMin(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setLinearLowerLimit(scriptVector3.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else{
        btVector3 value;
        constraint->getLinearLowerLimit(value);
        args.GetReturnValue().Set(scriptVector3.newInstance(value));
    }
}

void ScriptDof6PhysicLink::AccessLinearLimitMax(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setLinearUpperLimit(scriptVector3.getDataOfInstance(args[0]));
        args.GetReturnValue().Set(args[0]);
    }else{
        btVector3 value;
        constraint->getLinearUpperLimit(value);
        args.GetReturnValue().Set(scriptVector3.newInstance(value));
    }
}

ScriptDof6PhysicLink::ScriptDof6PhysicLink() :ScriptPhysicLink("Dof6PhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("frameA"), v8::FunctionTemplate::New(AccessFrameA));
    objectTemplate->Set(v8::String::New("frameB"), v8::FunctionTemplate::New(AccessFrameB));
    objectTemplate->Set(v8::String::New("springStiffness"), v8::FunctionTemplate::New(AccessSpringStiffness));
    objectTemplate->Set(v8::String::New("springDamping"), v8::FunctionTemplate::New(AccessSpringDamping));
    objectTemplate->Set(v8::String::New("springEquilibrium"), v8::FunctionTemplate::New(AccessSpringEquilibrium));
    objectTemplate->Set(v8::String::New("motorEnabled"), v8::FunctionTemplate::New(AccessMotorEnabled));
    objectTemplate->Set(v8::String::New("motorVelocity"), v8::FunctionTemplate::New(AccessMotorVelocity));
    objectTemplate->Set(v8::String::New("motorForce"), v8::FunctionTemplate::New(AccessMotorForce));
    objectTemplate->Set(v8::String::New("angularLimitMin"), v8::FunctionTemplate::New(AccessAngularLimitMin));
    objectTemplate->Set(v8::String::New("angularLimitMax"), v8::FunctionTemplate::New(AccessAngularLimitMax));
    objectTemplate->Set(v8::String::New("linearLimitMin"), v8::FunctionTemplate::New(AccessLinearLimitMin));
    objectTemplate->Set(v8::String::New("linearLimitMax"), v8::FunctionTemplate::New(AccessLinearLimitMax));
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
}



void ScriptConeTwistPhysicLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 4 &&
             scriptRigidObject.isCorrectInstance(args[0]) && scriptRigidObject.isCorrectInstance(args[1]) &&
             scriptMatrix4.isCorrectInstance(args[2]) && scriptMatrix4.isCorrectInstance(args[3])) {
        LinkInitializer initializer;
        RigidObject *a, *b;
        initializer.object[0] = a = scriptBaseObject.getDataOfInstance<RigidObject>(args[0]);
        initializer.object[1] = b = scriptBaseObject.getDataOfInstance<RigidObject>(args[1]);
        PhysicLink* linkPtr = new PhysicLink();
        if(linkPtr->init(initializer, new btConeTwistConstraint(*a->getBody(), *b->getBody(),
                                                             scriptMatrix4.getDataOfInstance(args[2])->getBTTransform(),
                                                             scriptMatrix4.getDataOfInstance(args[3])->getBTTransform()))) {
            linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("ConeTwistPhysicLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("ConeTwistPhysicLink Constructor: Class can't be initialized");
}

void ScriptConeTwistPhysicLink::AccessFrameA(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(scriptMatrix4.getDataOfInstance(args[0])->getBTTransform(), constraint->getFrameOffsetB());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetA()));
}

void ScriptConeTwistPhysicLink::AccessFrameB(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), scriptMatrix4.getDataOfInstance(args[0])->getBTTransform());
        args.GetReturnValue().Set(args[0]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(constraint->getFrameOffsetB()));
}

void ScriptConeTwistPhysicLink::GetSwingSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set((stdStrOfV8(property) == "spanA") ? constraint->getSwingSpan1() : constraint->getSwingSpan2());
}

void ScriptConeTwistPhysicLink::SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(stdStrOfV8(property) == "swingSpanA")
        constraint->setLimit(5, value->NumberValue());
    else
        constraint->setLimit(4, value->NumberValue());
    activateConstraint(constraint);
}

void ScriptConeTwistPhysicLink::GetTwistSpan(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getTwistSpan());
}

void ScriptConeTwistPhysicLink::SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    constraint->setLimit(3, value->NumberValue());
    activateConstraint(constraint);
}

void ScriptConeTwistPhysicLink::GetTwistAngle(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    args.GetReturnValue().Set(constraint->getTwistAngle());
}

ScriptConeTwistPhysicLink::ScriptConeTwistPhysicLink() :ScriptPhysicLink("ConeTwistPhysicLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("frameA"), v8::FunctionTemplate::New(AccessFrameA));
    objectTemplate->Set(v8::String::New("frameB"), v8::FunctionTemplate::New(AccessFrameB));
    objectTemplate->SetAccessor(v8::String::New("swingSpanA"), GetSwingSpan, SetSwingSpan);
    objectTemplate->SetAccessor(v8::String::New("swingSpanB"), GetSwingSpan, SetSwingSpan);
    objectTemplate->SetAccessor(v8::String::New("twistSpan"), GetTwistSpan, SetTwistSpan);
    objectTemplate->SetAccessor(v8::String::New("twistAngle"), GetTwistAngle);
    
    functionTemplate->Inherit(scriptPhysicLink.functionTemplate);
}



void ScriptTransformLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        TransformLink* linkPtr = static_cast<TransformLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() > 2 &&
             scriptBaseObject.isCorrectInstance(args[0]) && scriptBaseObject.isCorrectInstance(args[1])) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject.getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject.getDataOfInstance<BaseObject>(args[1]);
        TransformLink* linkPtr = new TransformLink();
        std::vector<btTransform> transformations;
        for(unsigned int i = 2; i < args.Length(); i ++) {
            if(!scriptMatrix4.isCorrectInstance(args[i]))
                return args.ScriptException("TransformLink Constructor: Invalid argument");
            transformations.push_back(scriptMatrix4.getDataOfInstance(args[i])->getBTTransform());
        }
        if(linkPtr->init(initializer, transformations)) {
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("TransformLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("TransformLink Constructor: Class can't be initialized");
}

void ScriptTransformLink::AccessTransformation(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    TransformLink* linkPtr = getDataOfInstance<TransformLink>(args.This());
    if(args.Length() == 0)
        return args.ScriptException("TransformLink transformation(): Too few arguments");
    if(!args[0]->IsNumber() || args[0]->Uint32Value() >= linkPtr->transformations.size())
        return args.ScriptException("TransformLink transformation(): Invalid argument");
    btTransform& transform = linkPtr->transformations[args[0]->Uint32Value()];
    if(args.Length() == 2 && scriptMatrix4.isCorrectInstance(args[1])) {
        Matrix4* mat = scriptMatrix4.getDataOfInstance(args[1]);
        if(mat->isValid())
            transform = mat->getBTTransform();
        args.GetReturnValue().Set(args[1]);
    }else
        args.GetReturnValue().Set(scriptMatrix4.newInstance(Matrix4(transform)));
}

ScriptTransformLink::ScriptTransformLink() :ScriptBaseLink("TransformLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("transformation"), v8::FunctionTemplate::New(AccessTransformation));
    
    functionTemplate->Inherit(scriptBaseLink.functionTemplate);
}



void ScriptBoneLink::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    
    if(args.Length() == 1 && args[0]->IsExternal()) {
        BoneLink* linkPtr = static_cast<BoneLink*>(v8::Local<v8::External>::Cast(args[0])->Value());
        linkPtr->scriptInstance = v8::Persistent<v8::Object>::New(v8::Isolate::GetCurrent(), args.This());
        args.This()->SetInternalField(0, args[0]);
        args.GetReturnValue().Set(args.This());
        return;
    }else if(args.Length() == 3 &&
             scriptBaseObject.isCorrectInstance(args[0]) && scriptBaseObject.isCorrectInstance(args[1]) &&
             args[2]->IsString()) {
        LinkInitializer initializer;
        initializer.object[0] = scriptBaseObject.getDataOfInstance<BaseObject>(args[0]);
        initializer.object[1] = scriptBaseObject.getDataOfInstance<BaseObject>(args[1]);
        BoneLink* linkPtr = new BoneLink();
        if(linkPtr->init(initializer, linkPtr->getBoneByName(cStrOfV8(args[2])))) {
            args.This()->SetInternalField(0, v8::External::New(linkPtr));
            args.GetReturnValue().Set(args.This());
            return;
        }else
            return args.ScriptException("BoneLink Constructor: Invalid argument");
    }
    
    return args.ScriptException("BoneLink Constructor: Class can't be initialized");
}

void ScriptBoneLink::GetBone(v8::Local<v8::String> property, const v8::PropertyCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    args.GetReturnValue().Set(v8::String::New(linkPtr->bone->name.c_str()));
}

void ScriptBoneLink::SetBone(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::PropertyCallbackInfo<void>& args) {
    v8::HandleScope handleScope;
    if(!value->IsString()) return;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    Bone* bone = linkPtr->getBoneByName(cStrOfV8(value));
    if(bone) linkPtr->bone = bone;
}

void ScriptBoneLink::GetBoneChildren(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(linkPtr->bone->children.size());
    for(auto child : linkPtr->bone->children)
        objects->Set(i ++, v8::String::New(child->name.c_str()));
    args.GetReturnValue().Set(objects);
}

void ScriptBoneLink::GetRelativeMat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    args.GetReturnValue().Set(scriptMatrix4.newInstance(Matrix4(linkPtr->bone->relativeMat)));
}

void ScriptBoneLink::GetAbsoluteMat(const v8::FunctionCallbackInfo<v8::Value>& args) {
    v8::HandleScope handleScope;
    BoneLink* linkPtr = getDataOfInstance<BoneLink>(args.This());
    args.GetReturnValue().Set(scriptMatrix4.newInstance(Matrix4(linkPtr->bone->absoluteInv)));
}

ScriptBoneLink::ScriptBoneLink() :ScriptBaseLink("BoneLink", Constructor) {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("bone"), GetBone, SetBone);
    objectTemplate->Set(v8::String::New("getBoneChildren"), v8::FunctionTemplate::New(GetBoneChildren));
    objectTemplate->Set(v8::String::New("getRelativeMat"), v8::FunctionTemplate::New(GetRelativeMat));
    objectTemplate->Set(v8::String::New("getAbsoluteMat"), v8::FunctionTemplate::New(GetAbsoluteMat));
    
    functionTemplate->Inherit(scriptBaseLink.functionTemplate);
}