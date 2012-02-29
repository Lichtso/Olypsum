//
//  BoundingVolumes.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "BoundingVolumes.h"
#import "Plane3.h"

Aabb3::Aabb3(Vector3 minB, Vector3 maxB) {
    min = minB;
    max = maxB;
}

void Aabb3::set(Vector3 minB, Vector3 maxB) {
    min = minB;
    max = maxB;
}

bool Aabb3::testPointHit(Vector3 pos) {
    return pos >= min && pos <= max;
}

bool Aabb3::testAabbHit(Aabb3* aabb) {
    Vector3 minB(fmax(min.x, aabb->min.x), fmax(min.y, aabb->min.y), fmax(min.z, aabb->min.z)),
            maxB(fmax(max.x, aabb->max.x), fmax(max.y, aabb->max.y), fmax(max.z, aabb->max.z));
    return minB < maxB;
}



Bs3::Bs3(Vector3 centerB, float radiusB) {
    center = centerB;
    radius = radiusB;
}

void Bs3::set(Vector3 centerB, float radiusB) {
    center = centerB;
    radius = radiusB;
}

bool Bs3::testPointHit(Vector3 pos) {
    return (center-pos).getLength() < radius;
}

bool Bs3::testBsHit(Bs3* bs) {
    return (center-bs->center).getLength() < radius+bs->radius;
}

bool Bs3::testAabbHit(Aabb3* aabb) {
    if(center >= aabb->min && center <= aabb->max) return true;
    
    Plane3 plane(aabb->min, Vector3(1, 0, 0));
    if(center*plane.normal-plane.distance < -radius) return false;
    plane.set(aabb->min, Vector3(0, 1, 0));
    if(center*plane.normal-plane.distance < -radius) return false;
    plane.set(aabb->min, Vector3(0, 0, 1));
    if(center*plane.normal-plane.distance < -radius) return false;
    plane.set(aabb->max, Vector3(-1, 0, 0));
    if(center*plane.normal-plane.distance < -radius) return false;
    plane.set(aabb->max, Vector3(0, -1, 0));
    if(center*plane.normal-plane.distance < -radius) return false;
    plane.set(aabb->max, Vector3(0, 0, -1));
    if(center*plane.normal-plane.distance < -radius) return false;
    
    return true;
}