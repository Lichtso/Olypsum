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
#define WEIGHT_ATTRIBUTE 5
#define JOINT_ATTRIBUTE 6

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
    void setUniformI(const char* name, int value);
    void setUniformF(const char* name, float value);
    void setUniformVec3(const char* name, Vector3 value);
    void setUniformMatrix3(const char* name, btMatrix3x3* mat);
    void setUniformMatrix3(const char* name, Matrix4* mat);
    void setUniformMatrix4(const char* name, btTransform* mat);
    void setUniformMatrix4(const char* name, Matrix4* mat);
    void setUniformMatrix4(const char* name, Matrix4* mat, unsigned int count);
	ShaderProgram& operator=(const ShaderProgram &b);
};

extern Matrix4 modelMat;
extern ShaderProgram *mainShaderProgram, *shadowShaderProgram,
                     *mainSkeletonShaderProgram, *shadowSkeletonShaderProgram,
                     *spriteShaderProgram, *blurShaderProgram, *currentShaderProgram;

#endif