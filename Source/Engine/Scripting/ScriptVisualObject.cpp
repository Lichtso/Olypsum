//
//  ScriptVisualObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 04.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptMatterObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "MatterObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(MatterObject);

static JSValueRef ScriptMatterObjectGetAngularFriction(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(instance)->getBody();
    return JSValueMakeNumber(context, physicBody->getRollingFriction());
}

static bool ScriptMatterObjectSetAngularFriction(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "MatterObject setAngularFriction(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        getDataOfInstance<PhysicObject>(instance)->getBody()->setRollingFriction(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptMatterObjectGetLinearFriction(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(instance)->getBody();
    return JSValueMakeNumber(context, physicBody->getFriction());
}

static bool ScriptMatterObjectSetLinearFriction(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "MatterObject setLinearFriction(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        getDataOfInstance<PhysicObject>(instance)->getBody()->setFriction(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptMatterObjectGetRestitution(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    btCollisionObject* physicBody = getDataOfInstance<PhysicObject>(instance)->getBody();
    return JSValueMakeNumber(context, physicBody->getRestitution());
}

static bool ScriptMatterObjectSetRestitution(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "MatterObject setRestitution(): Expected Number");
        return false;
    }
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        getDataOfInstance<PhysicObject>(instance)->getBody()->setRestitution(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptMatterObjectGetIntegrity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<MatterObject>(instance);
    return JSValueMakeNumber(context, objectPtr->integrity);
}

static bool ScriptMatterObjectSetIntegrity(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "MatterObject setIntegrity(): Expected Number");
        return false;
    }
    auto objectPtr = getDataOfInstance<MatterObject>(instance);
    if(objectPtr->integrity <= 0.0) return false;
    double numberValue = JSValueToNumber(context, value, NULL);
    objectPtr->integrity = fmax(0.0, numberValue);
    return true;
}

static JSValueRef ScriptMatterObjectAttachDecal(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 3 || !JSValueIsNumber(context, argv[0]) || !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptMatrix4]) || !JSValueIsString(context, argv[2]))
        return ScriptException(context, exception, "MatterObject attachDecal(): Expected Number, Matrix4, String");
    Decal* decal = new Decal();
    decal->life = JSValueToNumber(context, argv[0], NULL);
    decal->transformation = getDataOfInstance<Matrix4>(context, argv[1])->getBTTransform();
    ScriptString strDiffuse(context, argv[2]);
    decal->diffuse = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strDiffuse.getStdStr());
    if(argc > 3) {
        if(!JSValueIsString(context, argv[3]))
            return ScriptException(context, exception, "MatterObject attachDecal(): Expected Number, Matrix4, String, String");
        ScriptString strHeightMap(context, argv[3]);
        decal->heightMap = fileManager.getResourceByPath<Texture>(levelManager.levelPackage, strHeightMap.getStdStr());
    }
    auto objectPtr = getDataOfInstance<MatterObject>(instance);
    objectPtr->decals.insert(decal);
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptMatterObjectProperties[] = {
    {"angularFriction", ScriptMatterObjectGetAngularFriction, ScriptMatterObjectSetAngularFriction, kJSPropertyAttributeDontDelete},
    {"linearFriction", ScriptMatterObjectGetLinearFriction, ScriptMatterObjectSetLinearFriction, kJSPropertyAttributeDontDelete},
    {"restitution", ScriptMatterObjectGetRestitution, ScriptMatterObjectSetRestitution, kJSPropertyAttributeDontDelete},
    {"integrity", ScriptMatterObjectGetIntegrity, ScriptMatterObjectSetIntegrity, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptMatterObjectMethods[] = {
    {"attachDecal", ScriptMatterObjectAttachDecal, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(MatterObject, ScriptMatterObjectProperties, ScriptMatterObjectMethods);



static JSObjectRef ScriptRigidObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "RigidObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(RigidObject);

static JSValueRef ScriptRigidObjectGetModel(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    std::string name;
    FilePackage* filePackage = fileManager.findResource<Model>(objectPtr->model, name);
    if(!filePackage)
        return ScriptException(context, exception, "RigidObject getModel(): Internal error");
    ScriptString strModel(fileManager.getPathOfResource(filePackage, name));
    return strModel.getJSStr(context);
}

static bool ScriptRigidObjectSetModel(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "RigidObject setModel(): Expected String");
        return false;
    }
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    ScriptString strModel(context, value);
    auto model = fileManager.getResourceByPath<Model>(levelManager.levelPackage, strModel.getStdStr());
    if(model) {
        objectPtr->setModel(NULL, model);
        return true;
    }else
        return false;
}

static JSValueRef ScriptRigidObjectGetMass(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    return JSValueMakeNumber(context, (body->getInvMass() == 0.0) ? 0.0 : 1.0/body->getInvMass());
}

static bool ScriptRigidObjectSetMass(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "RigidObject setMass(): Expected Number");
        return false;
    }
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        btVector3 inertia;
        body->getCollisionShape()->calculateLocalInertia(numberValue, inertia);
        body->setMassProps(numberValue, inertia);
        body->activate();
        return true;
    }else
        return false;
}

static JSValueRef ScriptRigidObjectGetAngularDamping(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    return JSValueMakeNumber(context, body->getAngularDamping());
}

static bool ScriptRigidObjectSetAngularDamping(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "RigidObject setAngularDamping(): Expected Number");
        return false;
    }
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        body->setAngularDamping(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptRigidObjectGetLinearDamping(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    return JSValueMakeNumber(context, body->getLinearDamping());
}

static bool ScriptRigidObjectSetLinearDamping(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "RigidObject setLinearDamping(): Expected Number");
        return false;
    }
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    double numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue >= 0.0) {
        body->setLinearDamping(numberValue);
        return true;
    }else
        return false;
}

static JSValueRef ScriptRigidObjectGetKinematic(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    return JSValueMakeBoolean(context, objectPtr->getKinematic());
}

static bool ScriptRigidObjectSetKinematic(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsBoolean(context, value)) {
        ScriptException(context, exception, "RigidObject setKinematic(): Expected Boolean");
        return false;
    }
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    objectPtr->setKinematic(JSValueToBoolean(context, value));
    return true;
}

static JSValueRef ScriptRigidObjectAccessTextureAnimationTime(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 1 || !JSValueIsNumber(context, argv[0]))
        return ScriptException(context, exception, "RigidObject textureAnimationTime(): Expected Number");
    unsigned int index = JSValueToNumber(context, argv[0], NULL);
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    if(index >= objectPtr->textureAnimationTime.size())
        return ScriptException(context, exception, "RigidObject textureAnimationTime(): Out of bounds");
    
    if(argc == 2 && JSValueIsNumber(context, argv[1])) {
        objectPtr->textureAnimationTime[index] = JSValueToNumber(context, argv[1], NULL);
        return argv[1];
    }else
        return JSValueMakeNumber(context, objectPtr->textureAnimationTime[index]);
}

static JSValueRef ScriptRigidObjectAccessTransformation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        Matrix4* matrix = getDataOfInstance<Matrix4>(context, argv[0]);
        if(matrix->isValid()) {
            objectPtr->setTransformation(matrix->getBTTransform());
            objectPtr->setKinematic(true);
        }
        return argv[0];
    }else
        return newScriptMatrix4(context, objectPtr->getTransformation());
}

static JSValueRef ScriptRigidObjectAccessAngularVelocity(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        if(isValidVector(vec)) {
            body->setAngularVelocity(vec);
            body->activate();
        }
        return argv[0];
    }else
        return newScriptVector3(context, body->getAngularVelocity());
}

static JSValueRef ScriptRigidObjectAccessLinearVelocity(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        if(isValidVector(vec)) {
            body->setLinearVelocity(vec);
            body->activate();
        }
        return argv[0];
    }else
        return newScriptVector3(context, body->getLinearVelocity());
}

static JSValueRef ScriptRigidObjectAccessAngularFactor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        if(isValidVector(vec))
            body->setAngularFactor(vec);
        return argv[0];
    }else
        return newScriptVector3(context, body->getAngularFactor());
}

static JSValueRef ScriptRigidObjectAccessLinearFactor(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3])) {
        btVector3 vec = getScriptVector3(context, argv[0]);
        if(isValidVector(vec))
            body->setLinearFactor(vec);
        return argv[0];
    }else
        return newScriptVector3(context, body->getLinearFactor());
}

static JSValueRef ScriptRigidObjectApplyImpulseAtPoint(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "RigidObject applyImpulseAtPoint(): Expected Vector3, Vector3");
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    btVector3 vecA = getScriptVector3(context, argv[0]), vecB = getScriptVector3(context, argv[1]);
    if(isValidVector(vecA) && isValidVector(vecB)) {
        body->applyImpulse(vecA, vecB);
        body->activate();
    }
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptRigidObjectApplyAngularImpulse(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "RigidObject applyAngularImpulse(): Expected Vector3");
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    btVector3 vec = getScriptVector3(context, argv[0]);
    if(isValidVector(vec)) {
        body->applyTorqueImpulse(vec);
        body->activate();
    }
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptRigidObjectApplyLinearImpulse(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]))
        return ScriptException(context, exception, "RigidObject applyLinearImpulse(): Expected Vector3");
    auto body = getDataOfInstance<RigidObject>(instance)->getBody();
    btVector3 vec = getScriptVector3(context, argv[0]);
    if(isValidVector(vec)) {
        body->applyCentralImpulse(vec);
        body->activate();
    }
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptRigidObjectGetBoneByName(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1 || !JSValueIsString(context, argv[0]))
        return ScriptException(context, exception, "RigidObject getBoneByName(): Expected String");
    ScriptString strName(context, argv[0]);
    auto objectPtr = getDataOfInstance<RigidObject>(instance);
    BoneLink* linkPrt = objectPtr->findBoneLinkOfName(strName.getStdStr());
    if(linkPrt)
        return linkPrt->b->scriptInstance;
    else
        return JSValueMakeUndefined(context);
}

JSStaticValue ScriptRigidObjectProperties[] = {
    {"model", ScriptRigidObjectGetModel, ScriptRigidObjectSetModel, kJSPropertyAttributeDontDelete},
    {"mass", ScriptRigidObjectGetMass, ScriptRigidObjectSetMass, kJSPropertyAttributeDontDelete},
    {"angularDamping", ScriptRigidObjectGetAngularDamping, ScriptRigidObjectSetAngularDamping, kJSPropertyAttributeDontDelete},
    {"linearDamping", ScriptRigidObjectGetLinearDamping, ScriptRigidObjectSetLinearDamping, kJSPropertyAttributeDontDelete},
    {"kinematic", ScriptRigidObjectGetKinematic, ScriptRigidObjectSetKinematic, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptRigidObjectMethods[] = {
    {"textureAnimationTime", ScriptRigidObjectAccessTextureAnimationTime, ScriptMethodAttributes},
    {"angularVelocity", ScriptRigidObjectAccessAngularVelocity, ScriptMethodAttributes},
    {"linearVelocity", ScriptRigidObjectAccessLinearVelocity, ScriptMethodAttributes},
    {"angularFactor", ScriptRigidObjectAccessAngularFactor, ScriptMethodAttributes},
    {"linearFactor", ScriptRigidObjectAccessLinearFactor, ScriptMethodAttributes},
    {"applyImpulseAtPoint", ScriptRigidObjectApplyImpulseAtPoint, ScriptMethodAttributes},
    {"applyAngularImpulse", ScriptRigidObjectApplyAngularImpulse, ScriptMethodAttributes},
    {"applyLinearImpulse", ScriptRigidObjectApplyLinearImpulse, ScriptMethodAttributes},
    {"getBoneByName", ScriptRigidObjectGetBoneByName, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(RigidObject, ScriptRigidObjectProperties, ScriptRigidObjectMethods);



static JSObjectRef ScriptTerrainObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "TerrainObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(TerrainObject);

static JSValueRef ScriptTerrainObjectGetWidth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<TerrainObject>(instance)->width);
}

static JSValueRef ScriptTerrainObjectGetLength(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<TerrainObject>(instance)->length);
}

static JSValueRef ScriptTerrainObjectGetBitDepth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<TerrainObject>(instance)->bitDepth << 2);
}

static bool ScriptTerrainObjectSetBitDepth(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsNumber(context, value)) {
        ScriptException(context, exception, "TerrainObject setBitDepth(): Expected Number");
        return false;
    }
    auto objectPtr = getDataOfInstance<TerrainObject>(instance);
    unsigned int numberValue = JSValueToNumber(context, value, NULL);
    if(numberValue > 0 && numberValue <= 32 && numberValue % 4 == 0) {
        objectPtr->bitDepth = numberValue >> 2;
        return true;
    }else
        return false;
}

static JSValueRef ScriptTerrainObjectAccessCell(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc < 2 || !JSValueIsNumber(context, argv[0]) || !JSValueIsNumber(context, argv[1]))
        return ScriptException(context, exception, "TerrainObject textureAnimationTime(): Expected Number, Number");
    unsigned int x = JSValueToNumber(context, argv[0], NULL), y = JSValueToNumber(context, argv[1], NULL);
    auto objectPtr = getDataOfInstance<TerrainObject>(instance);
    if(x >= objectPtr->width || y >= objectPtr->length)
        return ScriptException(context, exception, "TerrainObject accessCell(): Out of bounds");
    
    auto value = &objectPtr->heights[objectPtr->width*y+x];
    if(argc == 3) {
        if(!JSValueIsNumber(context, argv[2]))
            return ScriptException(context, exception, "TerrainObject accessCell(): Invalid argument");
        *value = clamp(JSValueToNumber(context, argv[2], NULL), 0.0, 1.0);
        return argv[2];
    }else
        return JSValueMakeNumber(context, *value);
}

static JSValueRef ScriptTerrainObjectUpdateModel(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<TerrainObject>(instance);
    objectPtr->updateModel();
    objectPtr->updateTouchingObjects();
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptTerrainObjectProperties[] = {
    {"width", ScriptTerrainObjectGetWidth, 0, ScriptMethodAttributes},
    {"length", ScriptTerrainObjectGetLength, 0, ScriptMethodAttributes},
    {"bitDepth", ScriptTerrainObjectGetBitDepth, ScriptTerrainObjectSetBitDepth, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptTerrainObjectMethods[] = {
    {"accessCell", ScriptTerrainObjectAccessCell, ScriptMethodAttributes},
    {"updateModel", ScriptTerrainObjectUpdateModel, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(TerrainObject, ScriptTerrainObjectProperties, ScriptTerrainObjectMethods);