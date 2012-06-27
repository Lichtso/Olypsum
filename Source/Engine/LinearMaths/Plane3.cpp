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
    normal = ((posB-posA)/(posC-posA)).normalize();
    distance = -(normal*posA);
}

void Plane3::set(Ray3 ray) {
    distance = -(ray.direction*ray.origin);
    normal = ray.direction;
}

float Plane3::getPointDist(Vector3 pos) {
    return pos*normal-distance;
}

float Plane3::getRayDist(Ray3 ray) {
    return -(ray.origin*normal+distance)/(ray.direction*normal);
}

bool Plane3::testPointHit(Vector3 pos) {
    return (pos*normal >= distance);
}

bool Plane3::testBsHit(Bs3* bs) {
    return (bs->transformation->pos*normal-distance >= bs->radius);
}

template <class T> bool Plane3::testBoxHit(T* box) {
    Vector3 vertices[8];
    box->getVertices(vertices);
    for(unsigned char i = 0; i < 8; i ++)
        if(vertices[i]*normal >= distance) return true;
    return false;
}

template <class T> unsigned int Plane3::testBoxHitCount(T* box) {
    unsigned int count = 0;
    Vector3 vertices[8];
    box->getVertices(vertices);
    for(unsigned char i = 0; i < 8; i ++)
        if(vertices[i]*normal >= distance) count ++;
    return count;
}

bool Plane3::testPolyhedronInclusiveHit(Vector3* vertices, unsigned int verticesCount) {
    for(unsigned int i = 0; i < verticesCount; i ++)
        if(vertices[i]*normal >= distance) return true;
    return false;
}

bool Plane3::testPolyhedronExclusiveHit(Vector3* vertices, unsigned int verticesCount) {
    for(unsigned int i = 0; i < verticesCount; i ++)
        if(vertices[i]*normal < distance) return false;
    return true;
}