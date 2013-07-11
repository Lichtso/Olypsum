//
//  VertexArrayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() :vao(0), vbo(0), ibo(0) {
    drawType = GL_TRIANGLES;
}

VertexArrayObject::~VertexArrayObject() {
    if(vao == 0) return;
    glDeleteVertexArrays(1, &vao);
    if(ibo)
        glDeleteBuffers(2, &vbo);
    else
        glDeleteBuffers(1, &vbo);
}

void VertexArrayObject::init(std::vector<Attribute> attributes, bool indexMode) {
    glGenVertexArrays(1, &vao);
    if(indexMode)
        glGenBuffers(2, &vbo);
    else
        glGenBuffers(1, &vbo);
    
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    if(indexMode)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    GLsizei stride = 0, offset = 0;
    for(Attribute attribute : attributes)
        stride += attribute.size*sizeof(float);
    for(Attribute attribute : attributes) {
        glEnableVertexAttribArray(attribute.name);
        glVertexAttribPointer(attribute.name, attribute.size, GL_FLOAT, attribute.normalize, stride, reinterpret_cast<float*>(offset));
        offset += attribute.size*sizeof(float);
    }
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexArrayObject::updateVertices(unsigned int count, float* vertices, GLenum usage) {
    if(!vbo) return;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, count*sizeof(float), vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void VertexArrayObject::updateIndecies(unsigned int count, void* indecies, GLenum format, GLenum usage) {
    if(!ibo) return;
    elementsCount = count;
    indeciesFormat = format;
    GLsizei formatSize = 0;
    switch(format) {
        case GL_UNSIGNED_BYTE:
            formatSize = 1;
            break;
        case GL_UNSIGNED_SHORT:
            formatSize = 2;
            break;
        case GL_UNSIGNED_INT:
            formatSize = 4;
            break;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count*formatSize, indecies, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void VertexArrayObject::draw() {
    if(!vao || !vbo) return;
    glBindVertexArray(vao);
    if(ibo)
        glDrawElements(drawType, elementsCount, indeciesFormat, NULL);
    else
        glDrawArrays(drawType, 0, elementsCount);
    glBindVertexArray(0);
}

VertexArrayObject rectVAO;