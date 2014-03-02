//
//  ScriptIntersection.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.04.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ScriptIntersection.h"

void ScriptIntersection::Constructor(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    return ScriptException("Intersection Constructor: Class can't be instantiated");
}

void ScriptIntersection::RayCast(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 4)
        return ScriptException("Intersection rayCast(): Too few arguments");
    if(!scriptVector3->isCorrectInstance(args[0]) || !scriptVector3->isCorrectInstance(args[1]) ||
       !args[2]->IsInt32() || !args[3]->IsBoolean())
        return ScriptException("Intersection rayCast(): Invalid argument");
    
    Ray3 ray(scriptVector3->getDataOfInstance(args[0]), scriptVector3->getDataOfInstance(args[1]));
    v8::Handle<v8::Array> objects, points, normals;
    
    if(args[3]->BooleanValue()) {
        BaseObject* object;
        btVector3 point, normal;
        unsigned int count = ray.hitTestNearest(args[2]->Uint32Value(), object, point, normal);
        objects = v8::Array::New(v8::Isolate::GetCurrent(), count);
        points = v8::Array::New(v8::Isolate::GetCurrent(), count);
        normals = v8::Array::New(v8::Isolate::GetCurrent(), count);
        if(count) {
            objects->Set(0, v8::Handle<v8::Object>(*object->scriptInstance));
            points->Set(0, scriptVector3->newInstance(point));
            normals->Set(0, scriptVector3->newInstance(normal));
        }
    }else{
        std::vector<BaseObject*> object;
        std::vector<btVector3> point, normal;
        unsigned int count = ray.hitTestAll(args[2]->Uint32Value(), object, point, normal);
        objects = v8::Array::New(v8::Isolate::GetCurrent(), count);
        points = v8::Array::New(v8::Isolate::GetCurrent(), count);
        normals = v8::Array::New(v8::Isolate::GetCurrent(), count);
        for(unsigned int i = 0; i < count; i ++) {
            objects->Set(i, v8::Handle<v8::Object>(*object[i]->scriptInstance));
            points->Set(i, scriptVector3->newInstance(point[i]));
            normals->Set(i, scriptVector3->newInstance(normal[i]));
        }
    }
    
    v8::Handle<v8::Object> result = v8::Object::New(v8::Isolate::GetCurrent());
    result->Set(ScriptString("objects"), objects);
    result->Set(ScriptString("points"), points);
    result->Set(ScriptString("normals"), normals);
    ScriptReturn(result);
}

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

void ScriptIntersection::AABBIntersection(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 3)
        return ScriptException("Intersection aabbIntersection(): Too few arguments");
    if(!scriptVector3->isCorrectInstance(args[0]) || !scriptVector3->isCorrectInstance(args[1]) || !args[2]->IsInt32())
        return ScriptException("Intersection aabbIntersection(): Invalid argument");
    
    IntersectionCallback resultCallback(args[2]->Uint32Value());
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    broadphase->aabbTest(scriptVector3->getDataOfInstance(args[0]), scriptVector3->getDataOfInstance(args[1]), resultCallback);
    
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(v8::Isolate::GetCurrent(), resultCallback.hits.size());
    for(auto hit : resultCallback.hits)
        objects->Set(i ++, v8::Handle<v8::Object>(*hit->scriptInstance));
    ScriptReturn(objects);
}

void ScriptIntersection::SphereIntersection(const v8::FunctionCallbackInfo<v8::Value>& args) {
    ScriptScope();
    if(args.Length() < 3)
        return ScriptException("Intersection sphereIntersection(): Too few arguments");
    if(!scriptVector3->isCorrectInstance(args[0]) || !args[1]->IsNumber() || !args[2]->IsInt32())
        return ScriptException("Intersection sphereIntersection(): Invalid argument");
    
    btTransform tmpTransform;
    tmpTransform.setOrigin(scriptVector3->getDataOfInstance(args[0]));
    btSphereShape tmpShape(args[1]->NumberValue());
    btCollisionObject tmpObject;
    tmpObject.setCollisionShape(&tmpShape);
    tmpObject.setWorldTransform(tmpTransform);
    
    ContactResultCallback resultCallback(args[2]->Uint32Value());
    objectManager.physicsWorld->contactTest(&tmpObject, resultCallback);
    
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(v8::Isolate::GetCurrent(), resultCallback.hits.size());
    for(auto hit : resultCallback.hits)
        objects->Set(i ++, v8::Handle<v8::Object>(*hit->scriptInstance));
    ScriptReturn(objects);
}

ScriptIntersection::ScriptIntersection() :ScriptClass("Intersection", Constructor) {
    ScriptScope();
    
    (*functionTemplate)->Set(ScriptString("rayCast"), ScriptMethod(RayCast));
    (*functionTemplate)->Set(ScriptString("aabbIntersection"), ScriptMethod(AABBIntersection));
    (*functionTemplate)->Set(ScriptString("sphereIntersection"), ScriptMethod(SphereIntersection));
}