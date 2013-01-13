//
//  LightVolume.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "LightVolume.h"

void LightVolume::init() {
    std::vector<VertexArrayObject::Attribute> attributes;
    VertexArrayObject::Attribute attr;
    attr.size = 3;
    attr.name = POSITION_ATTRIBUTE;
    attributes.push_back(attr);
    vao.init(attributes, true);
    
    unsigned int verticesCount, trianglesCount;
    std::unique_ptr<float[]> vertices = getVertices(verticesCount);
    vao.updateVertices(verticesCount*3, vertices.get(), GL_STATIC_DRAW);
    std::unique_ptr<unsigned int[]> indecies = getIndecies(trianglesCount);
    vao.updateIndecies(trianglesCount*3, indecies.get(), GL_UNSIGNED_INT, GL_STATIC_DRAW);
}

void LightVolume::drawDebug(Color4 color) {
    shaderPrograms[colorSP]->use();
    shaderPrograms[colorSP]->setUniformVec3("color", color.getVector());
    vao.draw();
}

void LightVolume::draw() {
    vao.draw();
}



#define boxTrianglesCount 12
#define boxVerticesCount 8

unsigned int staticBoxIndecies[boxTrianglesCount*3] = {
    0, 1, 2, 1, 3, 2,
    6, 5, 4, 6, 7, 5,
    4, 1, 0, 4, 5, 1,
    2, 3, 6, 3, 7, 6,
    6, 4, 0, 2, 6, 0,
    7, 3, 1, 5, 7, 1
};

unsigned int staticBoxWireFrame[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 2, 1, 3, 4, 6, 5, 7,
    0, 4, 1, 5, 2, 6, 3, 7
};

LightBoxVolume::LightBoxVolume(btVector3 halfSizeB) :halfSize(halfSizeB) {
    
}

std::unique_ptr<float[]> LightBoxVolume::getVertices(unsigned int& verticesCount) {
    verticesCount = boxVerticesCount;
    std::unique_ptr<float[]> vertices(new float[verticesCount*3]);
    for(unsigned char i = 0; i < boxVerticesCount; i ++) {
        vertices[i*3  ] = (i < 4) ? -halfSize.x() : halfSize.x();
        vertices[i*3+1] = (i % 4 < 2) ? -halfSize.y() : halfSize.y();
        vertices[i*3+2] = (i % 2 == 0) ? -halfSize.z() : halfSize.z();
    }
    return vertices;
}

std::unique_ptr<unsigned int[]> LightBoxVolume::getIndecies(unsigned int& trianglesCount) {
    trianglesCount = boxTrianglesCount;
    std::unique_ptr<unsigned int[]> indecies(new unsigned int[trianglesCount*3]);
    memcpy(indecies.get(), staticBoxIndecies, sizeof(staticBoxIndecies));
    return indecies;
}



FrustumVolume::FrustumVolume(Ray3 boundsB[4], float lengthB) :length(lengthB) {
    bounds[0] = boundsB[0];
    bounds[1] = boundsB[1];
    bounds[2] = boundsB[2];
    bounds[3] = boundsB[3];
}

std::unique_ptr<float[]> FrustumVolume::getVertices(unsigned int& verticesCount) {
    verticesCount = boxVerticesCount;
    std::unique_ptr<float[]> vertices(new float[verticesCount*3]);
    for(unsigned char i = 0; i < 4; i ++) {
        vertices[i*6  ] = bounds[i].origin.x();
        vertices[i*6+1] = bounds[i].origin.y();
        vertices[i*6+2] = bounds[i].origin.z();
        vertices[i*6+3] = bounds[i].origin.x()+bounds[i].direction.x()*length;
        vertices[i*6+4] = bounds[i].origin.y()+bounds[i].direction.y()*length;
        vertices[i*6+5] = bounds[i].origin.z()+bounds[i].direction.z()*length;
    }
    return vertices;
}

std::unique_ptr<unsigned int[]> FrustumVolume::getIndecies(unsigned int& trianglesCount) {
    trianglesCount = boxTrianglesCount;
    std::unique_ptr<unsigned int[]> indecies(new unsigned int[trianglesCount*3]);
    memcpy(indecies.get(), staticBoxIndecies, sizeof(staticBoxIndecies));
    return indecies;
}



#define sphereTrianglesCount(accuracyX, accuracyY) (accuracyX*accuracyY*2)
#define sphereVerticesCount(accuracyX, accuracyY) (accuracyX*accuracyY+2)

LightSphereVolume::LightSphereVolume(float radiusB, unsigned int accuracyXb, unsigned int accuracyYb)
:radius(radiusB), accuracyX(accuracyXb), accuracyY(accuracyYb) {
    
}

std::unique_ptr<float[]> LightSphereVolume::getVertices(unsigned int& verticesCount) {
    verticesCount = sphereVerticesCount(accuracyX, accuracyY);
    std::unique_ptr<float[]> vertices(new float[verticesCount*3]);
    unsigned int index = 0;
    vertices[index ++] = 0.0;
    vertices[index ++] = 0.0;
    vertices[index ++] = radius;
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY+1)*M_PI, circleRadius = sinf(circleAngle)*radius, circleZ = cosf(circleAngle)*radius;
        for(unsigned char x = 0; x < accuracyX; x ++) {
            float angle = (float)x/accuracyX*M_PI*2.0;
            vertices[index ++] = sinf(angle)*circleRadius;
            vertices[index ++] = cosf(angle)*circleRadius;
            vertices[index ++] = circleZ;
        }
    }
    vertices[index ++] = 0.0;
    vertices[index ++] = 0.0;
    vertices[index ++] = -radius;
    return vertices;
}

std::unique_ptr<unsigned int[]> LightSphereVolume::getIndecies(unsigned int& trianglesCount) {
    trianglesCount = sphereTrianglesCount(accuracyX, accuracyY);
    std::unique_ptr<unsigned int[]> indecies(new unsigned int[trianglesCount*3]);
    unsigned int index = 0, referenceVertex;
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
    return indecies;
}



#define parabolidTrianglesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2-2)
#define parabolidVerticesCount(accuracyX, accuracyY) (accuracyX+accuracyX*accuracyY+1)

LightParabolidVolume::LightParabolidVolume(float radiusB, unsigned int accuracyXb, unsigned int accuracyYb)
:radius(radiusB), accuracyX(accuracyXb), accuracyY(accuracyYb) {
    
}

std::unique_ptr<float[]> LightParabolidVolume::getVertices(unsigned int& verticesCount) {
    verticesCount = parabolidVerticesCount(accuracyX, accuracyY);
    std::unique_ptr<float[]> vertices(new float[verticesCount*3]);
    unsigned int index = 0;
    vertices[index ++] = 0.0;
    vertices[index ++] = 0.0;
    vertices[index ++] = radius;
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY*2+2)*M_PI, circleRadius = sinf(circleAngle)*radius, circleZ = cosf(circleAngle)*radius;
        for(unsigned char x = 0; x < accuracyX; x ++) {
            float angle = (float)x/accuracyX*M_PI*2.0;
            vertices[index ++] = sinf(angle)*circleRadius;
            vertices[index ++] = cosf(angle)*circleRadius;
            vertices[index ++] = circleZ;
        }
    }
    for(unsigned char x = 0; x < accuracyX; x ++) {
        float angle = (float)x/accuracyX*M_PI*2.0;
        vertices[index ++] = sinf(angle)*radius;
        vertices[index ++] = cosf(angle)*radius;
        vertices[index ++] = 0.0;
    }
    return vertices;
}

std::unique_ptr<unsigned int[]> LightParabolidVolume::getIndecies(unsigned int& trianglesCount) {
    trianglesCount = parabolidTrianglesCount(accuracyX, accuracyY);
    std::unique_ptr<unsigned int[]> indecies(new unsigned int[trianglesCount*3]);
    unsigned int index = 0, referenceVertex;
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
    return indecies;
}