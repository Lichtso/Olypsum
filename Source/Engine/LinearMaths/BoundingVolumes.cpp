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

unsigned int Aabb3::getIndecies(unsigned int indecies[boxTrianglesCount]) {
    indecies[0] = 0; indecies[1] = 1; indecies[2] = 2;
    indecies[3] = 1; indecies[4] = 3; indecies[5] = 2;
    indecies[6] = 6; indecies[7] = 5; indecies[8] = 4;
    indecies[9] = 6; indecies[10] = 7; indecies[11] = 5;
    indecies[12] = 4; indecies[13] = 1; indecies[14] = 0;
    indecies[15] = 4; indecies[16] = 5; indecies[17] = 1;
    indecies[18] = 2; indecies[19] = 3; indecies[20] = 6;
    indecies[21] = 3; indecies[22] = 7; indecies[23] = 6;
    indecies[24] = 6; indecies[25] = 4; indecies[26] = 0;
    indecies[27] = 2; indecies[28] = 6; indecies[29] = 0;
    indecies[30] = 7; indecies[31] = 3; indecies[32] = 1;
    indecies[33] = 5; indecies[34] = 7; indecies[35] = 1;
    return 36;
}

unsigned int Aabb3::getVertices(Vector3 vertices[boxVerticesCount]) {
    vertices[0] = Vector3(min.x, min.y, min.z)+transformation->pos;
    vertices[1] = Vector3(min.x, min.y, max.z)+transformation->pos;
    vertices[2] = Vector3(min.x, max.y, min.z)+transformation->pos;
    vertices[3] = Vector3(min.x, max.y, max.z)+transformation->pos;
    vertices[4] = Vector3(max.x, min.y, min.z)+transformation->pos;
    vertices[5] = Vector3(max.x, min.y, max.z)+transformation->pos;
    vertices[6] = Vector3(max.x, max.y, min.z)+transformation->pos;
    vertices[7] = Vector3(max.x, max.y, max.z)+transformation->pos;
    return 8;
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



Box3::Box3(Vector3 minB, Vector3 maxB, Matrix4* transformationB) : Aabb3(minB, maxB, transformationB) {
    
}

unsigned int Box3::getIndecies(unsigned int indecies[boxTrianglesCount]) {
    return Aabb3::getIndecies(indecies);
}

unsigned int Box3::getVertices(Vector3 vertices[boxVerticesCount]) {
    vertices[0] = Vector3(min.x, min.y, min.z)*(*transformation);
    vertices[1] = Vector3(min.x, min.y, max.z)*(*transformation);
    vertices[2] = Vector3(min.x, max.y, min.z)*(*transformation);
    vertices[3] = Vector3(min.x, max.y, max.z)*(*transformation);
    vertices[4] = Vector3(max.x, min.y, min.z)*(*transformation);
    vertices[5] = Vector3(max.x, min.y, max.z)*(*transformation);
    vertices[6] = Vector3(max.x, max.y, min.z)*(*transformation);
    vertices[7] = Vector3(max.x, max.y, max.z)*(*transformation);
    return 8;
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
        if(!planes[i].testPointHit(pos)) return false;
    return true;
}



Bs3::Bs3(float radiusB, Matrix4* transformationB) : BoundingVolume(transformationB) {
    radius = radiusB;
}

unsigned int Bs3::getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int referenceVertex, index = 0;
    for(unsigned char x = 0; x < accuracyX; x ++) {
        indecies[index ++] = 0;
        indecies[index ++] = (x < accuracyX-1) ? x+2 : 1;
        indecies[index ++] = x+1;
    }
    for(unsigned char y = 0; y < accuracyY-1; y ++) {
        referenceVertex = accuracyX*y+1;
        for(unsigned char x = 0; x < accuracyX; x ++) {
            indecies[index ++] = referenceVertex+x;
            indecies[index ++] = (x < accuracyX-1) ? referenceVertex+x+1 : referenceVertex;
            indecies[index ++] = referenceVertex+accuracyX+x;
            indecies[index ++] = indecies[index-2];
            indecies[index ++] = (x < accuracyX-1) ? referenceVertex+accuracyX+x+1 : referenceVertex+accuracyX;
            indecies[index ++] = indecies[index-3];
        }
    }
    referenceVertex = accuracyX*(accuracyY-1)+1;
    for(unsigned char x = 0; x < accuracyX; x ++) {
        indecies[index ++] = referenceVertex+accuracyX;
        indecies[index ++] = referenceVertex+x;
        indecies[index ++] = (x < accuracyX-1) ? referenceVertex+x+1 : referenceVertex;
    }
    return index;
}

unsigned int Bs3::getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int index = 0;
    vertices[index ++] = Vector3(0.0, 0.0, radius)*(*transformation);
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY+1)*M_PI, circleRadius = sinf(circleAngle), circleZ = cosf(circleAngle);
        for(unsigned char x = 0; x < accuracyX; x ++) {
            float angle = (float)x/accuracyX*M_PI*2.0;
            vertices[index ++] = Vector3(sinf(angle)*circleRadius, cosf(angle)*circleRadius, circleZ)*radius*(*transformation);
        }
    }
    vertices[index ++] = Vector3(0.0, 0.0, -radius)*(*transformation);
    return index;
}

bool Bs3::testPointHit(Vector3 pos) {
    return (transformation->pos-pos).getLength() < radius;
}

bool Bs3::testBsHit(Bs3* bs) {
    return (transformation->pos-bs->transformation->pos).getLength() < radius+bs->radius;
}



Parabolid3::Parabolid3(float radiusB, Matrix4* transformation) : BoundingVolume(transformation) {
    radius = radiusB;
}

unsigned int Parabolid3::getIndecies(unsigned int indecies[], unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int referenceVertex, index = 0;
    for(unsigned char x = 0; x < accuracyX; x ++) {
        indecies[index ++] = 0;
        indecies[index ++] = (x < accuracyX-1) ? x+2 : 1;
        indecies[index ++] = x+1;
    }
    for(unsigned char y = 0; y < accuracyY; y ++) {
        referenceVertex = 1+accuracyX*y;
        for(unsigned char x = 0; x < accuracyX; x ++) {
            indecies[index ++] = referenceVertex+x;
            indecies[index ++] = (x < accuracyX-1) ? referenceVertex+x+1 : referenceVertex;
            indecies[index ++] = referenceVertex+accuracyX+x;
            indecies[index ++] = indecies[index-2];
            indecies[index ++] = (x < accuracyX-1) ? referenceVertex+accuracyX+x+1 : referenceVertex+accuracyX;
            indecies[index ++] = indecies[index-3];
        }
    }
    referenceVertex = 1+accuracyX*accuracyY;
    for(unsigned char x = 0; x < accuracyX-2; x ++) {
        indecies[index ++] = referenceVertex;
        indecies[index ++] = referenceVertex+x+1;
        indecies[index ++] = referenceVertex+x+2;
    }
    return index;
}

unsigned int Parabolid3::getVertices(Vector3 vertices[], unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int index = 0;
    vertices[index ++] = Vector3(0.0, 0.0, radius)*(*transformation);
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY*2+2)*M_PI, circleRadius = sinf(circleAngle), circleZ = cosf(circleAngle);
        for(unsigned char x = 0; x < accuracyX; x ++) {
            float angle = (float)x/accuracyX*M_PI*2.0;
            vertices[index ++] = Vector3(sinf(angle)*circleRadius, cosf(angle)*circleRadius, circleZ)*radius*(*transformation);
        }
    }
    for(unsigned char x = 0; x < accuracyX; x ++) {
        float angle = (float)x/accuracyX*M_PI*2.0;
        vertices[index ++] = Vector3(sinf(angle)*radius, cosf(angle)*radius, 0.0)*(*transformation);
    }
    return index;
}