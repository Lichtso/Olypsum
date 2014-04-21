//
//  ScriptIntersection.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.04.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#include "ScriptManager.h"

//! @cond
struct IntersectionCallback : btBroadphaseAabbCallback {
    short int m_collisionFilterMask;
    std::unordered_set<BaseObject*> hits;
    
    IntersectionCallback(short int filterMask) : m_collisionFilterMask(filterMask) {
        
    }
    
    bool process(const btBroadphaseProxy* proxy) {
        if((proxy->m_collisionFilterGroup & m_collisionFilterMask) == 0) return false;
        
        btCollisionObject* co = static_cast<btCollisionObject*>(proxy->m_clientObject);
        hits.insert(static_cast<BaseObject*>(co->getUserPointer()));
        return true;
    }
};

struct	ContactResultCallback : btCollisionWorld::ContactResultCallback {
    short int filterMask;
    std::unordered_set<BaseObject*> hits;
    
    ContactResultCallback(short _filterMask) :filterMask(_filterMask) { }
    
    bool needsCollision(btBroadphaseProxy* proxy0) const {
        return (proxy0->m_collisionFilterGroup & filterMask) != 0;
    }
    
    btScalar addSingleResult(btManifoldPoint& cp,
                             const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0,
                             const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1) {
        
        btCollisionObject* co = const_cast<btCollisionObject*>(colObj1Wrap->getCollisionObject());
        hits.insert(static_cast<BaseObject*>(co->getUserPointer()));
        return cp.m_distance1;
    }
};
//! @endcond

static JSValueRef ScriptIntersectionRayCast(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 4 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptVector3]) ||
       !JSValueIsNumber(context, argv[2]) || !JSValueIsBoolean(context, argv[3]))
        return ScriptException(context, exception, "Intersection rayCast(): Expected Vector3, Vector3, Number, Boolean");
    
    Ray3 ray(getScriptVector3(context, argv[0]), getScriptVector3(context, argv[1]));
    JSObjectRef objects = JSObjectMakeArray(context, 0, NULL, NULL),
    positions = JSObjectMakeArray(context, 0, NULL, NULL),
    normals = JSObjectMakeArray(context, 0, NULL, NULL),
    result = JSObjectMake(context, NULL, NULL);
    
    if(JSValueToBoolean(context, argv[3])) {
        BaseObject* object;
        btVector3 position, normal;
        unsigned int count = ray.hitTestNearest(JSValueToNumber(context, argv[2], NULL), object, position, normal);
        if(count) {
            JSObjectSetPropertyAtIndex(context, objects, 0, object->scriptInstance, NULL);
            JSObjectSetPropertyAtIndex(context, positions, 0, newScriptVector3(context, position), NULL);
            JSObjectSetPropertyAtIndex(context, normals, 0, newScriptVector3(context, normal), NULL);
        }
    }else{
        std::vector<BaseObject*> object;
        std::vector<btVector3> position, normal;
        unsigned int count = ray.hitTestAll(JSValueToNumber(context, argv[2], NULL), object, position, normal);
        for(unsigned int i = 0; i < count; i ++) {
            JSObjectSetPropertyAtIndex(context, objects, i, object[i]->scriptInstance, NULL);
            JSObjectSetPropertyAtIndex(context, positions, i, newScriptVector3(context, position[i]), NULL);
            JSObjectSetPropertyAtIndex(context, normals, i, newScriptVector3(context, normal[i]), NULL);
        }
    }
    
    JSObjectSetProperty(context, result, ScriptStringObjects.str, objects, 0, NULL);
    JSObjectSetProperty(context, result, ScriptStringPositions.str, positions, 0, NULL);
    JSObjectSetProperty(context, result, ScriptStringNormals.str, normals, 0, NULL);
    return result;
}

static JSValueRef ScriptIntersectionAABBIntersection(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) ||
       !JSValueIsObjectOfClass(context, argv[1], ScriptClasses[ScriptVector3]) ||
       !JSValueIsNumber(context, argv[2]))
        return ScriptException(context, exception, "Intersection aabbIntersection(): Expected Vector3, Vector3, Number");
    
    IntersectionCallback resultCallback(JSValueToNumber(context, argv[2], NULL));
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    broadphase->aabbTest(getScriptVector3(context, argv[0]), getScriptVector3(context, argv[1]), resultCallback);
    
    JSObjectRef objects = JSObjectMakeArray(context, 0, NULL, NULL);
    unsigned int i = 0;
    for(auto hit : resultCallback.hits)
        JSObjectSetPropertyAtIndex(context, objects, i ++, hit->scriptInstance, NULL);
    return objects;
}

static JSValueRef ScriptIntersectionSphereIntersection(JSContextRef context, JSObjectRef function, JSObjectRef instance, size_t argc, const JSValueRef argv[], JSValueRef* exception) {
    if(argc != 3 ||
       !JSValueIsObjectOfClass(context, argv[0], ScriptClasses[ScriptVector3]) ||
       !JSValueIsNumber(context, argv[1]) || !JSValueIsNumber(context, argv[2]))
        return ScriptException(context, exception, "Intersection sphereIntersection(): Expected Vector3, Number, Number");
    
    btTransform tmpTransform;
    tmpTransform.setOrigin(getScriptVector3(context, argv[0]));
    btSphereShape tmpShape(JSValueToNumber(context, argv[1], NULL));
    btCollisionObject tmpObject;
    tmpObject.setCollisionShape(&tmpShape);
    tmpObject.setWorldTransform(tmpTransform);
    ContactResultCallback resultCallback(JSValueToNumber(context, argv[2], NULL));
    objectManager.physicsWorld->contactTest(&tmpObject, resultCallback);
    
    JSObjectRef objects = JSObjectMakeArray(context, 0, NULL, NULL);
    unsigned int i = 0;
    for(auto hit : resultCallback.hits)
        JSObjectSetPropertyAtIndex(context, objects, i ++, hit->scriptInstance, NULL);
    return objects;
}

JSStaticFunction ScriptIntersectionMethods[] = {
    {"rayCast", ScriptIntersectionRayCast, ScriptMethodAttributes},
    {"aabbIntersection", ScriptIntersectionAABBIntersection, ScriptMethodAttributes},
    {"sphereIntersection", ScriptIntersectionSphereIntersection, ScriptMethodAttributes},
    {0, 0, 0}
};

ScriptClassDefinition(Intersection, NULL, ScriptIntersectionMethods);