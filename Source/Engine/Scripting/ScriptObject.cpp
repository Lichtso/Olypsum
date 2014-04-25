//
//  ScriptObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.03.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

static JSObjectRef ScriptBaseClassConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "BaseClass Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(BaseClass);

static JSValueRef ScriptBaseClassGetScriptClass(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<BaseClass>(instance);
    ScriptString strClassName(fileManager.getPathOfResource(objectPtr->scriptFile->filePackage, objectPtr->scriptFile->name));
    return strClassName.getJSStr(context);
}

static bool ScriptBaseClassSetScriptClass(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "BaseClass setScriptClass(): Expected String");
        return false;
    }
    auto objectPtr = getDataOfInstance<BaseClass>(instance);
    ScriptString strClassName(context, value);
    FileResourcePtr<ScriptFile> scriptFile = fileManager.getResourceByPath<ScriptFile>(levelManager.levelPackage, strClassName.getStdStr());
    if(scriptFile) {
        objectPtr->scriptFile = scriptFile;
        return true;
    }else
        return false;
}

static JSValueRef ScriptBaseClassDelete(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    getDataOfInstance<BaseClass>(instance)->removeClean();
    return JSValueMakeUndefined(context);
}

JSStaticValue ScriptBaseClassProperties[] = {
    {"scriptClass", ScriptBaseClassGetScriptClass, ScriptBaseClassSetScriptClass, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptBaseClassMethods[] = {
    {"delete", ScriptBaseClassDelete, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(BaseClass, ScriptBaseClassProperties, ScriptBaseClassMethods);



static JSObjectRef ScriptBaseObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    return ScriptException(context, exception, "BaseObject Constructor: Class can't be instantiated");
}

ScriptClassStaticDefinition(BaseObject);

static JSValueRef ScriptBaseObjectAccessTransformation(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<BaseObject>(instance);
    if(argc == 1 && JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4])) {
        Matrix4* matrix = getDataOfInstance<Matrix4>(context, argv[0]);
        if(matrix->isValid())
            objectPtr->setTransformation(matrix->getBTTransform());
        return argv[0];
    }else
        return newScriptMatrix4(context, objectPtr->getTransformation());
}

static JSValueRef ScriptBaseObjectGetTransformUpLink(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<BaseObject>(instance);
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<TransformLink*>(*i) && (*i)->b == objectPtr)
            return (*i)->scriptInstance;
    return JSValueMakeNull(context);
}

static JSValueRef ScriptBaseObjectGetBoneUpLink(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    auto objectPtr = getDataOfInstance<BaseObject>(instance);
    foreach_e(objectPtr->links, i)
        if(dynamic_cast<BoneLink*>(*i) && (*i)->b == objectPtr)
            return (*i)->scriptInstance;
    return JSValueMakeNull(context);
}

static JSValueRef ScriptBaseObjectIterateLinks(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 1)
        return ScriptException(context, exception, "BaseObject iterateLinks: Expected Function");
    JSObjectRef callback = JSValueToObject(context, argv[0], NULL);
    if(!callback || !JSObjectIsFunction(context, callback))
        return ScriptException(context, exception, "BaseObject iterateLinks: Expected Function");
    exception = NULL;
    auto objectPtr = getDataOfInstance<BaseObject>(instance);
    for(auto link : objectPtr->links) {
        JSValueRef args[] = { link->scriptInstance };
        JSObjectCallAsFunction(context, callback, callback, 1, args, exception);
        if(exception)
            return NULL;
    }
    return JSValueMakeUndefined(context);
}

static JSValueRef ScriptBaseObjectGetLinkCount(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    return JSValueMakeNumber(context, getDataOfInstance<BaseObject>(instance)->links.size());
}

JSStaticValue ScriptBaseObjectProperties[] = {
    {"linkCount", ScriptBaseObjectGetLinkCount, 0, ScriptMethodAttributes},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptBaseObjectMethods[] = {
    {"transformation", ScriptBaseObjectAccessTransformation, ScriptMethodAttributes},
    {"getTransformUpLink", ScriptBaseObjectGetTransformUpLink, ScriptMethodAttributes},
    {"getBoneUpLink", ScriptBaseObjectGetBoneUpLink, ScriptMethodAttributes},
    {"iterateLinks", ScriptBaseObjectIterateLinks, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(BaseObject, ScriptBaseObjectProperties, ScriptBaseObjectMethods);



static JSObjectRef ScriptPhysicObjectConstructor(JSContextRef context, JSObjectRef constructor, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 2 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptMatrix4]) ||
       !JSValueIsString(context, argv[1]))
        return ScriptException(context, exception, "PhysicObject Constructor: Expected Matrix4, String");
    ScriptString strName(context, argv[1]);
    btCollisionShape* collisionShape = levelManager.getCollisionShape(strName.getStdStr());
    if(collisionShape) {
        Matrix4* transformation = getDataOfInstance<Matrix4>(context, argv[0]);
        auto objectPtr = new PhysicObject(transformation->getBTTransform(), collisionShape);
        return objectPtr->scriptInstance;
    }else
        return ScriptException(context, exception, "PhysicObject Constructor: Invalid collision shape");
}

ScriptClassStaticDefinition(PhysicObject);

static JSValueRef ScriptPhysicObjectGetCollisionShape(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef* exception) {
    btCollisionShape* shape = getDataOfInstance<PhysicObject>(instance)->getBody()->getCollisionShape();
    std::string buffer = levelManager.getCollisionShapeName(shape);
    if(buffer.size() == 0)
        return ScriptException(context, exception, "PhysicObject getCollisionShape(): Internal error");
    ScriptString strName(buffer);
    return strName.getJSStr(context);
}

static bool ScriptPhysicObjectSetCollisionShape(JSContextRef context, JSObjectRef instance, JSStringRef propertyName, JSValueRef value, JSValueRef* exception) {
    if(!JSValueIsString(context, value)) {
        ScriptException(context, exception, "PhysicObject setCollisionShape(): Expected String");
        return false;
    }
    ScriptString strName(context, value);
    btCollisionShape* shape = levelManager.getCollisionShape(strName.getStdStr());
    if(shape) {
        auto objectPtr = getDataOfInstance<PhysicObject>(instance);
        objectPtr->getBody()->setCollisionShape(shape);
        objectPtr->updateTouchingObjects();
        return true;
    }else
        return false;
}

static JSValueRef ScriptPhysicObjectGetCollisionShapeInfo(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    btCollisionShape* shape = getDataOfInstance<PhysicObject>(instance)->getBody()->getCollisionShape();
    JSObjectRef result = JSObjectMake(context, NULL, NULL);
    ScriptString strType(shape->getName());
    JSObjectSetProperty(context, result, ScriptStringType.str, strType.getJSStr(context), 0, NULL);
    switch(shape->getShapeType()) {
        case CYLINDER_SHAPE_PROXYTYPE: {
            btVector3 size = static_cast<btCylinderShape*>(shape)->getHalfExtentsWithMargin();
            JSObjectSetProperty(context, result, ScriptStringSize.str, newScriptVector3(context, size), 0, NULL);
            if(dynamic_cast<btCylinderShapeX*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringX.getJSStr(context), 0, NULL);
            else if(dynamic_cast<btCylinderShapeZ*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringZ.getJSStr(context), 0, NULL);
            else
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringY.getJSStr(context), 0, NULL);
        } break;
        case BOX_SHAPE_PROXYTYPE: {
            btVector3 size = static_cast<btBoxShape*>(shape)->getHalfExtentsWithMargin();
            JSObjectSetProperty(context, result, ScriptStringSize.str, newScriptVector3(context, size), 0, NULL);
        } break;
        case SPHERE_SHAPE_PROXYTYPE: {
            double radius = static_cast<btSphereShape*>(shape)->getRadius();
            JSObjectSetProperty(context, result, ScriptStringSize.str, JSValueMakeNumber(context, radius), 0, NULL);
        } break;
        case CAPSULE_SHAPE_PROXYTYPE: {
            auto capsuleShape = static_cast<btCapsuleShape*>(shape);
            double radius = capsuleShape->getRadius(), length = capsuleShape->getHalfHeight();
            JSObjectSetProperty(context, result, ScriptStringRadius.str, JSValueMakeNumber(context, radius), 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringLength.str, JSValueMakeNumber(context, length), 0, NULL);
            if(dynamic_cast<btCapsuleShapeX*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringX.getJSStr(context), 0, NULL);
            else if(dynamic_cast<btCapsuleShapeZ*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringZ.getJSStr(context), 0, NULL);
            else
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringY.getJSStr(context), 0, NULL);
        } break;
        case CONE_SHAPE_PROXYTYPE: {
            auto coneShape = static_cast<btConeShape*>(shape);
            double radius = coneShape->getRadius(), length = coneShape->getHeight();
            JSObjectSetProperty(context, result, ScriptStringRadius.str, JSValueMakeNumber(context, radius), 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringLength.str, JSValueMakeNumber(context, length), 0, NULL);
            if(dynamic_cast<btConeShapeX*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringX.getJSStr(context), 0, NULL);
            else if(dynamic_cast<btConeShapeZ*>(shape))
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringZ.getJSStr(context), 0, NULL);
            else
                JSObjectSetProperty(context, result, ScriptStringDirection.str, ScriptStringY.getJSStr(context), 0, NULL);
        } break;
        case MULTI_SPHERE_SHAPE_PROXYTYPE: {
            auto multiSphereShape = static_cast<btMultiSphereShape*>(shape);
            JSObjectRef positions = JSObjectMakeArray(context, 0, NULL, NULL),
                        radii = JSObjectMakeArray(context, 0, NULL, NULL);
            for(unsigned int i = 0; i < multiSphereShape->getSphereCount(); i ++) {
                JSObjectSetPropertyAtIndex(context, positions, i, newScriptVector3(context, multiSphereShape->getSpherePosition(i)), NULL);
                JSObjectSetPropertyAtIndex(context, radii, i, JSValueMakeNumber(context, multiSphereShape->getSphereRadius(i)), NULL);
            }
            JSObjectSetProperty(context, result, ScriptStringPositions.str, positions, 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringRadii.str, radii, 0, NULL);
        } break;
        case COMPOUND_SHAPE_PROXYTYPE: {
            auto compoundShape = static_cast<btCompoundShape*>(shape);
            JSObjectRef transformations = JSObjectMakeArray(context, 0, NULL, NULL),
                        children = JSObjectMakeArray(context, 0, NULL, NULL);
            for(unsigned int i = 0; i < compoundShape->getNumChildShapes(); i ++) {
                ScriptString strName(levelManager.getCollisionShapeName(compoundShape->getChildShape(i)));
                JSObjectSetPropertyAtIndex(context, transformations, i, newScriptMatrix4(context, compoundShape->getChildTransform(i)), NULL);
                JSObjectSetPropertyAtIndex(context, children, i, strName.getJSStr(context), NULL);
            }
            JSObjectSetProperty(context, result, ScriptStringTransformations.str, transformations, 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringChildren.str, children, 0, NULL);
        } break;
        case CONVEX_HULL_SHAPE_PROXYTYPE: {
            auto convexHullShape = static_cast<btConvexHullShape*>(shape);
           JSObjectRef positions = JSObjectMakeArray(context, 0, NULL, NULL);
            for(unsigned int i = 0; i < convexHullShape->getNumPoints(); i ++)
                JSObjectSetPropertyAtIndex(context, positions, i, newScriptVector3(context, convexHullShape->getUnscaledPoints()[i]), NULL);
            JSObjectSetProperty(context, result, ScriptStringPositions.str, positions, 0, NULL);
        } break;
        case STATIC_PLANE_PROXYTYPE: {
            auto staticPlaneShape = static_cast<btStaticPlaneShape*>(shape);
            JSObjectSetProperty(context, result, ScriptStringNormal.str, newScriptVector3(context, staticPlaneShape->getPlaneNormal()), 0, NULL);
            JSObjectSetProperty(context, result, ScriptStringDistance.str, JSValueMakeNumber(context, staticPlaneShape->getPlaneConstant()), 0, NULL);
        } break;
        case TERRAIN_SHAPE_PROXYTYPE: {
            auto objectPtr = getDataOfInstance<TerrainObject>(instance);
            btVector3 size = static_cast<btHeightfieldTerrainShape*>(shape)->getLocalScaling();
            size *= btVector3(objectPtr->width*0.5, 0.5, objectPtr->length*0.5);
            JSObjectSetProperty(context, result, ScriptStringSize.str, newScriptVector3(context, size), 0, NULL);
        } break;
    }
    return result;
}

JSStaticValue ScriptPhysicObjectProperties[] = {
    {"collisionShape", ScriptPhysicObjectGetCollisionShape, ScriptPhysicObjectSetCollisionShape, kJSPropertyAttributeDontDelete},
    {0, 0, 0, 0}
};

JSStaticFunction ScriptPhysicObjectMethods[] = {
    {"collisionShapeInfo", ScriptPhysicObjectGetCollisionShapeInfo, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(PhysicObject, ScriptPhysicObjectProperties, ScriptPhysicObjectMethods);