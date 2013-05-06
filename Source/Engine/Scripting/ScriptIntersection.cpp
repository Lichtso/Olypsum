//
//  ScriptIntersection.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 03.04.13.
//
//

#include "ScriptIntersection.h"

v8::Handle<v8::Value> ScriptIntersection::Constructor(const v8::Arguments &args) {
    v8::HandleScope handleScope;
    return v8::ThrowException(v8::String::New("Intersection Constructor: Class can't be instantiated"));
}

v8::Handle<v8::Value> ScriptIntersection::RayCast(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 4)
        return v8::ThrowException(v8::String::New("rayCast(): Too less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]) ||
       !args[2]->IsInt32() || !args[3]->IsBoolean())
        return v8::ThrowException(v8::String::New("rayCast(): Invalid argument"));
    
    Ray3 ray(scriptVector3.getDataOfInstance(args[0]), scriptVector3.getDataOfInstance(args[1]));
    v8::Handle<v8::Array> objects, points, normals;
    
    if(args[3]->BooleanValue()) {
        BaseObject* object;
        btVector3 point, normal;
        unsigned int count = ray.hitTestNearest(args[2]->Uint32Value(), object, point, normal);
        objects = v8::Array::New(count);
        points = v8::Array::New(count);
        normals = v8::Array::New(count);
        if(count) {
            objects->Set(0, object->scriptInstance);
            points->Set(0, scriptVector3.newInstance(point));
            normals->Set(0, scriptVector3.newInstance(normal));
        }
    }else{
        std::vector<BaseObject*> object;
        std::vector<btVector3> point, normal;
        unsigned int count = ray.hitTestAll(args[2]->Uint32Value(), object, point, normal);
        objects = v8::Array::New(count);
        points = v8::Array::New(count);
        normals = v8::Array::New(count);
        for(unsigned int i = 0; i < count; i ++) {
            objects->Set(i, object[i]->scriptInstance);
            points->Set(i, scriptVector3.newInstance(point[i]));
            normals->Set(i, scriptVector3.newInstance(normal[i]));
        }
    }
    
    v8::Handle<v8::Object> result = v8::Object::New();
    result->Set(v8::String::New("objects"), objects);
    result->Set(v8::String::New("points"), points);
    result->Set(v8::String::New("normals"), normals);
    return handleScope.Close(result);
}

//! @cond
struct IntersectionCallback : btBroadphaseAabbCallback {
    short int m_collisionFilterMask;
    std::set<BaseObject*> hits;
    
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
    std::set<BaseObject*> hits;
    
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

v8::Handle<v8::Value> ScriptIntersection::AABBIntersection(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 3)
        return v8::ThrowException(v8::String::New("aabbIntersection(): Too less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !scriptVector3.isCorrectInstance(args[1]) || !args[2]->IsInt32())
        return v8::ThrowException(v8::String::New("aabbIntersection(): Invalid argument"));
    
    IntersectionCallback resultCallback(args[2]->Uint32Value());
    btDbvtBroadphase* broadphase = static_cast<btDbvtBroadphase*>(objectManager.broadphase);
    broadphase->aabbTest(scriptVector3.getDataOfInstance(args[0]), scriptVector3.getDataOfInstance(args[1]), resultCallback);
    
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(resultCallback.hits.size());
    for(auto hit : resultCallback.hits)
        objects->Set(i ++, hit->scriptInstance);
    return handleScope.Close(objects);
}

v8::Handle<v8::Value> ScriptIntersection::SphereIntersection(const v8::Arguments& args) {
    v8::HandleScope handleScope;
    if(args.Length() < 3)
        return v8::ThrowException(v8::String::New("sphereIntersection(): Too less arguments"));
    if(!scriptVector3.isCorrectInstance(args[0]) || !args[1]->IsNumber() || !args[2]->IsInt32())
        return v8::ThrowException(v8::String::New("sphereIntersection(): Invalid argument"));
    
    btTransform tmpTransform;
    tmpTransform.setOrigin(scriptVector3.getDataOfInstance(args[0]));
    btSphereShape tmpShape(args[1]->NumberValue());
    btCollisionObject tmpObject;
    tmpObject.setCollisionShape(&tmpShape);
    tmpObject.setWorldTransform(tmpTransform);
    
    ContactResultCallback resultCallback(args[2]->Uint32Value());
    objectManager.physicsWorld->contactTest(&tmpObject, resultCallback);
    
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New(resultCallback.hits.size());
    for(auto hit : resultCallback.hits)
        objects->Set(i ++, hit->scriptInstance);
    return handleScope.Close(objects);
}

ScriptIntersection::ScriptIntersection() :ScriptClass("Intersection", Constructor) {
    v8::HandleScope handleScope;
    
    functionTemplate->Set(v8::String::New("rayCast"), v8::FunctionTemplate::New(RayCast));
    functionTemplate->Set(v8::String::New("aabbIntersection"), v8::FunctionTemplate::New(AABBIntersection));
    functionTemplate->Set(v8::String::New("sphereIntersection"), v8::FunctionTemplate::New(SphereIntersection));
}

ScriptIntersection scriptIntersection;