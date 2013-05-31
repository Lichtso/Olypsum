//
//  ScriptLinks.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//
//

#include "ScriptLinks.h"

v8::Handle<v8::Value> ScriptBaseLink::GetObjectA(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(info.This());
    return handleScope.Close(linkPtr->a->scriptInstance);
}

v8::Handle<v8::Value> ScriptBaseLink::GetObjectB(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    BaseLink* linkPtr = getDataOfInstance<BaseLink>(info.This());
    return handleScope.Close(linkPtr->b->scriptInstance);
}

ScriptBaseLink::ScriptBaseLink() :ScriptBaseLink("BaseLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("objectA"), GetObjectA);
    objectTemplate->SetAccessor(v8::String::New("objectB"), GetObjectB);
}



v8::Handle<v8::Value> ScriptPhysicLink::GetBurstImpulse(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(info.This())->constraint;
    return v8::Number::New(constraint->getBreakingImpulseThreshold());
}

void ScriptPhysicLink::SetBurstImpulse(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(info.This())->constraint;
    constraint->setBreakingImpulseThreshold(value->NumberValue());
}

v8::Handle<v8::Value> ScriptPhysicLink::GetFrame(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    bool isA = (stdStrOfV8(property) == "frameA");
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(info.This())->constraint;
    switch(constraint->getConstraintType()) {
        case HINGE_CONSTRAINT_TYPE: {
            btHingeConstraint* hinge = static_cast<btHingeConstraint*>(constraint);
            return scriptMatrix4.newInstance((isA) ? hinge->getFrameOffsetA() : hinge->getFrameOffsetB());
        }
        case SLIDER_CONSTRAINT_TYPE: {
            btSliderConstraint* slider = static_cast<btSliderConstraint*>(constraint);
            return scriptMatrix4.newInstance((isA) ? slider->getFrameOffsetA() : slider->getFrameOffsetB());
        }
        case D6_CONSTRAINT_TYPE:
        case D6_SPRING_CONSTRAINT_TYPE: {
            btGeneric6DofConstraint* dof6 = static_cast<btGeneric6DofConstraint*>(constraint);
            return scriptMatrix4.newInstance((isA) ? dof6->getFrameOffsetA() : dof6->getFrameOffsetB());
        }
        case CONETWIST_CONSTRAINT_TYPE: {
            btConeTwistConstraint* coneTwist = static_cast<btConeTwistConstraint*>(constraint);
            return scriptMatrix4.newInstance((isA) ? coneTwist->getFrameOffsetA() : coneTwist->getFrameOffsetB());
        }
        default:
            return v8::Undefined();
    }
}

void ScriptPhysicLink::SetFrame(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptMatrix4.isCorrectInstance(value)) return;
    
    bool isA = (stdStrOfV8(property) == "frameA");
    btTransform frame = scriptMatrix4.getDataOfInstance(value)->getBTTransform();
    btTypedConstraint* constraint = getDataOfInstance<PhysicLink>(info.This())->constraint;
    switch(constraint->getConstraintType()) {
        case HINGE_CONSTRAINT_TYPE: {
            btHingeConstraint* hinge = static_cast<btHingeConstraint*>(constraint);
            if(isA)
                hinge->setFrames(frame, hinge->getFrameOffsetB());
            else
                hinge->setFrames(hinge->getFrameOffsetB(), frame);
        }
        case SLIDER_CONSTRAINT_TYPE: {
            btSliderConstraint* slider = static_cast<btSliderConstraint*>(constraint);
            if(isA)
                slider->setFrames(frame, slider->getFrameOffsetB());
            else
                slider->setFrames(slider->getFrameOffsetB(), frame);
        }
        case D6_CONSTRAINT_TYPE:
        case D6_SPRING_CONSTRAINT_TYPE: {
            btGeneric6DofConstraint* dof6 = static_cast<btGeneric6DofConstraint*>(constraint);
            if(isA)
                dof6->setFrames(frame, dof6->getFrameOffsetB());
            else
                dof6->setFrames(dof6->getFrameOffsetB(), frame);
        }
        case CONETWIST_CONSTRAINT_TYPE: {
            btConeTwistConstraint* coneTwist = static_cast<btConeTwistConstraint*>(constraint);
            if(isA)
                coneTwist->setFrames(frame, coneTwist->getFrameOffsetB());
            else
                coneTwist->setFrames(coneTwist->getFrameOffsetB(), frame);
        }
        default:
            return;
    }
}

ScriptPhysicLink::ScriptPhysicLink() :ScriptBaseLink("PhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("burstImpulse"), GetBurstImpulse, SetBurstImpulse);
}



v8::Handle<v8::Value> ScriptPointPhysicLink::GetPoint(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return scriptVector3.newInstance((stdStrOfV8(property) == "pointA") ? constraint->getPivotInA() : constraint->getPivotInB());
}

void ScriptPointPhysicLink::SetPoint(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value)) return;
    btVector3 point = scriptVector3.getDataOfInstance(value);
    
    btPoint2PointConstraint* constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    if(stdStrOfV8(property) == "pointA")
        constraint->setPivotA(point);
    else
        constraint->setPivotB(point);
}

ScriptPointPhysicLink::ScriptPointPhysicLink() :ScriptPhysicLink("PointPhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("pointA"), GetPoint, SetPoint);
    objectTemplate->SetAccessor(v8::String::New("pointB"), GetPoint, SetPoint);
}



v8::Handle<v8::Value> ScriptGearPhysicLink::GetAxis(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return scriptVector3.newInstance((stdStrOfV8(property) == "axisA") ? constraint->getAxisA() : constraint->getAxisB());
}

void ScriptGearPhysicLink::SetAxis(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!scriptVector3.isCorrectInstance(value)) return;
    btVector3 axis = scriptVector3.getDataOfInstance(value);
    
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    if(stdStrOfV8(property) == "axisA")
        constraint->setAxisA(axis);
    else
        constraint->setAxisB(axis);
}

v8::Handle<v8::Value> ScriptGearPhysicLink::GetRatio(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return v8::Number::New(constraint->getRatio());
}

void ScriptGearPhysicLink::SetRatio(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btGearConstraint* constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    constraint->setRatio(value->NumberValue());
}

ScriptGearPhysicLink::ScriptGearPhysicLink() :ScriptPhysicLink("GearPhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("axisA"), GetAxis, SetAxis);
    objectTemplate->SetAccessor(v8::String::New("axisB"), GetAxis, SetAxis);
    objectTemplate->SetAccessor(v8::String::New("ratio"), GetRatio, SetRatio);
}



ScriptHingePhysicLink::ScriptHingePhysicLink() :ScriptPhysicLink("HingePhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("frameA"), GetFrame, SetFrame);
    objectTemplate->SetAccessor(v8::String::New("frameB"), GetFrame, SetFrame);
}



ScriptSliderPhysicLink::ScriptSliderPhysicLink() :ScriptPhysicLink("SliderPhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("frameA"), GetFrame, SetFrame);
    objectTemplate->SetAccessor(v8::String::New("frameB"), GetFrame, SetFrame);
}



v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessSpringStiffness(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5 || constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE)
        return v8::Undefined();
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        constraint->setStiffness(args[0]->IntegerValue(), args[1]->NumberValue());
        return args[1];
    }else
        return v8::Number::New(constraint->getStiffness(args[0]->IntegerValue()));
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessSpringDamping(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5 || constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE)
        return v8::Undefined();
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        constraint->setDamping(args[0]->IntegerValue(), args[1]->NumberValue());
        return args[1];
    }else
        return v8::Number::New(constraint->getDamping(args[0]->IntegerValue()));
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessSpringEquilibrium(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofSpringConstraint* constraint = static_cast<btGeneric6DofSpringConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5 || constraint->getConstraintType() != D6_SPRING_CONSTRAINT_TYPE)
        return v8::Undefined();
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        constraint->setEquilibriumPoint(args[0]->IntegerValue(), args[1]->NumberValue());
        return args[1];
    }else
        return v8::Number::New(constraint->getEquilibriumPoint(args[0]->IntegerValue()));
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessAngularLimitMin(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        constraint->setAngularLowerLimit(scriptVector3.getDataOfInstance(args[0]));
        return args[0];
    }else{
        btVector3 value;
        constraint->getAngularLowerLimit(value);
        return handleScope.Close(scriptVector3.newInstance(value));
    }
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessMotorEnabled(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return v8::Undefined();
    
    bool* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_enableMotor[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_enableMotor;
    }
    
    if(args.Length() > 1 && !args[1]->IsBoolean()) {
        *value = args[1]->BooleanValue();
        return args[1];
    }else
        return v8::Boolean::New(value);
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessMotorVelocity(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return v8::Undefined();
    
    float* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_targetVelocity[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_targetVelocity;
    }
    
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        *value = args[1]->NumberValue();
        return args[1];
    }else
        return v8::Boolean::New(value);
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessMotorForce(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 0 || !args[0]->IsUint32() || args[0]->IntegerValue() > 5)
        return v8::Undefined();
    
    float* value;
    if(args[0]->IntegerValue() < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_maxMotorForce[args[0]->IntegerValue()];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(args[0]->IntegerValue()-3);
        value = &angularMotor->m_maxMotorForce;
    }
    
    if(args.Length() > 1 && !args[1]->IsNumber()) {
        *value = args[1]->NumberValue();
        return args[1];
    }else
        return v8::Boolean::New(value);
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessAngularLimitMax(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        constraint->setAngularUpperLimit(scriptVector3.getDataOfInstance(args[0]));
        return args[0];
    }else{
        btVector3 value;
        constraint->getAngularUpperLimit(value);
        return handleScope.Close(scriptVector3.newInstance(value));
    }
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessLinearLimitMin(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        constraint->setLinearLowerLimit(scriptVector3.getDataOfInstance(args[0]));
        return args[0];
    }else{
        btVector3 value;
        constraint->getLinearLowerLimit(value);
        return handleScope.Close(scriptVector3.newInstance(value));
    }
}

v8::Handle<v8::Value> ScriptDof6PhysicLink::AccessLinearLimitMax(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    btGeneric6DofConstraint* constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(args.This())->constraint);
    if(args.Length() == 1 && scriptVector3.isCorrectInstance(args[0])) {
        constraint->setLinearUpperLimit(scriptVector3.getDataOfInstance(args[0]));
        return args[0];
    }else{
        btVector3 value;
        constraint->getLinearUpperLimit(value);
        return handleScope.Close(scriptVector3.newInstance(value));
    }
}

ScriptDof6PhysicLink::ScriptDof6PhysicLink() :ScriptPhysicLink("Dof6PhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("frameA"), GetFrame, SetFrame);
    objectTemplate->SetAccessor(v8::String::New("frameB"), GetFrame, SetFrame);
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
}



v8::Handle<v8::Value> ScriptConeTwistPhysicLink::GetSwingSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return v8::Number::New((stdStrOfV8(property) == "spanA") ? constraint->getSwingSpan1() : constraint->getSwingSpan2());
}

void ScriptConeTwistPhysicLink::SetSwingSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    if(stdStrOfV8(property) == "swingSpanA")
        constraint->setLimit(5, value->NumberValue());
    else
        constraint->setLimit(4, value->NumberValue());
}

v8::Handle<v8::Value> ScriptConeTwistPhysicLink::GetTwistSpan(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return v8::Number::New(constraint->getTwistSpan());
}

void ScriptConeTwistPhysicLink::SetTwistSpan(v8::Local<v8::String> property, v8::Local<v8::Value> value, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    if(!value->IsNumber()) return;
    
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    constraint->setLimit(3, value->NumberValue());
}

v8::Handle<v8::Value> ScriptConeTwistPhysicLink::GetTwistAngle(v8::Local<v8::String> property, const v8::AccessorInfo& info) {
    v8::HandleScope handleScope;
    btConeTwistConstraint* constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(info.This())->constraint);
    return v8::Number::New(constraint->getTwistAngle());
}

ScriptConeTwistPhysicLink::ScriptConeTwistPhysicLink() :ScriptPhysicLink("ConeTwistPhysicLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->SetAccessor(v8::String::New("frameA"), GetFrame, SetFrame);
    objectTemplate->SetAccessor(v8::String::New("frameB"), GetFrame, SetFrame);
    objectTemplate->SetAccessor(v8::String::New("swingSpanA"), GetSwingSpan, SetSwingSpan);
    objectTemplate->SetAccessor(v8::String::New("swingSpanB"), GetSwingSpan, SetSwingSpan);
    objectTemplate->SetAccessor(v8::String::New("twistSpan"), GetTwistSpan, SetTwistSpan);
    objectTemplate->SetAccessor(v8::String::New("twistAngle"), GetTwistAngle);
}



v8::Handle<v8::Value> ScriptTransformLink::AccessTransformation(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    TransformLink* linkPtr = getDataOfInstance<TransformLink>(args.This());
    if(args.Length() == 1 && scriptMatrix4.isCorrectInstance(args[0])) {
        Matrix4* mat = scriptMatrix4.getDataOfInstance(args[0]);
        if(mat->isValid())
            linkPtr->transform = mat->getBTTransform();
        return args[0];
    }else
        return handleScope.Close(scriptMatrix4.newInstance(Matrix4(linkPtr->transform)));
}

ScriptTransformLink::ScriptTransformLink() :ScriptTransformLink("TransformLink") {
    v8::HandleScope handleScope;
    
    v8::Local<v8::ObjectTemplate> objectTemplate = functionTemplate->PrototypeTemplate();
    objectTemplate->Set(v8::String::New("transformation"), v8::FunctionTemplate::New(AccessTransformation));
}



ScriptBaseLink scriptBaseLink;
ScriptPhysicLink scriptPhysicLink;
ScriptPointPhysicLink scriptPointPhysicLink;
ScriptGearPhysicLink scriptGearPhysicLink;
ScriptHingePhysicLink scriptHingePhysicLink;
ScriptSliderPhysicLink scriptSliderPhysicLink;
ScriptDof6PhysicLink scriptDof6PhysicLink;
ScriptConeTwistPhysicLink scriptConeTwistPhysicLink;
ScriptTransformLink scriptTransformLink;