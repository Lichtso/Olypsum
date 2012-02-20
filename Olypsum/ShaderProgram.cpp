//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "ShaderProgram.h"

void ShaderProgram::loadShader(unsigned int shaderType, char *fileName) {
	FILE *fp = fopen(fileName, "rb");
	if(!fp) {
		printf("The file %s couldn't be found.", fileName);
		return;
	}
	
	if(GLname == 0)
        GLname = glCreateProgram();
	
	fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize];
	fread(data, 1, dataSize, fp);
	
	unsigned int shaderId = glCreateShader(shaderType);
	const GLchar* dataArray[1];
	dataArray[0] = data;
	glShaderSource(shaderId, 1, dataArray, (GLint*) &dataSize);
	glCompileShader(shaderId);
	int infoLogLength = 1024;
	char infoLog[infoLogLength];
	glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER %s\nERROR: %s", fileName, infoLog);
		return;
	}
	glAttachShader(GLname, shaderId);
	
	fclose(fp);
}

ShaderProgram::ShaderProgram () {
    
}

ShaderProgram::~ShaderProgram () {
	if(GLname == 0) return;
	glDeleteProgram(GLname);
}

void ShaderProgram::link(char** attributeNames, unsigned int attributeCount) {
	if(GLname == 0) return;
    
    for(unsigned int s = 0; s < attributeCount; s ++)
        glBindAttribLocation(GLname, s, attributeNames[s]);
	glLinkProgram(GLname);
    
    int infoLogLength = 1024;
	char infoLog[infoLogLength];
    glGetProgramInfoLog(GLname, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER PROGRAM\nERROR: %s", infoLog);
		return;
	}
}

void ShaderProgram::initSamplers(char** samplerNames, unsigned int samplerCount) {
    use();
    for(unsigned int s = 0; s < samplerCount; s ++)
        glUniform1i(glGetUniformLocation(GLname, samplerNames[s]), s);
}

void ShaderProgram::use () {
	if(GLname == 0) return;
	glUseProgram(GLname);
    currentShaderProgram = this;
}

void ShaderProgram::setAttribute(unsigned int index, unsigned int size, GLenum type, GLsizei stride, GLvoid* data) {
    glVertexAttribPointer(index, size, type, false, stride, data);
}

void ShaderProgram::setUnfiformI(char* name, int value) {
    glUniform1i(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUnfiformF(char* name, float value) {
    glUniform1f(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUnfiformMatrix3(char* name, btMatrix3x3* mat) {
    btScalar matData[16];
    mat->getOpenGLSubMatrix(matData);
    matData[3] = matData[4];
    matData[4] = matData[5];
    matData[5] = matData[6];
    matData[6] = matData[8];
    matData[7] = matData[9];
    matData[8] = matData[10];
    glUniformMatrix3fv(glGetUniformLocation(GLname, name), 9, false, matData);
}

void ShaderProgram::setUnfiformMatrix4(char* name, btTransform* mat) {
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(glGetUniformLocation(GLname, name), 16, false, matData);
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram &b) {
	GLname = b.GLname;
	return *this;
}

ShaderProgram* currentShaderProgram;