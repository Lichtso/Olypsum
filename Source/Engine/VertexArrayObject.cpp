//
//  VertexArrayObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 11.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "VertexArrayObject.h"

VertexArrayObject::VertexArrayObject() :vao(0), vbo(0), ibo(0), drawType(GL_TRIANGLES) {
    
}

VertexArrayObject::~VertexArrayObject() {
    if(vao == 0) return;
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers((ibo) ? 2 : 1, &vbo);
}

void VertexArrayObject::init(std::vector<Attribute> attributes, bool indexMode) {
    glGenVertexArrays(1, &vao);
    glGenBuffers((indexMode) ? 2 : 1, &vbo);
    
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
    if(verticesCount == count)
        glBufferSubData(GL_ARRAY_BUFFER, 0, count*sizeof(float), vertices);
    else
        glBufferData(GL_ARRAY_BUFFER, count*sizeof(float), vertices, usage);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    verticesCount = count;
}

void VertexArrayObject::updateIndecies(unsigned int count, void* indecies, GLenum format, GLenum usage) {
    if(!ibo) return;
    GLint newSize = count;
    switch(format) {
        case GL_UNSIGNED_SHORT:
            newSize *= 2;
            break;
        case GL_UNSIGNED_INT:
            newSize *= 4;
            break;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    if(indeciesCount == count && indeciesFormat == format)
        glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, newSize, indecies);
    else
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, newSize, indecies, usage);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    indeciesCount = count;
    indeciesFormat = format;
}

void VertexArrayObject::draw() {
    if(!vao || !vbo) return;
    glBindVertexArray(vao);
    if(ibo)
        glDrawElements(drawType, indeciesCount, indeciesFormat, NULL);
    else
        glDrawArrays(drawType, 0, indeciesCount);
    glBindVertexArray(0);
}

VertexArrayObject rectVAO;