//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import <OpenGL/gl.h>
#import <BulletDynamics/btBulletDynamicsCommon.h>
#import <string.h>
#import <stdio.h>

#ifndef ShaderProgram_h
#define ShaderProgram_h

class ShaderProgram {
	GLuint GLname;
    public:
	ShaderProgram();
	~ShaderProgram();
	void loadShader(unsigned int shaderType, char* fileName);
	void link(char** attributeNames, unsigned int attributeCount);
    void initSamplers(char** samplerNames, unsigned int samplerCount);
	void use();
    void setAttribute(unsigned int index, unsigned int size, GLenum type, GLsizei stride, GLvoid* data);
    void setUnfiformI(char* name, int value);
    void setUnfiformF(char* name, float value);
    void setUnfiformMatrix3(char* name, btMatrix3x3* mat);
    void setUnfiformMatrix4(char* name, btTransform* mat);
	ShaderProgram& operator=(const ShaderProgram &b);
};

extern ShaderProgram* currentShaderProgram;

#endif
