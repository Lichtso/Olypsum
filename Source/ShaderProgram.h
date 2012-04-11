//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Localization.h"
#import <string.h>
#import <stdio.h>

#ifndef ShaderProgram_h
#define ShaderProgram_h

#define POSITION_ATTRIBUTE 0
#define TEXTURE_COORD_ATTRIBUTE 1
#define NORMAL_ATTRIBUTE 2
#define TANGENT_ATTRIBUTE 3
#define BITANGENT_ATTRIBUTE 4

class ShaderProgram {
    public:
	GLuint GLname;
	ShaderProgram();
	~ShaderProgram();
	bool loadShader(GLuint shaderType, const char* soucreCode);
    bool loadShaderProgram(const char* fileName);
    void addAttribute(unsigned int index, const char* attributeName);
	void link();
	void use();
    void setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data);
    void setUnfiformI(const char* name, int value);
    void setUnfiformF(const char* name, float value);
    void setUnfiformMatrix3(const char* name, btMatrix3x3* mat);
    void setUnfiformMatrix3(const char* name, Matrix4* mat);
    void setUnfiformMatrix4(const char* name, btTransform* mat);
    void setUnfiformMatrix4(const char* name, Matrix4* mat);
	ShaderProgram& operator=(const ShaderProgram &b);
};

extern Matrix4 modelMat;
extern ShaderProgram *mainShaderProgram, *spriteShaderProgram, *shadowShaderProgram, *currentShaderProgram;

#endif
