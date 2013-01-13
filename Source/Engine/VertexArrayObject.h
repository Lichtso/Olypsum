//
//  VertexArrayObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 11.01.13.
//
//

#include "Texture.h"

#ifndef VertexArrayObject_h
#define VertexArrayObject_h

//! A universal class to manage OpenGL VAOs, VBOs and IBOs
class VertexArrayObject {
    GLuint vao, vbo, ibo; //!< The OpenGL names
    GLenum indeciesFormat; //!< The type of the indecies if available
    public:
    //! Attribute for the ShaderProgram
    struct Attribute {
        GLint name; //!< Name of the attribute
        GLuint size; //!< Size in elements which it contains
        bool normalize = false; //!< If true than OpenGL will normalize it
    };
    unsigned int elementsCount; //!< The count of elemnets to be drawn
    GLenum drawType; //!< The type of 3D elements to be drawn
    VertexArrayObject();
    ~VertexArrayObject();
    /*! Ititialize
     @param attributes A list of attributes for the ShaderProgram
     @param indexMode True if IBO will be used
     */
    void init(std::vector<Attribute> attributes, bool indexMode);
    /*! Update the VBO
     @param count The count of elements in the buffer
     @param vertices A pointer to the array which will be copied to fill the buffer
     @param usage GL_STREAM_DRAW, GL_STATIC_DRAW or GL_DYNAMIC_DRAW
     */
    void updateVertices(unsigned int count, float* vertices, GLenum usage);
    /*! Update the VBO
     @param count The count of elements in the buffer
     @param indecies A pointer to the array which will be copied to fill the buffer
     @param format GL_UNSIGNED_BYTE, GL_UNSIGNED_SHORT or GL_UNSIGNED_INT
     @param usage GL_STREAM_DRAW, GL_STATIC_DRAW or GL_DYNAMIC_DRAW
     */
    void updateIndecies(unsigned int count, void* indecies, GLenum format, GLenum usage);
    //! Draws the buffers
    void draw();
};

#endif
