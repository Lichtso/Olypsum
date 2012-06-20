//
//  BoundingVolumes.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "BoundingVolumes.h"
#import "Plane3.h"


BoundingVolume::BoundingVolume(Matrix4* transformationB) {
    transformation = transformationB;
}



Aabb3::Aabb3(Vector3 minB, Vector3 maxB, Matrix4* transformationB) : BoundingVolume(transformationB) {
    min = minB;
    max = maxB;
}

void Aabb3::getVertices(Vector3 vertices[8]) {
    vertices[0] = Vector3(min.x, min.y, min.z)+transformation->pos;
    vertices[1] = Vector3(min.x, min.y, max.z)+transformation->pos;
    vertices[2] = Vector3(min.x, max.y, min.z)+transformation->pos;
    vertices[3] = Vector3(min.x, max.y, max.z)+transformation->pos;
    vertices[4] = Vector3(max.x, min.y, min.z)+transformation->pos;
    vertices[5] = Vector3(max.x, min.y, max.z)+transformation->pos;
    vertices[6] = Vector3(max.x, max.y, min.z)+transformation->pos;
    vertices[7] = Vector3(max.x, max.y, max.z)+transformation->pos;
}

void Aabb3::getPlanes(Plane3 planes[6]) {
    Vector3 pos = min+transformation->pos;
    planes[0].set(pos, Vector3(-1, 0, 0));
    planes[1].set(pos, Vector3(0, -1, 0));
    planes[2].set(pos, Vector3(0, 0, -1));
    pos = max+transformation->pos;
    planes[3].set(pos, Vector3(1, 0, 0));
    planes[4].set(pos, Vector3(0, 1, 0));
    planes[5].set(pos, Vector3(0, 0, 1));
}

bool Aabb3::testPointHit(Vector3 pos) {
    return pos >= transformation->pos+min && pos <= transformation->pos+max;
}

bool Aabb3::testAabbHit(Aabb3* aabb) {
    Vector3 minB = min+transformation->pos, maxB = max+transformation->pos,
            minC = aabb->min+aabb->transformation->pos, maxC = aabb->max+aabb->transformation->pos;
    
    return Vector3(fmax(minB.x, minC.x), fmax(minB.y, minC.y), fmax(minB.z, minC.z))
         < Vector3(fmin(maxB.x, maxC.x), fmin(maxB.y, maxC.y), fmin(maxB.z, maxC.z));
}



void Box3::getVertices(Vector3 vertices[8]) {
    vertices[0] = Vector3(min.x, min.y, min.z)*(*transformation);
    vertices[1] = Vector3(min.x, min.y, max.z)*(*transformation);
    vertices[2] = Vector3(min.x, max.y, min.z)*(*transformation);
    vertices[3] = Vector3(min.x, max.y, max.z)*(*transformation);
    vertices[4] = Vector3(max.x, min.y, min.z)*(*transformation);
    vertices[5] = Vector3(max.x, min.y, max.z)*(*transformation);
    vertices[6] = Vector3(max.x, max.y, min.z)*(*transformation);
    vertices[7] = Vector3(max.x, max.y, max.z)*(*transformation);
}

void Box3::getPlanes(Plane3 planes[6]) {
    Matrix4 rotMat;
    rotMat.setMatrix3(*transformation);
    
    Vector3 normal = Vector3(-1, 0, 0)*rotMat;
    planes[0].set(normal+transformation->pos, normal);
    normal = Vector3(0, -1, 0)*rotMat;
    planes[1].set(normal+transformation->pos, normal);
    normal = Vector3(0, 0, -1)*rotMat;
    planes[2].set(normal+transformation->pos, normal);
    normal = Vector3(1, 0, 0)*rotMat;
    planes[3].set(normal+transformation->pos, normal);
    normal = Vector3(0, 1, 0)*rotMat;
    planes[4].set(normal+transformation->pos, normal);
    normal = Vector3(0, 0, 1)*rotMat;
    planes[5].set(normal+transformation->pos, normal);
}

bool Box3::testPointHit(Vector3 pos) {
    Plane3 planes[6];
    getPlanes(planes);
    for(unsigned char i = 0; i < 6; i ++)
        if(transformation->pos*planes[i].normal > planes[i].distance) return false;
    return true;
}



Bs3::Bs3(float radiusB, Matrix4* transformationB) : BoundingVolume(transformationB) {
    radius = radiusB;
}

bool Bs3::testPointHit(Vector3 pos) {
    return (transformation->pos-pos).getLength() < radius;
}

bool Bs3::testBsHit(Bs3* bs) {
    return (transformation->pos-bs->transformation->pos).getLength() < radius+bs->radius;
}