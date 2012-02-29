//
//  Plane3.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Plane3.h"

Plane3::Plane3() {
    
}

Plane3::Plane3(float distanceB, Vector3 normalB) {
    normal = normalB;
    distance = distanceB;
}

Plane3::Plane3(Vector3 pos, Vector3 normalB) {
    normal = normalB;
    distance = -(normal*pos);
}

Plane3::Plane3(Vector3 posA, Vector3 posB, Vector3 posC) {
    normal = (posC-posA)/(posB-posA);
    distance = -(normal*posA);
}

Plane3::Plane3(Ray3 ray) {
    normal = ray.direction;
    distance = -(normal*ray.origin);
}

void Plane3::set(float distanceB, Vector3 normalB) {
    normal = normalB;
    distance = distanceB;
}

void Plane3::set(Vector3 pos, Vector3 normalB) {
    normal = normalB;
    distance = -(normal*pos);
}

void Plane3::set(Vector3 posA, Vector3 posB, Vector3 posC) {
    normal = (posB-posA)/(posC-posA);
    distance = -(normal*posA);
}

void Plane3::set(Ray3 ray) {
    distance = -(ray.direction*ray.origin);
    normal = ray.direction;
}

bool Plane3::testPointHit(Vector3 pos) {
    return (pos*normal <= distance);
}

bool Plane3::testBsHit(Bs3 bs) {
    return (bs.center*normal-distance <= bs.radius);
}

bool Plane3::testAabbHit(Aabb3* aabb) {
    if(Vector3(aabb->min.x, aabb->min.y, aabb->min.z)*normal <= distance) return true;
    if(Vector3(aabb->min.x, aabb->min.y, aabb->max.z)*normal <= distance) return true;
    if(Vector3(aabb->min.x, aabb->max.y, aabb->min.z)*normal <= distance) return true;
    if(Vector3(aabb->min.x, aabb->max.y, aabb->max.z)*normal <= distance) return true;
    if(Vector3(aabb->max.x, aabb->min.y, aabb->min.z)*normal <= distance) return true;
    if(Vector3(aabb->max.x, aabb->min.y, aabb->max.z)*normal <= distance) return true;
    if(Vector3(aabb->max.x, aabb->max.y, aabb->min.z)*normal <= distance) return true;
    if(Vector3(aabb->max.x, aabb->max.y, aabb->max.z)*normal <= distance) return true;
    return false;
}

unsigned int Plane3::testAabbHitCount(Aabb3* aabb) {
    unsigned int count = 0;
    if(Vector3(aabb->min.x, aabb->min.y, aabb->min.z)*normal <= distance) count ++;
    if(Vector3(aabb->min.x, aabb->min.y, aabb->max.z)*normal <= distance) count ++;
    if(Vector3(aabb->min.x, aabb->max.y, aabb->min.z)*normal <= distance) count ++;
    if(Vector3(aabb->min.x, aabb->max.y, aabb->max.z)*normal <= distance) count ++;
    if(Vector3(aabb->max.x, aabb->min.y, aabb->min.z)*normal <= distance) count ++;
    if(Vector3(aabb->max.x, aabb->min.y, aabb->max.z)*normal <= distance) count ++;
    if(Vector3(aabb->max.x, aabb->max.y, aabb->min.z)*normal <= distance) count ++;
    if(Vector3(aabb->max.x, aabb->max.y, aabb->max.z)*normal <= distance) count ++;
    return count;
}