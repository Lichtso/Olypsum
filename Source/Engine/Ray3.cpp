//
//  Ray3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 24.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ObjectManager.h"

Ray3::Ray3(const btVector3& originB, const btVector3& directionB) :origin(originB), direction(directionB) {
    
}

float Ray3::getDistToPlane(const btVector3& normal, float dist) {
    float determinant = normal.dot(direction);
    if(determinant == 0.0) return NAN;
    return -(normal.dot(origin)-dist)/determinant;
}

btVector3 Ray3::getHitPointPlane(const btVector3& normal, float dist) {
    float determinant = normal.dot(direction);
    if(determinant == 0.0) return origin;
    return origin-(normal.dot(origin)-dist)/determinant*direction;
}

unsigned int Ray3::hitTestNearest(short filterMask, BaseObject*& object, btVector3& point, btVector3& normal) {
    btCollisionWorld::ClosestRayResultCallback rayCallback(origin, origin+direction);
    rayCallback.m_collisionFilterGroup = 0xFFFF;
    rayCallback.m_collisionFilterMask = filterMask;
    objectManager.physicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
    if(rayCallback.hasHit()) {
        point = rayCallback.m_hitPointWorld;
        normal = rayCallback.m_hitNormalWorld;
    }
    btCollisionObject* co = const_cast<btCollisionObject*>(rayCallback.m_collisionObject);
    if(co) {
        object = static_cast<BaseObject*>(co->getUserPointer());
        return (object) ? 1 : 0;
    }else
        return 0;
}

unsigned int Ray3::hitTestAll(short filterMask, std::vector<BaseObject*>& objects, std::vector<btVector3>& points, std::vector<btVector3>& normals) {
    btCollisionWorld::AllHitsRayResultCallback rayCallback(origin, origin+direction);
    rayCallback.m_collisionFilterGroup = 0xFFFF;
    rayCallback.m_collisionFilterMask = filterMask;
    objectManager.physicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
    
    for(unsigned int i = 0; i < rayCallback.m_collisionObjects.size(); i ++) {
        objects.push_back(static_cast<BaseObject*>(rayCallback.m_collisionObjects[i]->getUserPointer()));
        points.push_back(rayCallback.m_hitPointWorld[i]);
        normals.push_back(rayCallback.m_hitNormalWorld[i]);
    }
    return rayCallback.m_collisionObjects.size();
}