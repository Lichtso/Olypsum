//
//  ScriptParticlesObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptParticlesObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsNumber(context, argv[1]) || !JSValueIsString(context, argv[2]) || !JSValueIsString(context, argv[3]))
        return ScriptException(context, exception, "ParticlesObject Constructor: Expected Matrix4, String, String");
    ScriptString strTexture(context, argv[2]);
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strTexture.getStdStr());
    if(texture) {
        ScriptString strCollisionShape(context, argv[3]);
        btCollisionShape* collisionShape = levelManager.getCollisionShape(strCollisionShape.getStdStr());
        if(collisionShape) {
            Matrix4* transformation = getDataOfInstance<Matrix4>(context, argv[0]);
            auto objectPtr = new ParticlesObject(transformation->getBTTransform(), JSValueToNumber(context, argv[1], NULL), texture, collisionShape);
            return objectPtr->scriptInstance;
        }else
            return ScriptException(context, exception, "ParticlesObject Constructor: Invalid collision shape");
    }else
        return ScriptException(context, exception, "ParticlesObject Constructor: Invalid texture");
}

ScriptClassStaticDefinition(ParticlesObject);

static JSValueRef ScriptParticlesObjectGetMaxParticles(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->maxParticles);
}

static JSValueRef ScriptParticlesObjectGetTexture(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Texture>(objectPtr->texture, name);
    if(filePackage) {
        ScriptString strName(fileManager.getPathOfResource(filePackage, name));
        return strName.getJSStr(context);
    }else
        return ScriptException(context, exception, "ParticlesObject getTexture(): Internal error");
}

static bool ScriptParticlesObjectSetTexture(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "ParticlesObject setTexture(): Expected String");
        return false;
    }
    ScriptString strName(context, value);
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    auto texture = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strName.getStdStr());
    if(texture) {
        objectPtr->texture = texture;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectGetTransformAligned(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeBoolean(context, getDataOfInstance<ParticlesObject>(instance)->transformAligned);
}

static bool ScriptParticlesObjectSetTransformAligned(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "ParticlesObject setTransformAligned(): Expected Boolean");
        return false;
    }
    getDataOfInstance<ParticlesObject>(instance)->transformAligned = JSValueToBoolean(context, value);
    return true;
}

static JSValueRef ScriptParticlesObjectGetSystemLife(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->systemLife);
}

static bool ScriptParticlesObjectSetSystemLife(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ParticlesObject setSystemLife(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<ParticlesObject>(instance)->systemLife = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectGetLifeMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->lifeMin);
}

static bool ScriptParticlesObjectSetLifeMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ParticlesObject setLifeMin(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<ParticlesObject>(instance)->lifeMin = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectGetLifeMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->lifeMax);
}

static bool ScriptParticlesObjectSetLifeMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ParticlesObject setLifeMax(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<ParticlesObject>(instance)->lifeMax = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectGetSizeMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->sizeMin);
}

static bool ScriptParticlesObjectSetSizeMin(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ParticlesObject setLifeMin(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<ParticlesObject>(instance)->sizeMin = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectGetSizeMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<ParticlesObject>(instance)->sizeMax);
}

static bool ScriptParticlesObjectSetSizeMax(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "ParticlesObject setSizeMax(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(isfinite(numberValue)) {
        getDataOfInstance<ParticlesObject>(instance)->sizeMax = numberValue;
        return true;
    }else
        return false;
}

static JSValueRef ScriptParticlesObjectAccessForce(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->force = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->force);
}

static JSValueRef ScriptParticlesObjectAccessPosMin(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->posMin = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->posMin);
}

static JSValueRef ScriptParticlesObjectAccessPosMax(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->posMin = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->posMax);
}

static JSValueRef ScriptParticlesObjectAccessDirMin(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->dirMin = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->dirMin);
}

static JSValueRef ScriptParticlesObjectAccessDirMax(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<ParticlesObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        objectPtr->dirMin = getScriptVector3(context, argv[0]);
        return argv[0];
    }else
        return newScriptVector3(context, objectPtr->dirMin);
}

JSStaticValue ScriptParticlesObjectProperties[] = {
    {"maxParticles", ScriptParticlesObjectGetMaxParticles, 0, ScriptMethodAttributes},
    {"texture", ScriptParticlesObjectGetTexture, ScriptParticlesObjectSetTexture, kJSPropertyAttributeDontDelete},
    {"transformAligned", ScriptParticlesObjectGetTransformAligned, ScriptParticlesObjectSetTransformAligned, kJSPropertyAttributeDontDelete},
    {"systemLife", ScriptParticlesObjectGetSystemLife, ScriptParticlesObjectSetSystemLife, kJSPropertyAttributeDontDelete},
    {"lifeMin", ScriptParticlesObjectGetLifeMin, ScriptParticlesObjectSetLifeMin, kJSPropertyAttributeDontDelete},
    {"lifeMax", ScriptParticlesObjectGetLifeMax, ScriptParticlesObjectSetLifeMax, kJSPropertyAttributeDontDelete},
    {"sizeMin", ScriptParticlesObjectGetSizeMin, ScriptParticlesObjectSetSizeMin, kJSPropertyAttributeDontDelete},
    {"sizeMax", ScriptParticlesObjectGetSizeMax, ScriptParticlesObjectSetSizeMax, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptParticlesObjectMethods[] = {
    {"force", ScriptParticlesObjectAccessForce, ScriptMethodAttributes},
    {"posMin", ScriptParticlesObjectAccessPosMin, ScriptMethodAttributes},
    {"posMax", ScriptParticlesObjectAccessPosMax, ScriptMethodAttributes},
    {"dirMin", ScriptParticlesObjectAccessDirMin, ScriptMethodAttributes},
    {"dirMax", ScriptParticlesObjectAccessDirMax, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(ParticlesObject, ScriptParticlesObjectProperties, ScriptParticlesObjectMethods);