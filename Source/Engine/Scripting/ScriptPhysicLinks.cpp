//
//  ScriptLinks.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 29.05.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static void activateConstraint(btTypedConstraint* constraint) {
    constraint->getRigidBodyA().activate();
    constraint->getRigidBodyB().activate();
}



static JSObjectRef ScriptPhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "PhysicLink Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(PhysicLink);

static JSValueRef ScriptPhysicLinkGetBurstImpulse(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<PhysicLink>(instance)->constraint->getBreakingImpulseThreshold());
}

static bool ScriptPhysicLinkSetBurstImpulse(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "PhysicLink setBurstImpulse(): Expected Number");
        return false;
    }
    getDataOfInstance<PhysicLink>(instance)->constraint->setBreakingImpulseThreshold(JSValueToNumber(context, value, NULL));
    return true;
}

static JSValueRef ScriptPhysicLinkGetCollisionDisabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<PhysicLink>(instance)->isCollisionDisabled());
}

static bool ScriptPhysicLinkSetCollisionDisabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "PhysicLink setCollisionDisabled(): Expected Boolean");
        return false;
    }
    getDataOfInstance<PhysicLink>(instance)->setCollisionDisabled(JSValueToBoolean(context, value));
    return true;
}

static JSValueRef ScriptPhysicObjectAppliedForceObjectA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return newScriptVector3(context, getDataOfInstance<PhysicLink>(instance)->constraint->m_appliedForceBodyA);
}

static JSValueRef ScriptPhysicObjectAppliedForceObjectB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return newScriptVector3(context, getDataOfInstance<PhysicLink>(instance)->constraint->m_appliedForceBodyB);
}

static JSValueRef ScriptPhysicObjectAppliedTorqueObjectA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return newScriptVector3(context, getDataOfInstance<PhysicLink>(instance)->constraint->m_appliedTorqueBodyA);
}

static JSValueRef ScriptPhysicObjectAppliedTorqueObjectB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return newScriptVector3(context, getDataOfInstance<PhysicLink>(instance)->constraint->m_appliedTorqueBodyB);
}

JSStaticValue ScriptPhysicLinkProperties[] = {
    {"burstImpulse", ScriptPhysicLinkGetBurstImpulse, ScriptPhysicLinkSetBurstImpulse, kJSPropertyAttributeDontDelete},
    {"collisionDisabled", ScriptPhysicLinkGetCollisionDisabled, ScriptPhysicLinkSetCollisionDisabled, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptPhysicLinkMethods[] = {
    {"appliedForceObjectA", ScriptPhysicObjectAppliedForceObjectA, ScriptMethodAttributes},
    {"appliedForceObjectB", ScriptPhysicObjectAppliedForceObjectB, ScriptMethodAttributes},
    {"appliedTorqueObjectA", ScriptPhysicObjectAppliedTorqueObjectA, ScriptMethodAttributes},
    {"appliedTorqueObjectB", ScriptPhysicObjectAppliedTorqueObjectB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(PhysicLink, ScriptPhysicLinkProperties, ScriptPhysicLinkMethods);



static JSObjectRef ScriptConeTwistPhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "ConeTwistPhysicLink Constructor: Expected RigidObject, RigidObject, Matrix4, Matrix4");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btTransform transA = getDataOfInstance<Matrix4>(context, argv[2])->getBTTransform(),
                transB = getDataOfInstance<Matrix4>(context, argv[3])->getBTTransform();
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btConeTwistConstraint(*a->getBody(), *b->getBody(), transA, transB);
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptConeTwistPhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "ConeTwistPhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(ConeTwistPhysicLink);

static JSValueRef ScriptConeTwistPhysicLinkGetSwingSpanA(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getSwingSpan1());
}

static bool ScriptConeTwistPhysicLinkSetSwingSpanA(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ConeTwistPhysicLink setSwingSpanA(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLimit(5, JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptConeTwistPhysicLinkGetSwingSpanB(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getSwingSpan2());
}

static bool ScriptConeTwistPhysicLinkSetSwingSpanB(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ConeTwistPhysicLink setSwingSpanB(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLimit(4, JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptConeTwistPhysicLinkGetTwistSpan(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getTwistSpan());
}

static bool ScriptConeTwistPhysicLinkSetTwistSpan(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ConeTwistPhysicLink setTwistSpan(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLimit(3, JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptConeTwistPhysicLinkGetTwistAngle(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getTwistAngle());
}

static JSValueRef ScriptConeTwistPhysicLinkAccessFrameA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform(), constraint->getFrameOffsetB());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetA());
}

static JSValueRef ScriptConeTwistPhysicLinkAccessFrameB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btConeTwistConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetB());
}

JSStaticValue ScriptConeTwistPhysicLinkProperties[] = {
    {"swingSpanA", ScriptConeTwistPhysicLinkGetSwingSpanA, ScriptConeTwistPhysicLinkSetSwingSpanA, kJSPropertyAttributeDontDelete},
    {"swingSpanB", ScriptConeTwistPhysicLinkGetSwingSpanB, ScriptConeTwistPhysicLinkSetSwingSpanB, kJSPropertyAttributeDontDelete},
    {"twistSpan", ScriptConeTwistPhysicLinkGetTwistSpan, ScriptConeTwistPhysicLinkSetTwistSpan, kJSPropertyAttributeDontDelete},
    {"twistAngle", ScriptConeTwistPhysicLinkGetTwistAngle, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptConeTwistPhysicLinkMethods[] = {
    {"frameA", ScriptConeTwistPhysicLinkAccessFrameA, ScriptMethodAttributes},
    {"frameB", ScriptConeTwistPhysicLinkAccessFrameB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(ConeTwistPhysicLink, ScriptConeTwistPhysicLinkProperties, ScriptConeTwistPhysicLinkMethods);



static btGeneric6DofSpringConstraint* ScriptDof6PhysicLinkEnableSpring(PhysicLink* link) {
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

static btGeneric6DofSpringConstraint* ScriptDof6PhysicLinkDisableSpring(PhysicLink* link) {
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

static JSObjectRef ScriptDof6PhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "Dof6PhysicLink Constructor: Expected RigidObject, RigidObject, Matrix4, Matrix4");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btTransform transA = getDataOfInstance<Matrix4>(context, argv[2])->getBTTransform(),
                transB = getDataOfInstance<Matrix4>(context, argv[3])->getBTTransform();
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btGeneric6DofConstraint(*a->getBody(), *b->getBody(), transA, transB, true);
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptDof6PhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "Dof6PhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(Dof6PhysicLink);

static JSValueRef ScriptDof6PhysicLinkAccessFrameA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform(), constraint->getFrameOffsetB());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetA());
}

static JSValueRef ScriptDof6PhysicLinkAccessFrameB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetB());
}

static JSValueRef ScriptDof6PhysicLinkAccessSpringStiffness(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink springStiffness(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink springStiffness(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        btScalar stiffness = JSValueToNumber(context, argv[1], NULL);
        if(stiffness > 0.0) {
            constraint = ScriptDof6PhysicLinkEnableSpring(link);
            constraint->enableSpring(index, true);
            return argv[1];
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
                constraint = ScriptDof6PhysicLinkDisableSpring(link);
            activateConstraint(constraint);
        }
    }else if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
        return JSValueMakeNumber(context, constraint->getStiffness(index));
    return JSValueMakeNumber(context, 0.0);
}

static JSValueRef ScriptDof6PhysicLinkAccessSpringDamping(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink springDamping(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink springDamping(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        auto constraint = ScriptDof6PhysicLinkEnableSpring(link);
        constraint->setDamping(index, JSValueToNumber(context, argv[1], NULL));
        return argv[1];
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
            return JSValueMakeNumber(context, constraint->getDamping(index));
    }
    return JSValueMakeNumber(context, 0.0);
}

static JSValueRef ScriptDof6PhysicLinkAccessSpringEquilibrium(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink springEquilibrium(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink springEquilibrium(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        auto constraint = ScriptDof6PhysicLinkEnableSpring(link);
        constraint->setEquilibriumPoint(index, JSValueToNumber(context, argv[1], NULL));
        return argv[1];
    }else{
        auto constraint = static_cast<btGeneric6DofSpringConstraint*>(link->constraint);
        if(constraint->getConstraintType() == D6_SPRING_CONSTRAINT_TYPE)
            return JSValueMakeNumber(context, constraint->getEquilibriumPoint(index));
    }
    return JSValueMakeNumber(context, 0.0);
}

static JSValueRef ScriptDof6PhysicLinkAccessMotorEnabled(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink motorEnabled(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink motorEnabled(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    auto constraint = static_cast<btGeneric6DofConstraint*>(link->constraint);
    bool* value;
    if(index < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_enableMotor[index];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(index-3);
        value = &angularMotor->m_enableMotor;
    }
    if(argc == 2 && JSValueIsBoolean(context, argv[1])) {
        activateConstraint(constraint);
        *value = JSValueToBoolean(context, argv[1]);
        return argv[1];
    }else
        return JSValueMakeBoolean(context, *value);
}

static JSValueRef ScriptDof6PhysicLinkAccessMotorVelocity(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink motorVelocity(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink motorVelocity(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    auto constraint = static_cast<btGeneric6DofConstraint*>(link->constraint);
    btScalar* value;
    if(index < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_targetVelocity[index];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(index-3);
        value = &angularMotor->m_targetVelocity;
    }
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        activateConstraint(constraint);
        *value = JSValueToNumber(context, argv[1], NULL);
        return argv[1];
    }else
        return JSValueMakeNumber(context, *value);
}

static JSValueRef ScriptDof6PhysicLinkAccessMotorForce(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc == 0 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "Dof6PhysicLink motorForce(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    if(index > 5)
        return ScriptException(context, exception, "Dof6PhysicLink motorForce(): Out of bounds");
    auto link = getDataOfInstance<PhysicLink>(instance);
    auto constraint = static_cast<btGeneric6DofConstraint*>(link->constraint);
    btScalar* value;
    if(index < 3) {
        btTranslationalLimitMotor* linearMotor = constraint->getTranslationalLimitMotor();
        value = &linearMotor->m_maxMotorForce[index];
    }else{
        btRotationalLimitMotor* angularMotor = constraint->getRotationalLimitMotor(index-3);
        value = &angularMotor->m_maxMotorForce;
    }
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        activateConstraint(constraint);
        *value = JSValueToNumber(context, argv[1], NULL);
        return argv[1];
    }else
        return JSValueMakeNumber(context, *value);
}

static JSValueRef ScriptDof6PhysicLinkAccessAngularLimitMin(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        activateConstraint(constraint);
        constraint->setAngularLowerLimit(getScriptVector3(context, argv[0]));
        return argv[0];
    }else{
        btVector3 value;
        constraint->getAngularLowerLimit(value);
        return newScriptVector3(context, value);
    }
}

static JSValueRef ScriptDof6PhysicLinkAccessAngularLimitMax(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        activateConstraint(constraint);
        constraint->setAngularUpperLimit(getScriptVector3(context, argv[0]));
        return argv[0];
    }else{
        btVector3 value;
        constraint->getAngularUpperLimit(value);
        return newScriptVector3(context, value);
    }
}

static JSValueRef ScriptDof6PhysicLinkAccessLinearLimitMin(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        activateConstraint(constraint);
        constraint->setLinearLowerLimit(getScriptVector3(context, argv[0]));
        return argv[0];
    }else{
        btVector3 value;
        constraint->getLinearLowerLimit(value);
        return newScriptVector3(context, value);
    }
}

static JSValueRef ScriptDof6PhysicLinkAccessLinearLimitMax(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGeneric6DofConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        activateConstraint(constraint);
        constraint->setLinearUpperLimit(getScriptVector3(context, argv[0]));
        return argv[0];
    }else{
        btVector3 value;
        constraint->getLinearUpperLimit(value);
        return newScriptVector3(context, value);
    }
}

JSStaticFunction ScriptDof6PhysicLinkMethods[] = {
    {"frameA", ScriptDof6PhysicLinkAccessFrameA, ScriptMethodAttributes},
    {"frameB", ScriptDof6PhysicLinkAccessFrameB, ScriptMethodAttributes},
    {"springStiffness", ScriptDof6PhysicLinkAccessSpringStiffness, ScriptMethodAttributes},
    {"springDamping", ScriptDof6PhysicLinkAccessSpringDamping, ScriptMethodAttributes},
    {"springEquilibrium", ScriptDof6PhysicLinkAccessSpringEquilibrium, ScriptMethodAttributes},
    {"motorEnabled", ScriptDof6PhysicLinkAccessMotorEnabled, ScriptMethodAttributes},
    {"motorVelocity", ScriptDof6PhysicLinkAccessMotorVelocity, ScriptMethodAttributes},
    {"motorForce", ScriptDof6PhysicLinkAccessMotorForce, ScriptMethodAttributes},
    {"angularLimitMin", ScriptDof6PhysicLinkAccessAngularLimitMin, ScriptMethodAttributes},
    {"angularLimitMax", ScriptDof6PhysicLinkAccessAngularLimitMax, ScriptMethodAttributes},
    {"linearLimitMin", ScriptDof6PhysicLinkAccessLinearLimitMin, ScriptMethodAttributes},
    {"linearLimitMax", ScriptDof6PhysicLinkAccessLinearLimitMax, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Dof6PhysicLink, NULL, ScriptDof6PhysicLinkMethods);



static JSObjectRef ScriptGearPhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 5 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsNumber(context, argv[4]))
        return ScriptException(context, exception, "GearPhysicLink Constructor: Expected RigidObject, RigidObject, Vector3, Vector3, Number");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btVector3 axisA = getScriptVector3(context, argv[2]),
              axisB = getScriptVector3(context, argv[3]);
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btGearConstraint(*a->getBody(), *b->getBody(), axisA, axisB, JSValueToNumber(context, argv[4], NULL));
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptGearPhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "GearPhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(GearPhysicLink);

static JSValueRef ScriptGearPhysicLinkGetRatio(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getRatio());
}

static bool ScriptGearPhysicLinkSetRatio(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "GearPhysicLink setRatio(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setRatio(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptGearPhysicLinkAccessAxisA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        constraint->setAxisA(vec);
        activateConstraint(constraint);
        return argv[0];
    }else
        return newScriptVector3(context, constraint->getAxisA());
}

static JSValueRef ScriptGearPhysicLinkAccessAxisB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btGearConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        constraint->setAxisB(vec);
        activateConstraint(constraint);
        return argv[0];
    }else
        return newScriptVector3(context, constraint->getAxisB());
}

JSStaticValue ScriptGearPhysicLinkProperties[] = {
    {"ratio", ScriptGearPhysicLinkGetRatio, ScriptGearPhysicLinkSetRatio, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptGearPhysicLinkMethods[] = {
    {"axisA", ScriptGearPhysicLinkAccessAxisA, ScriptMethodAttributes},
    {"axisB", ScriptGearPhysicLinkAccessAxisB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(GearPhysicLink, ScriptGearPhysicLinkProperties, ScriptGearPhysicLinkMethods);



static JSObjectRef ScriptHingePhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "HingePhysicLink Constructor: Expected RigidObject, RigidObject, Matrix4, Matrix4");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btTransform transA = getDataOfInstance<Matrix4>(context, argv[2])->getBTTransform(),
    transB = getDataOfInstance<Matrix4>(context, argv[3])->getBTTransform();
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btHingeConstraint(*a->getBody(), *b->getBody(), transA, transB, true);
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptHingePhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "HingePhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(HingePhysicLink);

static JSValueRef ScriptHingePhysicLinkGetAngularPos(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getHingeAngle());
}

static JSValueRef ScriptHingePhysicLinkGetAngularLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getLowerLimit());
}

static bool ScriptHingePhysicLinkSetAngularLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "HingePhysicLink setAngularLimitMin(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLimit(JSValueToNumber(context, value, NULL), constraint->getUpperLimit());
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptHingePhysicLinkGetAngularLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getUpperLimit());
}

static bool ScriptHingePhysicLinkSetAngularLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "HingePhysicLink setAngularLimitMax(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLimit(constraint->getLowerLimit(), JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptHingePhysicLinkGetAngularMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getEnableAngularMotor());
}

static bool ScriptHingePhysicLinkSetAngularMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "HingePhysicLink setAngularMotorEnabled(): Expected Boolean");
        return false;
    }
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->enableAngularMotor(JSValueToBoolean(context, value),
                                   constraint->getMotorTargetVelosity(),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptHingePhysicLinkGetAngularMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getMotorTargetVelosity());
}

static bool ScriptHingePhysicLinkSetAngularMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "HingePhysicLink setAngularMotorVelocity(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   JSValueToNumber(context, value, NULL),
                                   constraint->getMaxMotorImpulse());
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptHingePhysicLinkGetAngularMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getMaxMotorImpulse());
}

static bool ScriptHingePhysicLinkSetAngularMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "HingePhysicLink setAngularMotorForce(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->enableAngularMotor(constraint->getEnableAngularMotor(),
                                   constraint->getMotorTargetVelosity(),
                                   JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptHingePhysicLinkAccessFrameA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform(), constraint->getFrameOffsetB());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetA());
}

static JSValueRef ScriptHingePhysicLinkAccessFrameB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btHingeConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetB());
}

JSStaticValue ScriptHingePhysicLinkProperties[] = {
    {"angularPos", ScriptHingePhysicLinkGetAngularPos, 0, ScriptMethodAttributes},
    {"angularLimitMin", ScriptHingePhysicLinkGetAngularLimitMin, ScriptHingePhysicLinkSetAngularLimitMin, kJSPropertyAttributeDontDelete},
    {"angularLimitMax", ScriptHingePhysicLinkGetAngularLimitMax, ScriptHingePhysicLinkSetAngularLimitMax, kJSPropertyAttributeDontDelete},
    {"angularMotorEnabled", ScriptHingePhysicLinkGetAngularMotorEnabled, ScriptHingePhysicLinkSetAngularMotorEnabled, kJSPropertyAttributeDontDelete},
    {"angularMotorVelocity", ScriptHingePhysicLinkGetAngularMotorVelocity, ScriptHingePhysicLinkSetAngularMotorVelocity, kJSPropertyAttributeDontDelete},
    {"angularMotorForce", ScriptHingePhysicLinkGetAngularMotorForce, ScriptHingePhysicLinkSetAngularMotorForce, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptHingePhysicLinkMethods[] = {
    {"frameA", ScriptHingePhysicLinkAccessFrameA, ScriptMethodAttributes},
    {"frameB", ScriptHingePhysicLinkAccessFrameB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(HingePhysicLink, ScriptHingePhysicLinkProperties, ScriptHingePhysicLinkMethods);



static JSObjectRef ScriptPointPhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "PointPhysicLink Constructor: Expected RigidObject, RigidObject, Vector3, Vector3");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btVector3 pointA = getScriptVector3(context, argv[2]),
    pointB = getScriptVector3(context, argv[3]);
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btPoint2PointConstraint(*a->getBody(), *b->getBody(), pointA, pointB);
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptPointPhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "PointPhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(PointPhysicLink);

static JSValueRef ScriptPointPhysicLinkAccessPointA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        constraint->setPivotA(vec);
        activateConstraint(constraint);
        return argv[0];
    }else
        return newScriptVector3(context, constraint->getPivotInA());
}

static JSValueRef ScriptPointPhysicLinkAccessPointB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btPoint2PointConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        constraint->setPivotB(vec);
        activateConstraint(constraint);
        return argv[0];
    }else
        return newScriptVector3(context, constraint->getPivotInB());
}

JSStaticFunction ScriptPointPhysicLinkMethods[] = {
    {"pointA", ScriptPointPhysicLinkAccessPointA, ScriptMethodAttributes},
    {"pointB", ScriptPointPhysicLinkAccessPointB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(PointPhysicLink, NULL, ScriptPointPhysicLinkMethods);



static JSObjectRef ScriptSliderPhysicLinkConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptRigidObject]) ||
       !JSValueIsObjectOfClass(context, argv[2], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[3], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "SliderPhysicLink Constructor: Expected RigidObject, RigidObject, Matrix4, Matrix4");
    LinkInitializer initializer;
    RigidObject *a, *b;
    initializer.object[0] = a = getDataOfInstance<RigidObject>(context, argv[0]);
    initializer.object[1] = b = getDataOfInstance<RigidObject>(context, argv[1]);
    btTransform transA = getDataOfInstance<Matrix4>(context, argv[2])->getBTTransform(),
    transB = getDataOfInstance<Matrix4>(context, argv[3])->getBTTransform();
    PhysicLink* linkPtr = new PhysicLink();
    auto constraint = new btSliderConstraint(*a->getBody(), *b->getBody(), transA, transB, true);
    if(linkPtr->init(initializer, constraint)) {
        linkPtr->scriptInstance = JSObjectMake(context, ScriptClasses[ScriptSliderPhysicLink], linkPtr);
        return linkPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "SliderPhysicLink Constructor: Invalid argument");
}

ScriptClassStaticDefinition(SliderPhysicLink);

static JSValueRef ScriptSliderPhysicLinkGetAngularPos(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getAngularPos());
}

static JSValueRef ScriptSliderPhysicLinkGetLinearPos(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getLinearPos());
}

static JSValueRef ScriptSliderPhysicLinkGetAngularLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getLowerAngLimit());
}

static bool ScriptSliderPhysicLinkSetAngularLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setAngularLimitMin(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLowerAngLimit(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetAngularLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getUpperAngLimit());
}

static bool ScriptSliderPhysicLinkSetAngularLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setAngularLimitMax(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setUpperAngLimit(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetAngularMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getPoweredAngMotor());
}

static bool ScriptSliderPhysicLinkSetAngularMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setAngularMotorEnabled(): Expected Boolean");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setPoweredAngMotor(JSValueToBoolean(context, value));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetAngularMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getTargetAngMotorVelocity());
}

static bool ScriptSliderPhysicLinkSetAngularMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setAngularMotorVelocity(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setTargetAngMotorVelocity(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetAngularMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getMaxAngMotorForce());
}

static bool ScriptSliderPhysicLinkSetAngularMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setAngularMotorForce(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setMaxAngMotorForce(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetLinearLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getLowerLinLimit());
}

static bool ScriptSliderPhysicLinkSetLinearLimitMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setLinearLimitMin(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setLowerLinLimit(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetLinearLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getUpperLinLimit());
}

static bool ScriptSliderPhysicLinkSetLinearLimitMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setLinearLimitMax(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setUpperLinLimit(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetLinearMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getPoweredLinMotor());
}

static bool ScriptSliderPhysicLinkSetLinearMotorEnabled(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setLinearMotorEnabled(): Expected Boolean");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setPoweredLinMotor(JSValueToBoolean(context, value));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetLinearMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getTargetLinMotorVelocity());
}

static bool ScriptSliderPhysicLinkSetLinearMotorVelocity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setLinearMotorVelocity(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setTargetLinMotorVelocity(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkGetLinearMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint)->getMaxLinMotorForce());
}

static bool ScriptSliderPhysicLinkSetLinearMotorForce(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "SliderPhysicLink setLinearMotorForce(): Expected Number");
        return false;
    }
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    constraint->setMaxLinMotorForce(JSValueToNumber(context, value, NULL));
    activateConstraint(constraint);
    return true;
}

static JSValueRef ScriptSliderPhysicLinkAccessFrameA(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform(), constraint->getFrameOffsetB());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetA());
}

static JSValueRef ScriptSliderPhysicLinkAccessFrameB(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto constraint = static_cast<btSliderConstraint*>(getDataOfInstance<PhysicLink>(instance)->constraint);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        activateConstraint(constraint);
        constraint->setFrames(constraint->getFrameOffsetA(), getDataOfInstance<Matrix4>(context, argv[0])->getBTTransform());
        return argv[0];
    }else
        return newScriptMatrix4(context, constraint->getFrameOffsetB());
}

JSStaticValue ScriptSliderPhysicLinkProperties[] = {
    {"angularPos", ScriptSliderPhysicLinkGetAngularPos, 0, ScriptMethodAttributes},
    {"linearPos", ScriptSliderPhysicLinkGetLinearPos, 0, ScriptMethodAttributes},
    {"angularLimitMin", ScriptSliderPhysicLinkGetAngularLimitMin, ScriptSliderPhysicLinkSetAngularLimitMin, kJSPropertyAttributeDontDelete},
    {"angularLimitMax", ScriptSliderPhysicLinkGetAngularLimitMax, ScriptSliderPhysicLinkSetAngularLimitMax, kJSPropertyAttributeDontDelete},
    {"angularMotorEnabled", ScriptSliderPhysicLinkGetAngularMotorEnabled, ScriptSliderPhysicLinkSetAngularMotorEnabled, kJSPropertyAttributeDontDelete},
    {"angularMotorVelocity", ScriptSliderPhysicLinkGetAngularMotorVelocity, ScriptSliderPhysicLinkSetAngularMotorVelocity, kJSPropertyAttributeDontDelete},
    {"angularMotorForce", ScriptSliderPhysicLinkGetAngularMotorForce, ScriptSliderPhysicLinkSetAngularMotorForce, kJSPropertyAttributeDontDelete},
    {"linearLimitMin", ScriptSliderPhysicLinkGetLinearLimitMin, ScriptSliderPhysicLinkSetLinearLimitMin, kJSPropertyAttributeDontDelete},
    {"linearLimitMax", ScriptSliderPhysicLinkGetLinearLimitMax, ScriptSliderPhysicLinkSetLinearLimitMax, kJSPropertyAttributeDontDelete},
    {"linearMotorEnabled", ScriptSliderPhysicLinkGetLinearMotorEnabled, ScriptSliderPhysicLinkSetLinearMotorEnabled, kJSPropertyAttributeDontDelete},
    {"linearMotorVelocity", ScriptSliderPhysicLinkGetLinearMotorVelocity, ScriptSliderPhysicLinkSetLinearMotorVelocity, kJSPropertyAttributeDontDelete},
    {"linearMotorForce", ScriptSliderPhysicLinkGetLinearMotorForce, ScriptSliderPhysicLinkSetLinearMotorForce, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptSliderPhysicLinkMethods[] = {
    {"frameA", ScriptSliderPhysicLinkAccessFrameA, ScriptMethodAttributes},
    {"frameB", ScriptSliderPhysicLinkAccessFrameB, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(SliderPhysicLink, ScriptSliderPhysicLinkProperties, ScriptSliderPhysicLinkMethods);