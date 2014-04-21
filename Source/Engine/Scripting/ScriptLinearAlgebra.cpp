//
//  ScriptLinearAlgebra.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 30.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

JSStringRef propertyNames[] = { ScriptStringX.str, ScriptStringY.str, ScriptStringZ.str, ScriptStringW.str };

static JSObjectRef ScriptVector3Constructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    JSObjectRef instance = JSObjectMake(context, ScriptClasses[ScriptVector3], NULL);
    
    switch(argc) {
        case 1: {
            JSObjectRef valueObject = JSValueToObject(context, argv[0], NULL);
            if(!valueObject)
                return ScriptException(context, exception, "Vector3 Constructor: Expected Array or Object");
            JSValueRef valueLength = JSObjectGetProperty(context, valueObject, ScriptStringLength.str, NULL);
            if(valueLength) {
                unsigned int length = JSValueToNumber(context, valueLength, NULL);
                if(length != 3)
                    return ScriptException(context, exception, "Vector3 Constructor: Expected Array.length to be 3");
                for(unsigned int i = 0; i < 3; i ++) {
                    JSValueRef property = JSObjectGetPropertyAtIndex(context, valueObject, i, NULL);
                    if(!JSValueIsNumber(context, property))
                        return ScriptException(context, exception, "Vector3 Constructor: Element is not a Number");
                    JSObjectSetProperty(context, instance, propertyNames[i], property, kJSPropertyAttributeDontDelete, NULL);
                }
            }else{
                for(unsigned int i = 0; i < 3; i ++) {
                    JSValueRef property = JSObjectGetProperty(context, valueObject, propertyNames[i], NULL);
                    if(!JSValueIsNumber(context, property))
                        return ScriptException(context, exception, "Vector3 Constructor: Element is not a Number");
                    JSObjectSetProperty(context, instance, propertyNames[i], property, kJSPropertyAttributeDontDelete, NULL);
                }
            }
            return instance;
        } case 3:
            for(unsigned int i = 0; i < 3; i ++) {
                if(!JSValueIsNumber(context, argv[i]))
                    return ScriptException(context, exception, "Vector3 Constructor: Element is not a Number");
                JSObjectSetProperty(context, instance, propertyNames[i], argv[i], kJSPropertyAttributeDontDelete, NULL);
            }
            return instance;
        default:
            return ScriptException(context, exception, "Vector3 Constructor: Expected 1 or 3 arguments");
    }
}

ScriptClassStaticDefinition(Vector3);

static JSValueRef ScriptVector3ToString(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btVector3 vec = getScriptVector3(context, instance);
    ScriptString result(stringOf(vec));
    return result.getJSStr(context);
}

static JSValueRef ScriptVector3ToJSON(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    JSValueRef values[3];
    for(unsigned int i = 0; i < 3; i ++)
        values[i] = JSObjectGetProperty(context, instance, propertyNames[i], NULL);
    return JSObjectMakeArray(context, 3, values, NULL);
}

static JSValueRef ScriptVector3GetLength(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btVector3 vec = getScriptVector3(context, instance);
    return JSValueMakeNumber(context, vec.length());
}

static JSValueRef ScriptVector3GetNormalized(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btVector3 vec = getScriptVector3(context, instance);
    return newScriptVector3(context, vec.normalize());
}

static JSValueRef ScriptVector3GetAngle(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 getAngle: Expected Vector3");
    btVector3 vecA = getScriptVector3(context, instance),
              vecB = getScriptVector3(context, argv[0]);
    return JSValueMakeNumber(context, vecA.angle(vecB));
}

static JSValueRef ScriptVector3GetDotProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 getDot: Expected Vector3");
    btVector3 vecA = getScriptVector3(context, instance),
    vecB = getScriptVector3(context, argv[0]);
    return JSValueMakeNumber(context, vecA.dot(vecB));
}

static JSValueRef ScriptVector3GetCrossProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 getCross: Expected Vector3");
    btVector3 vecA = getScriptVector3(context, instance),
    vecB = getScriptVector3(context, argv[0]);
    return newScriptVector3(context, vecA.cross(vecB));
}

static JSValueRef ScriptVector3GetSum(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 getSum: Expected Vector3");
    btVector3 vec = getScriptVector3(context, instance);
    vec += getScriptVector3(context, argv[0]);
    return newScriptVector3(context, vec);
}

static JSValueRef ScriptVector3GetDifference(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 getDiff: Expected Vector3");
    btVector3 vec = getScriptVector3(context, instance);
    vec -= getScriptVector3(context, argv[0]);
    return newScriptVector3(context, vec);
}

static JSValueRef ScriptVector3GetProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 1)
        return ScriptException(context, exception, "Vector3 getProduct: Expected Vector3 or Number");
    btVector3 vec = getScriptVector3(context, instance);
    if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return newScriptVector3(context, vec * getScriptVector3(context, argv[0]));
    else if(JSValueIsNumber(context, argv[0]))
        return newScriptVector3(context, vec * JSValueToNumber(context, argv[0], NULL));
    else
        return ScriptException(context, exception, "Vector3 getProduct: Expected Vector3 or Number");
}

static JSValueRef ScriptVector3GetQuotient(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 1)
        return ScriptException(context, exception, "Vector3 getQuotient: Expected Vector3 or Number");
    btVector3 vec = getScriptVector3(context, instance);
    if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return newScriptVector3(context, vec / getScriptVector3(context, argv[0]));
    else if(JSValueIsNumber(context, argv[0]))
        return newScriptVector3(context, vec / JSValueToNumber(context, argv[0], NULL));
    else
        return ScriptException(context, exception, "Vector3 getQuotient: Expected Vector3 or Number");
}

static JSValueRef ScriptVector3GetInterpolation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "Vector3 getInterpolation: Expected Vector3, Number");
    btVector3 vecA = getScriptVector3(context, instance),
              vecB = getScriptVector3(context, argv[0]);
    return newScriptVector3(context, vecA.lerp(vecB, JSValueToNumber(context, argv[1], NULL)));
}

static JSValueRef ScriptVector3Add(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 add: Expected Vector3");
    btVector3 vec = getScriptVector3(context, instance);
    vec += getScriptVector3(context, argv[0]);
    setScriptVector3(context, instance, vec);
    return instance;
}

static JSValueRef ScriptVector3Subtract(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 sub: Expected Vector3");
    btVector3 vec = getScriptVector3(context, instance);
    vec -= getScriptVector3(context, argv[0]);
    setScriptVector3(context, instance, vec);
    return instance;
}

static JSValueRef ScriptVector3Multiply(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 1)
        return ScriptException(context, exception, "Vector3 mult: Expected Vector3 or Number");
    btVector3 vec = getScriptVector3(context, instance);
    if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        setScriptVector3(context, instance, vec * getScriptVector3(context, argv[0]));
    else if(JSValueIsNumber(context, argv[0]))
        setScriptVector3(context, instance, vec * JSValueToNumber(context, argv[0], NULL));
    else
        return ScriptException(context, exception, "Vector3 mult: Expected Vector3 or Number");
    return instance;
}

static JSValueRef ScriptVector3Divide(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 1)
        return ScriptException(context, exception, "Vector3 divide: Expected Vector3 or Number");
    btVector3 vec = getScriptVector3(context, instance);
    if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        setScriptVector3(context, instance, vec / getScriptVector3(context, argv[0]));
    else if(JSValueIsNumber(context, argv[0]))
        setScriptVector3(context, instance, vec / JSValueToNumber(context, argv[0], NULL));
    else
        return ScriptException(context, exception, "Vector3 divide: Expected Vector3 or Number");
    return instance;
}

static JSValueRef ScriptVector3Normalize(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btVector3 vec = getScriptVector3(context, instance);
    setScriptVector3(context, instance, vec.normalize());
    return instance;
}

static JSValueRef ScriptVector3CrossProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Vector3 cross: Expected Vector3");
    btVector3 vecA = getScriptVector3(context, instance),
              vecB = getScriptVector3(context, argv[0]);
    setScriptVector3(context, instance, vecA.cross(vecB));
    return instance;
}

JSStaticFunction ScriptVector3Methods[] = {
    {"toString", ScriptVector3ToString, ScriptMethodAttributes},
    {"toJSON", ScriptVector3ToJSON, ScriptMethodAttributes},
    {"getLength", ScriptVector3GetLength, ScriptMethodAttributes},
    {"getNormalized", ScriptVector3GetNormalized, ScriptMethodAttributes},
    {"getAngle", ScriptVector3GetAngle, ScriptMethodAttributes},
    {"getDot", ScriptVector3GetDotProduct, ScriptMethodAttributes},
    {"getCross", ScriptVector3GetCrossProduct, ScriptMethodAttributes},
    {"getSum", ScriptVector3GetSum, ScriptMethodAttributes},
    {"getDiff", ScriptVector3GetDifference, ScriptMethodAttributes},
    {"getProduct", ScriptVector3GetProduct, ScriptMethodAttributes},
    {"getQuotient", ScriptVector3GetQuotient, ScriptMethodAttributes},
    {"getInterpolation", ScriptVector3GetInterpolation, ScriptMethodAttributes},
    {"add", ScriptVector3Add, ScriptMethodAttributes},
    {"sub", ScriptVector3Subtract, ScriptMethodAttributes},
    {"mult", ScriptVector3Multiply, ScriptMethodAttributes},
    {"divide", ScriptVector3Divide, ScriptMethodAttributes},
    {"normalize", ScriptVector3Normalize, ScriptMethodAttributes},
    {"cross", ScriptVector3CrossProduct, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Vector3, NULL, ScriptVector3Methods);

btVector3 getScriptVector3(JSContextRef context, JSValueRef value) {
    JSObjectRef instance = JSValueToObject(context, value, NULL);
    double values[3];
    for(unsigned int i = 0; i < 3; i ++)
        values[i] = JSValueToNumber(context, JSObjectGetProperty(context, instance, propertyNames[i], NULL), NULL);
    return btVector3(values[0], values[1], values[2]);
}

void setScriptVector3(JSContextRef context, JSValueRef value, const btVector3& vec) {
    JSObjectRef instance = JSValueToObject(context, value, NULL);
    double values[3] = { vec.x(), vec.y(), vec.z() };
    for(unsigned int i = 0; i < 3; i ++)
        JSObjectSetProperty(context, instance, propertyNames[i], JSValueMakeNumber(context, values[i]), kJSPropertyAttributeDontDelete, NULL);
}

JSObjectRef newScriptVector3(JSContextRef context, const btVector3& vec) {
    JSObjectRef instance = JSObjectMake(context, ScriptClasses[ScriptVector3], NULL);
    double values[3] = { vec.x(), vec.y(), vec.z() };
    for(unsigned int i = 0; i < 3; i ++)
        JSObjectSetProperty(context, instance, propertyNames[i], JSValueMakeNumber(context, values[i]), kJSPropertyAttributeDontDelete, NULL);
    return instance;
}



static JSObjectRef ScriptQuaternionConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    JSObjectRef instance = JSObjectMake(context, ScriptClasses[ScriptQuaternion], NULL);
    
    JSValueRef values[4] = { NULL, NULL, NULL, NULL };
    switch(argc) {
        case 1: {
            JSObjectRef valueObject = JSValueToObject(context, argv[0], NULL);
            if(!valueObject)
                return ScriptException(context, exception, "Quaternion Constructor: Expected Array or Object");
            JSValueRef valueLength = JSObjectGetProperty(context, valueObject, ScriptStringLength.str, NULL);
            if(valueLength) {
                unsigned int length = JSValueToNumber(context, valueLength, NULL);
                if(length != 3 && length != 4)
                    return ScriptException(context, exception, "Quaternion Constructor: Expected Array.length to be 3 or 4");
                for(unsigned int i = 0; i < length; i ++) {
                    JSValueRef property = JSObjectGetPropertyAtIndex(context, valueObject, i, NULL);
                    if(JSValueIsNumber(context, property))
                        values[i] = property;
                    else
                        return ScriptException(context, exception, "Quaternion Constructor: Element is not a Number");
                }
            }else
                for(unsigned int i = 0; i < 4; i ++) {
                    JSValueRef property = JSObjectGetProperty(context, valueObject, propertyNames[i], NULL);
                    if(!property && i == 3) break;
                    if(JSValueIsNumber(context, property))
                        values[i] = property;
                    else
                        return ScriptException(context, exception, "Quaternion Constructor: Element is not a Number");
                }
        } break;
        case 2:
            if(!JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) || JSValueIsNumber(context, argv[1]))
                return ScriptException(context, exception, "Quaternion Constructor: Expected Vextor3, Number");
            setScriptQuaternion(context, instance, btQuaternion(getScriptVector3(context, argv[0]),
                                                                      JSValueToNumber(context, argv[1], NULL)));
            return instance;
        case 3:
        case 4:
            for(unsigned int i = 0; i < argc; i ++) {
                if(JSValueIsNumber(context, argv[i]))
                    values[i] = argv[i];
                else
                    return ScriptException(context, exception, "Quaternion Constructor: Element is not a Number");
            }
            break;
        default:
            return ScriptException(context, exception, "Quaternion Constructor: Expected at least 1 and at most 4 arguments");
    }
    
    if(values[3])
        for(unsigned int i = 0; i < 4; i ++)
            JSObjectSetProperty(context, instance, propertyNames[i], values[i], kJSPropertyAttributeDontDelete, NULL);
    else
        setScriptQuaternion(context, instance, btQuaternion(JSValueToNumber(context, values[0], NULL),
                                                                  JSValueToNumber(context, values[1], NULL),
                                                                  JSValueToNumber(context, values[2], NULL)));
    return instance;
}

ScriptClassStaticDefinition(Quaternion);

static JSValueRef ScriptQuaternionToString(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    ScriptString result(stringOf(quaternion));
    return result.getJSStr(context);
}

static JSValueRef ScriptQuaternionToJSON(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    JSValueRef values[4];
    for(unsigned int i = 0; i < 4; i ++)
        values[i] = JSObjectGetProperty(context, instance, propertyNames[i], NULL);
    return JSObjectMakeArray(context, 4, values, NULL);
}

static JSValueRef ScriptQuaternionGetAngle(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternionA = getScriptQuaternion(context, instance);
    if(argc == 0)
        return JSValueMakeNumber(context, quaternionA.getAngle());
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion getAngle: Expected Quaternion");
    btQuaternion quaternionB = getScriptQuaternion(context, argv[0]);
    return JSValueMakeNumber(context, quaternionA.angle(quaternionB));
}

static JSValueRef ScriptQuaternionGetAxis(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    return newScriptVector3(context, quaternion.getAxis());
}

static JSValueRef ScriptQuaternionGetInverse(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    return newScriptQuaternion(context, quaternion.inverse());
}

static JSValueRef ScriptQuaternionGetLength(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    return JSValueMakeNumber(context, quaternion.length());
}

static JSValueRef ScriptQuaternionGetNormalized(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    return newScriptQuaternion(context, quaternion.normalize());
}

static JSValueRef ScriptQuaternionGetSum(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion getSum: Expected Quaternion");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    quaternion += getScriptQuaternion(context, argv[0]);
    return newScriptQuaternion(context, quaternion);
}

static JSValueRef ScriptQuaternionGetDifference(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion getDiff: Expected Quaternion");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    quaternion += getScriptQuaternion(context, argv[0]);
    return newScriptQuaternion(context, quaternion);
}

static JSValueRef ScriptQuaternionGetDotProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion getDot: Expected Quaternion");
    btQuaternion quaternionA = getScriptQuaternion(context, instance),
                 quaternionB = getScriptQuaternion(context, argv[0]);
    return JSValueMakeNumber(context, quaternionA.dot(quaternionB));
}

static JSValueRef ScriptQuaternionGetProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1)
        return ScriptException(context, exception, "Quaternion getProduct: Expected Quaternion or Number");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    if(JSValueIsNumber(context, argv[0]))
        quaternion *= JSValueToNumber(context, argv[0], NULL);
    else if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        quaternion *= getScriptQuaternion(context, argv[0]);
    else
        return ScriptException(context, exception, "Quaternion getProduct: Expected Quaternion or Number");
    return newScriptQuaternion(context, quaternion);
}

static JSValueRef ScriptQuaternionGetInterpolation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "Quaternion getInterpolation: Expected Quaternion, Number");
    btQuaternion quaternionA = getScriptQuaternion(context, instance),
                 quaternionB = getScriptQuaternion(context, argv[0]);
    return newScriptQuaternion(context, quaternionA.slerp(quaternionB, JSValueToNumber(context, argv[1], NULL)));
}

static JSValueRef ScriptQuaternionAdd(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion add: Expected Quaternion");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    quaternion += getScriptQuaternion(context, argv[0]);
    setScriptQuaternion(context, instance, quaternion);
    return instance;
}

static JSValueRef ScriptQuaternionSubtract(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        return ScriptException(context, exception, "Quaternion sub: Expected Quaternion");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    quaternion += getScriptQuaternion(context, argv[0]);
    setScriptQuaternion(context, instance, quaternion);
    return instance;
}

static JSValueRef ScriptQuaternionMultiply(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1)
        return ScriptException(context, exception, "Quaternion mult: Expected Quaternion or Number");
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    if(JSValueIsNumber(context, argv[0]))
        quaternion *= JSValueToNumber(context, argv[0], NULL);
    else if(JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion]))
        quaternion *= getScriptQuaternion(context, argv[0]);
    else
        return ScriptException(context, exception, "Quaternion mult: Expected Quaternion or Number");
    setScriptQuaternion(context, instance, quaternion);
    return instance;
}

static JSValueRef ScriptQuaternionInvert(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    setScriptQuaternion(context, instance, quaternion.inverse());
    return instance;
}

static JSValueRef ScriptQuaternionNormalize(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btQuaternion quaternion = getScriptQuaternion(context, instance);
    setScriptQuaternion(context, instance, quaternion.normalize());
    return instance;
}

JSStaticFunction ScriptQuaternionMethods[] = {
    {"toString", ScriptQuaternionToString, ScriptMethodAttributes},
    {"toJSON", ScriptQuaternionToJSON, ScriptMethodAttributes},
    {"getAngle", ScriptQuaternionGetAngle, ScriptMethodAttributes},
    {"getAxis", ScriptQuaternionGetAxis, ScriptMethodAttributes},
    {"getInverse", ScriptQuaternionGetInverse, ScriptMethodAttributes},
    {"getLength", ScriptQuaternionGetLength, ScriptMethodAttributes},
    {"getNormalized", ScriptQuaternionGetNormalized, ScriptMethodAttributes},
    {"getSum", ScriptQuaternionGetSum, ScriptMethodAttributes},
    {"getDiff", ScriptQuaternionGetDifference, ScriptMethodAttributes},
    {"getDot", ScriptQuaternionGetDotProduct, ScriptMethodAttributes},
    {"getProduct", ScriptQuaternionGetProduct, ScriptMethodAttributes},
    {"getInterpolation", ScriptQuaternionGetInterpolation, ScriptMethodAttributes},
    {"add", ScriptQuaternionAdd, ScriptMethodAttributes},
    {"sub", ScriptQuaternionSubtract, ScriptMethodAttributes},
    {"mult", ScriptQuaternionMultiply, ScriptMethodAttributes},
    {"invert", ScriptQuaternionInvert, ScriptMethodAttributes},
    {"normalize", ScriptQuaternionNormalize, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Quaternion, NULL, ScriptQuaternionMethods);

btQuaternion getScriptQuaternion(JSContextRef context, JSValueRef value) {
    JSObjectRef instance = JSValueToObject(context, value, NULL);
    double values[4];
    for(unsigned int i = 0; i < 4; i ++)
        values[i] = JSValueToNumber(context, JSObjectGetProperty(context, instance, propertyNames[i], NULL), NULL);
    return btQuaternion(values[0], values[1], values[2], values[3]);
}

void setScriptQuaternion(JSContextRef context, JSValueRef value, const btQuaternion& quaternion) {
    JSObjectRef instance = JSValueToObject(context, value, NULL);
    double values[4] = { quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w() };
    for(unsigned int i = 0; i < 4; i ++)
        JSObjectSetProperty(context, instance, propertyNames[i], JSValueMakeNumber(context, values[i]), kJSPropertyAttributeDontDelete, NULL);
}

JSObjectRef newScriptQuaternion(JSContextRef context, const btQuaternion& quaternion) {
    JSObjectRef instance = JSObjectMake(context, ScriptClasses[ScriptQuaternion], NULL);
    double values[4] = { quaternion.x(), quaternion.y(), quaternion.z(), quaternion.w() };
    for(unsigned int i = 0; i < 4; i ++)
        JSObjectSetProperty(context, instance, propertyNames[i], JSValueMakeNumber(context, values[i]), kJSPropertyAttributeDontDelete, NULL);
    return instance;
}



static JSObjectRef ScriptMatrix4Constructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    std::unique_ptr<Matrix4> matrix(new Matrix4());
    JSObjectRef instance = JSObjectMake(context, ScriptClasses[ScriptMatrix4], matrix.get());
    
    if(argc == 0)
        matrix->setIdentity();
    else{
        JSObjectRef valueObject = JSValueToObject(context, argv[0], NULL);
        if(!valueObject)
            return ScriptException(context, exception, "Matrix4 Constructor: Expected Array or Matrix4");
        JSValueRef valueLength = JSObjectGetProperty(context, valueObject, ScriptStringLength.str, NULL);
        if(valueLength) {
            unsigned int length = JSValueToNumber(context, valueLength, NULL);
            if(length != 16)
                return ScriptException(context, exception, "Matrix4 Constructor: Expected Array.length to be 16");
            for(unsigned int i = 0; i < length; i ++) {
                JSValueRef property = JSObjectGetPropertyAtIndex(context, valueObject, i, NULL);
                if(JSValueIsNumber(context, property))
                    matrix->values[i] = JSValueToNumber(context, property, NULL);
                else
                    return ScriptException(context, exception, "Matrix4 Constructor: Element is not a Number");
            }
        }else if(JSValueIsObjectOfClass(context, valueObject, ScriptClasses[ScriptMatrix4]))
            *matrix = *getDataOfInstance<Matrix4>(context, valueObject);
        else
            return ScriptException(context, exception, "Matrix4 Constructor: Expected Array or Matrix4");
    }
    
    matrix.release();
    return instance;
}

ScriptClassStaticDefinition(Matrix4);

static void ScriptMatrix4Destructor(JSObjectRef instance) {
    delete getDataOfInstance<Matrix4>(instance);
}

static JSValueRef ScriptMatrix4ToString(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    ScriptString result(stringOf(*matrix));
    return result.getJSStr(context);
}

static JSValueRef ScriptMatrix4ToJSON(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    JSValueRef values[16];
    for(unsigned int i = 0; i < 16; i ++)
        values[i] = JSValueMakeNumber(context, matrix->values[i]);
    return JSObjectMakeArray(context, 16, values, NULL);
}

static JSValueRef ScriptMatrix4AccessX(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        matrix->x = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, matrix->x);
}

static JSValueRef ScriptMatrix4AccessY(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        matrix->y = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, matrix->y);
}

static JSValueRef ScriptMatrix4AccessZ(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        matrix->z = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, matrix->z);
}

static JSValueRef ScriptMatrix4AccessW(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        matrix->w = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, matrix->w);
}

static JSValueRef ScriptMatrix4AccessRotation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) {
        btMatrix3x3 basis;
        basis.setRotation(getScriptQuaternion(context, argv[0]));
        *matrix = Matrix4(basis, matrix->w);
        return argv[0];
    }else{
        btQuaternion quaternion;
        matrix->getBTMatrix3x3().getRotation(quaternion);
        return newScriptQuaternion(context, quaternion);
    }
}

static JSValueRef ScriptMatrix4SetIdentity(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    matrix->setIdentity();
    return instance;
}

static JSValueRef ScriptMatrix4GetInverse(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return newScriptMatrix4(context, getDataOfInstance<Matrix4>(instance)->getInverse());
}

static JSValueRef ScriptMatrix4GetProduct(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "Matrix4 getProduct: Expected Matrix4");
    return newScriptMatrix4(context, *getDataOfInstance<Matrix4>(instance) * *getDataOfInstance<Matrix4>(context, argv[0]));
}

static JSValueRef ScriptMatrix4GetRotatedVector(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Matrix4 getRotated: Expected Vector3");
    btMatrix3x3 basis = getDataOfInstance<Matrix4>(instance)->getBTMatrix3x3();
    return newScriptVector3(context, basis * getScriptVector3(context, argv[0]));
}

static JSValueRef ScriptMatrix4GetTransformedVector(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Matrix4 getTransformed: Expected Vector3");
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    btVector3 vec = getScriptVector3(context, argv[0]);
    vec.setW(1.0);
    return newScriptVector3(context, (*matrix)(vec));
}

static JSValueRef ScriptMatrix4Multiply(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]))
        return ScriptException(context, exception, "Matrix4 mult: Expected Matrix4");
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    *matrix *= *getDataOfInstance<Matrix4>(context, argv[0]);
    return instance;
}

static JSValueRef ScriptMatrix4Scale(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Matrix4 scale: Expected Vector3");
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    matrix->scale(getScriptVector3(context, argv[0]));
    return instance;
}

static JSValueRef ScriptMatrix4Rotate(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    switch(argc) {
        case 1:
            if(!JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptQuaternion])) break;
            matrix->rotate(getScriptQuaternion(context, argv[0]));
            return instance;
        case 2:
            if(!JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) || !JSValueIsNumber(context, argv[1])) break;
            matrix->rotate(getScriptVector3(context, argv[0]), JSValueToNumber(context, argv[1], NULL));
            return instance;
    }
    return ScriptException(context, exception, "Matrix4 rotate: Expected Quaternion or Vector3, Number");
}

static JSValueRef ScriptMatrix4Translate(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "Matrix4 scale: Expected Vector3");
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    matrix->translate(getScriptVector3(context, argv[0]));
    return instance;
}

static JSValueRef ScriptMatrix4Invert(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    Matrix4* matrix = getDataOfInstance<Matrix4>(instance);
    *matrix = matrix->getInverse();
    return instance;
}

JSStaticFunction ScriptMatrix4Methods[] = {
    {"toString", ScriptMatrix4ToString, ScriptMethodAttributes},
    {"toJSON", ScriptMatrix4ToJSON, ScriptMethodAttributes},
    {"x", ScriptMatrix4AccessX, ScriptMethodAttributes},
    {"y", ScriptMatrix4AccessY, ScriptMethodAttributes},
    {"z", ScriptMatrix4AccessZ, ScriptMethodAttributes},
    {"w", ScriptMatrix4AccessW, ScriptMethodAttributes},
    {"rotation", ScriptMatrix4AccessRotation, ScriptMethodAttributes},
    {"setIdentity", ScriptMatrix4SetIdentity, ScriptMethodAttributes},
    {"getInverse", ScriptMatrix4GetInverse, ScriptMethodAttributes},
    {"getProduct", ScriptMatrix4GetProduct, ScriptMethodAttributes},
    {"getRotated", ScriptMatrix4GetRotatedVector, ScriptMethodAttributes},
    {"getTransformed", ScriptMatrix4GetTransformedVector, ScriptMethodAttributes},
    {"mult", ScriptMatrix4Multiply, ScriptMethodAttributes},
    {"scale", ScriptMatrix4Scale, ScriptMethodAttributes},
    {"rotate", ScriptMatrix4Rotate, ScriptMethodAttributes},
    {"translate", ScriptMatrix4Translate, ScriptMethodAttributes},
    {"invert", ScriptMatrix4Invert, ScriptMethodAttributes},
    {0, 0, 0}
};

JSClassDefinition ScriptMatrix4Definition = {
    0, kJSClassAttributeNone, "Matrix4", NULL, NULL,
    ScriptMatrix4Methods, NULL, ScriptMatrix4Destructor,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL
};

JSObjectRef newScriptMatrix4(JSContextRef context, const Matrix4& copy) {
    Matrix4* matrix = new Matrix4();
    *matrix = copy;
    return JSObjectMake(context, ScriptClasses[ScriptMatrix4], matrix);
}