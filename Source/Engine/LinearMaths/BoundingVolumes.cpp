//
//  BoundingVolumes.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"
#import "Cam.h"

unsigned int boxIndecies[36] = {
    0, 1, 2, 1, 3, 2,
    6, 5, 4, 6, 7, 5,
    4, 1, 0, 4, 5, 1,
    2, 3, 6, 3, 7, 6,
    6, 4, 0, 2, 6, 0,
    7, 3, 1, 5, 7, 1
};

unsigned int boxWireFrame[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 2, 1, 3, 4, 6, 5, 7,
    0, 4, 1, 5, 2, 6, 3, 7
};

Aabb3::Aabb3(Vector3 minB, Vector3 maxB) : min(minB), max(maxB) { }

unsigned int Aabb3::getVertices(Vector3 vertices[boxVerticesCount]) {
    vertices[0] = Vector3(min.x, min.y, min.z);
    vertices[1] = Vector3(min.x, min.y, max.z);
    vertices[2] = Vector3(min.x, max.y, min.z);
    vertices[3] = Vector3(min.x, max.y, max.z);
    vertices[4] = Vector3(max.x, min.y, min.z);
    vertices[5] = Vector3(max.x, min.y, max.z);
    vertices[6] = Vector3(max.x, max.y, min.z);
    vertices[7] = Vector3(max.x, max.y, max.z);
    return 8;
}

void Aabb3::drawWireFrame(Vector3 color) {
    Vector3 vertices[8];
    getVertices(vertices);
    if(!currentCam->frustum.testPolyhedronInclusiveHit(vertices, 8)) return;
    modelMat.setIdentity();
    float data[48];
    for(unsigned int i = 0; i < 8; i ++) {
        data[i*6+0] = vertices[i].x;
        data[i*6+1] = vertices[i].y;
        data[i*6+2] = vertices[i].z;
        data[i*6+3] = color.x;
        data[i*6+4] = color.y;
        data[i*6+5] = color.z;
    }
    shaderPrograms[colorSP]->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, &data[0]);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &data[3]);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, boxWireFrame);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
}

void Aabb3::getPlanes(Plane3 planes[6]) {
    planes[0].set(min, Vector3(-1, 0, 0));
    planes[1].set(min, Vector3(0, -1, 0));
    planes[2].set(min, Vector3(0, 0, -1));
    planes[3].set(max, Vector3(1, 0, 0));
    planes[4].set(max, Vector3(0, 1, 0));
    planes[5].set(max, Vector3(0, 0, 1));
}

bool Aabb3::testPointHit(Vector3 pos) {
    return pos >= min && pos <= max;
}

bool Aabb3::testAabbHit(Aabb3* aabb) {
    return Vector3(fmax(min.x, aabb->min.x), fmax(min.y, aabb->min.y), fmax(min.z, aabb->min.z))
         < Vector3(fmin(max.x, aabb->max.x), fmin(max.y, aabb->max.y), fmin(max.z, aabb->max.z));
}



Box3::Box3(Matrix4* trans, Vector3 minB, Vector3 maxB) : BoundingVolume(trans), min(minB), max(maxB) { }

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

void Box3::drawWireFrame(Vector3 color) {
    Vector3 vertices[8];
    getVertices(vertices);
    if(!currentCam->frustum.testPolyhedronInclusiveHit(vertices, 8)) return;
    modelMat.setIdentity();
    float data[48];
    for(unsigned int i = 0; i < 8; i ++) {
        data[i*6+0] = vertices[i].x;
        data[i*6+1] = vertices[i].y;
        data[i*6+2] = vertices[i].z;
        data[i*6+3] = color.x;
        data[i*6+4] = color.y;
        data[i*6+5] = color.z;
    }
    shaderPrograms[colorSP]->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, &data[0]);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &data[3]);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, boxWireFrame);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
}

void Box3::getPlanes(Plane3 planes[6]) {
    Matrix4 rotMat;
    rotMat.setMatrix3(*transformation);
    
    Vector3 normal = Vector3(-1, 0, 0)*(*transformation);
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



Bs3::Bs3(Matrix4* trans, float radiusB) : BoundingVolume(trans), radius(radiusB) { }

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

void Bs3::drawWireFrame(Vector3 color, unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int verticesCount = sphereVerticesCount(accuracyX, accuracyY);
    Vector3* vertices = new Vector3[verticesCount];
    getVertices(vertices, accuracyX, accuracyY);
    if(!currentCam->frustum.testPolyhedronInclusiveHit(vertices, verticesCount)) {
        delete [] vertices;
        return;
    }
    modelMat.setIdentity();
    float data[verticesCount*6];
    for(unsigned int i = 0; i < verticesCount; i ++) {
        data[i*6+0] = vertices[i].x;
        data[i*6+1] = vertices[i].y;
        data[i*6+2] = vertices[i].z;
        data[i*6+3] = color.x;
        data[i*6+4] = color.y;
        data[i*6+5] = color.z;
    }
    unsigned int indeciesCount = accuracyX*accuracyY*2+accuracyX*(accuracyY+1)*2, indecies[indeciesCount], index = 0;
    for(unsigned int y = 0; y < accuracyY; y ++) {
        for(unsigned int x = 0; x < accuracyX-1; x ++) {
            indecies[index ++] = 1+accuracyX*y+x;
            indecies[index ++] = 2+accuracyX*y+x;
        }
        indecies[index ++] = 1+accuracyX*y;
        indecies[index ++] = accuracyX*y+accuracyX;
    }
    for(unsigned int x = 0; x < accuracyX; x ++) {
        indecies[index ++] = 0;
        for(unsigned int y = 0; y < accuracyY; y ++) {
            indecies[index ++] = 1+accuracyX*y+x;
            indecies[index ++] = 1+accuracyX*y+x;
        }
        indecies[index ++] = verticesCount-1;
    }
    shaderPrograms[colorSP]->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, &data[0]);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &data[3]);
    glDrawElements(GL_LINES, indeciesCount, GL_UNSIGNED_INT, indecies);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    delete [] vertices;
}

bool Bs3::testPointHit(Vector3 pos) {
    return (transformation->pos-pos).getLength() < radius;
}

bool Bs3::testBsHit(Bs3* bs) {
    return (transformation->pos-bs->transformation->pos).getLength() < radius+bs->radius;
}



Parabolid3::Parabolid3(Matrix4* trans, float radiusB) : BoundingVolume(trans), radius(radiusB) { }

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

void Parabolid3::drawWireFrame(Vector3 color, unsigned char accuracyX, unsigned char accuracyY) {
    unsigned int verticesCount = parabolidVerticesCount(accuracyX, accuracyY);
    Vector3* vertices = new Vector3[verticesCount];
    getVertices(vertices, accuracyX, accuracyY);
    if(!currentCam->frustum.testPolyhedronInclusiveHit(vertices, verticesCount)) {
        delete [] vertices;
        return;
    }
    modelMat.setIdentity();
    float data[verticesCount*6];
    for(unsigned int i = 0; i < verticesCount; i ++) {
        data[i*6+0] = vertices[i].x;
        data[i*6+1] = vertices[i].y;
        data[i*6+2] = vertices[i].z;
        data[i*6+3] = color.x;
        data[i*6+4] = color.y;
        data[i*6+5] = color.z;
    }
    unsigned int indeciesCount = accuracyX*(accuracyY+1)*2+accuracyX*(accuracyY+1)*2, indecies[indeciesCount], index = 0;
    for(unsigned int y = 0; y <= accuracyY; y ++) {
        for(unsigned int x = 0; x < accuracyX-1; x ++) {
            indecies[index ++] = 1+accuracyX*y+x;
            indecies[index ++] = 2+accuracyX*y+x;
        }
        indecies[index ++] = 1+accuracyX*y;
        indecies[index ++] = accuracyX*y+accuracyX;
    }
    for(unsigned int x = 0; x < accuracyX; x ++) {
        indecies[index ++] = 0;
        for(unsigned int y = 0; y < accuracyY; y ++) {
            indecies[index ++] = 1+accuracyX*y+x;
            indecies[index ++] = 1+accuracyX*y+x;
        }
        indecies[index ++] = 1+accuracyX*accuracyY+x;
    }
    shaderPrograms[colorSP]->use();
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*6, &data[0]);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*6, &data[3]);
    glDrawElements(GL_LINES, indeciesCount, GL_UNSIGNED_INT, indecies);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    delete [] vertices;
}