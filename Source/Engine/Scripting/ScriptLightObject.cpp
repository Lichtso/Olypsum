//
//  ScriptLightObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 12.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptLightObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "LightObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(LightObject);

static JSValueRef ScriptLightObjectGetRange(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<LightObject>(instance)->getRange());
}

static JSValueRef ScriptLightObjectAccessColor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<LightObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->color = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->color.getVector());
}

JSStaticValue ScriptLightObjectProperties[] = {
    {"range", ScriptLightObjectGetRange, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptLightObjectMethods[] = {
    {"color", ScriptLightObjectAccessColor, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(LightObject, ScriptLightObjectProperties, ScriptLightObjectMethods);



static JSObjectRef ScriptDirectionalLightConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "DirectionalLight Constructor: Expected Matrix4, Vector3");
    Matrix4* transformation = getDataOfInstance<Matrix4>(context, argv[0]);
    auto objectPtr = new DirectionalLight(transformation->getBTTransform(), getScriptVector3(context, argv[1]));
    return objectPtr->scriptInstance;
}

ScriptClassStaticDefinition(DirectionalLight);

static JSValueRef ScriptDirectionalLightAccessBounds(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<DirectionalLight>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->setBounds(getScriptVector3(context, argv[0]));
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->getBounds());
}

JSStaticFunction ScriptDirectionalLightMethods[] = {
    {"color", ScriptDirectionalLightAccessBounds, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(DirectionalLight, NULL, ScriptDirectionalLightMethods);



static JSObjectRef ScriptPositionalLightConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsBoolean(context, argv[1]) ||
       !JSValueIsNumber(context, argv[2]))
        return ScriptException(context, exception, "PositionalLight Constructor: Expected Matrix4, Boolean, Number");
    Matrix4* transformation = getDataOfInstance<Matrix4>(context, argv[0]);
    auto objectPtr = new PositionalLight(transformation->getBTTransform(), JSValueToBoolean(context, argv[1]), JSValueToNumber(context, argv[2], NULL));
    return objectPtr->scriptInstance;
}

ScriptClassStaticDefinition(PositionalLight);

static JSValueRef ScriptPositionalLightGetOmniDirectional(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<PositionalLight>(instance)->getOmniDirectional());
}

static JSValueRef ScriptPositionalLightSetBounds(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 || !JSValueIsBoolean(context, argv[0]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "SpotLight setBounds(): Expected Boolean, Number");
    auto objectPtr = getDataOfInstance<PositionalLight>(instance);
    objectPtr->setBounds(JSValueToBoolean(context, argv[0]), JSValueToNumber(context, argv[1], NULL));
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptPositionalLightProperties[] = {
    {"omniDirectional", ScriptPositionalLightGetOmniDirectional, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptPositionalLightMethods[] = {
    {"setBounds", ScriptPositionalLightSetBounds, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(PositionalLight, ScriptPositionalLightProperties, ScriptPositionalLightMethods);



static JSObjectRef ScriptSpotLightConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsNumber(context, argv[1]) ||
       !JSValueIsNumber(context, argv[2]))
        return ScriptException(context, exception, "SpotLight Constructor: Expected Matrix4, Number, Number");
    Matrix4* transformation = getDataOfInstance<Matrix4>(context, argv[0]);
    auto objectPtr = new SpotLight(transformation->getBTTransform(), JSValueToNumber(context, argv[1], NULL), JSValueToNumber(context, argv[2], NULL));
    return objectPtr->scriptInstance;
}

ScriptClassStaticDefinition(SpotLight);

static JSValueRef ScriptSpotLightGetCutoff(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<SpotLight>(instance)->getCutoff());
}

static JSValueRef ScriptSpotLightSetBounds(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 || !JSValueIsNumber(context, argv[0]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "SpotLight setBounds(): Expected Number, Number");
    auto objectPtr = getDataOfInstance<SpotLight>(instance);
    objectPtr->setBounds(JSValueToNumber(context, argv[0], NULL), JSValueToNumber(context, argv[1], NULL));
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptSpotLightProperties[] = {
    {"cutoff", ScriptSpotLightGetCutoff, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptSpotLightMethods[] = {
    {"setBounds", ScriptSpotLightSetBounds, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(SpotLight, ScriptSpotLightProperties, ScriptSpotLightMethods);