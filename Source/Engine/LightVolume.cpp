//
//  LightVolume.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 23.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "LightVolume.h"

LightVolume::LightVolume() {
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
}

LightVolume::~LightVolume() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
}

void LightVolume::draw() {
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 3*sizeof(float), NULL);
}



#define boxTrianglesCount 12
#define boxVerticesCount 8

unsigned int boxWireFrame[] = {
    0, 1, 2, 3, 4, 5, 6, 7,
    0, 2, 1, 3, 4, 6, 5, 7,
    0, 4, 1, 5, 2, 6, 3, 7
};

LightBoxVolume::LightBoxVolume(btVector3 halfSizeB) :halfSize(halfSizeB) {
    float* vertices = new float[boxVerticesCount*3];
    for(unsigned char i = 0; i < boxVerticesCount; i ++) {
        vertices[i*3  ] = (i < 4) ? -halfSize.x() : halfSize.x();
        vertices[i*3+1] = (i % 4 < 2) ? -halfSize.y() : halfSize.y();
        vertices[i*3+2] = (i % 2 == 0) ? -halfSize.z() : halfSize.z();
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, boxVerticesCount*3*sizeof(float), vertices, GL_STATIC_DRAW);
    delete [] vertices;
    
    unsigned int indecies[boxTrianglesCount*3] = {
        0, 1, 2, 1, 3, 2,
        6, 5, 4, 6, 7, 5,
        4, 1, 0, 4, 5, 1,
        2, 3, 6, 3, 7, 6,
        6, 4, 0, 2, 6, 0,
        7, 3, 1, 5, 7, 1
    };
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indecies), indecies, GL_STATIC_DRAW);
}

void LightBoxVolume::drawWireFrame(Color4 color) {
    float* colors = new float[boxVerticesCount*3];
    for(unsigned int i = 0; i < 8; i ++) {
        colors[i*3  ] = color.r;
        colors[i*3+1] = color.g;
        colors[i*3+2] = color.b;
    }
    shaderPrograms[colorSP]->use();
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*3, NULL);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*3, colors);
    glDrawElements(GL_LINES, 24, GL_UNSIGNED_INT, boxWireFrame);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] colors;
}

void LightBoxVolume::draw() {
    LightVolume::draw();
    glDrawElements(GL_TRIANGLES, boxTrianglesCount*3, GL_UNSIGNED_INT, 0);
}



#define sphereTrianglesCount(accuracyX, accuracyY) (accuracyX*accuracyY*2)
#define sphereVerticesCount(accuracyX, accuracyY) (accuracyX*accuracyY+2)

LightSphereVolume::LightSphereVolume(btScalar radiusB, unsigned int accuracyXb, unsigned int accuracyYb)
:radius(radiusB), accuracyX(accuracyXb), accuracyY(accuracyYb) {
    unsigned int trianglesCount = sphereTrianglesCount(accuracyX, accuracyY),
                 verticesCount = sphereVerticesCount(accuracyX, accuracyY);
    float* vertices = new float[verticesCount*3];
    unsigned int index = 0;
    vertices[index ++] = 0.0;
    vertices[index ++] = 0.0;
    vertices[index ++] = radius;
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY+1)*M_PI, circleRadius = sinf(circleAngle), circleZ = cosf(circleAngle);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesCount*3*sizeof(float), vertices, GL_STATIC_DRAW);
    delete [] vertices;
    
    index = 0;
    unsigned int *indecies = new unsigned int[trianglesCount*3], referenceVertex;
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesCount*3*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    delete [] indecies;
}

void LightSphereVolume::drawWireFrame(Color4 color) {
    unsigned int verticesCount = sphereVerticesCount(accuracyX, accuracyY);
    float* colors = new float[verticesCount*3];
    for(unsigned int i = 0; i < 8; i ++) {
        colors[i*3  ] = color.r;
        colors[i*3+1] = color.g;
        colors[i*3+2] = color.b;
    }
    unsigned int indeciesCount = accuracyX*accuracyY*2+accuracyX*(accuracyY+1)*2, index = 0,
                *indecies = new unsigned int[indeciesCount];
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*3, NULL);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*3, colors);
    glDrawElements(GL_LINES, indeciesCount, GL_UNSIGNED_INT, indecies);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] colors;
    delete [] indecies;
}

void LightSphereVolume::draw() {
    LightVolume::draw();
    glDrawElements(GL_TRIANGLES, sphereTrianglesCount(accuracyX, accuracyY)*3, GL_UNSIGNED_INT, 0);
}



#define parabolidTrianglesCount(accuracyX, accuracyY) (accuracyX*2+accuracyX*accuracyY*2-2)
#define parabolidVerticesCount(accuracyX, accuracyY) (accuracyX+accuracyX*accuracyY+1)

LightParabolidVolume::LightParabolidVolume(btScalar radiusB, unsigned int accuracyXb, unsigned int accuracyYb)
:radius(radiusB), accuracyX(accuracyXb), accuracyY(accuracyYb) {
    unsigned int trianglesCount = parabolidTrianglesCount(accuracyX, accuracyY),
                 verticesCount = parabolidVerticesCount(accuracyX, accuracyY);
    float* vertices = new float[verticesCount*3];
    unsigned int index = 0;
    vertices[index ++] = 0.0;
    vertices[index ++] = 0.0;
    vertices[index ++] = radius;
    for(unsigned char y = 0; y < accuracyY; y ++) {
        float circleAngle = (float)(y+1)/(accuracyY*2+2)*M_PI, circleRadius = sinf(circleAngle), circleZ = cosf(circleAngle);
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesCount*3*sizeof(float), vertices, GL_STATIC_DRAW);
    delete [] vertices;
    
    index = 0;
    unsigned int *indecies = new unsigned int[trianglesCount*3], referenceVertex;
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
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, trianglesCount*3*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    delete [] indecies;
}

void LightParabolidVolume::drawWireFrame(Color4 color) {
    unsigned int verticesCount = sphereVerticesCount(accuracyX, accuracyY);
    float* colors = new float[verticesCount*3];
    for(unsigned int i = 0; i < 8; i ++) {
        colors[i*3  ] = color.r;
        colors[i*3+1] = color.g;
        colors[i*3+2] = color.b;
    }
    unsigned int indeciesCount = accuracyX*(accuracyY+1)*2+accuracyX*(accuracyY+1)*2, index = 0,
                *indecies = new unsigned int[indeciesCount];
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
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, sizeof(float)*3, NULL);
    currentShaderProgram->setAttribute(COLOR_ATTRIBUTE, 3, sizeof(float)*3, colors);
    glDrawElements(GL_LINES, indeciesCount, GL_UNSIGNED_INT, indecies);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(COLOR_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] colors;
    delete [] indecies;
}

void LightParabolidVolume::draw() {
    LightVolume::draw();
    glDrawElements(GL_TRIANGLES, parabolidTrianglesCount(accuracyX, accuracyY)*3, GL_UNSIGNED_INT, 0);
}