//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

void ShaderProgram::loadShader(unsigned int shaderType, const char *fileName) {
	FILE *fp = fopen(fileName, "rb");
	if(!fp) {
		printf("The file %s couldn't be found.", fileName);
		return;
	}
	fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize];
	fread(data, 1, dataSize, fp);
    fclose(fp);
	
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
    glDeleteShader(shaderId);
}

ShaderProgram::ShaderProgram () {
    GLname = glCreateProgram();
}

ShaderProgram::~ShaderProgram () {
    if(currentShaderProgram == this)
        currentShaderProgram = NULL;
    
	glDeleteProgram(GLname);
}

void ShaderProgram::addAttribute(unsigned int index, const char* attributeName) {
    glBindAttribLocation(GLname, index, attributeName);
}

void ShaderProgram::link() {
	glLinkProgram(GLname);
    
    int infoLogLength = 1024;
	char infoLog[infoLogLength];
    glGetProgramInfoLog(GLname, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER PROGRAM\nERROR: %s", infoLog);
		return;
	}
    
    glUseProgram(GLname);
    char buffer[64], samplerIndex = 0;
    GLint location;
    while(samplerIndex < 255) {
        sprintf(buffer, "sampler%d", samplerIndex);
        location = glGetUniformLocation(GLname, buffer);
        if(location < 0) break;
        glUniform1i(location, samplerIndex);
        samplerIndex ++;
    }
}

void ShaderProgram::use () {
	glUseProgram(GLname);
    currentShaderProgram = this;
    setUnfiformMatrix4("modelMat", &modelMat);
    setUnfiformMatrix3("normalMat", &modelMat);
    if(currentCam) {
        setUnfiformMatrix4("viewMat", &currentCam->viewMat);
        setUnfiformMatrix4("shadowMat", &currentCam->shadowMat);
        Matrix4 modelViewMat(modelMat * currentCam->viewMat);
        setUnfiformMatrix4("modelViewMat", &modelViewMat);
    }
}

void ShaderProgram::setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, false, stride, data);
}

void ShaderProgram::setUnfiformI(const char* name, int value) {
    glUniform1i(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUnfiformF(const char* name, float value) {
    glUniform1f(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUnfiformMatrix3(const char* name, btMatrix3x3* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLSubMatrix(matData);
    matData[3] = matData[4];
    matData[4] = matData[5];
    matData[5] = matData[6];
    matData[6] = matData[8];
    matData[7] = matData[9];
    matData[8] = matData[10];
    glUniformMatrix3fv(location, 1, false, matData);
}

void ShaderProgram::setUnfiformMatrix3(const char* name, Matrix4* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[9];
    mat->getOpenGLMatrix3(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUnfiformMatrix4(const char* name, btTransform* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUnfiformMatrix4(const char* name, Matrix4* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix4(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram &b) {
	GLname = b.GLname;
	return *this;
}

Matrix4 modelMat;
ShaderProgram *mainShaderProgram, *spriteShaderProgram, *shadowShaderProgram, *currentShaderProgram;