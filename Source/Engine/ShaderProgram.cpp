//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

const char* seperatorString = "#|#|#";

ShaderProgram::ShaderProgram() {
    GLname = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    if(currentShaderProgram == this)
        currentShaderProgram = NULL;
    
	glDeleteProgram(GLname);
}

bool ShaderProgram::loadShader(GLuint shaderType, const char* soucreCode) {
	unsigned int shaderId = glCreateShader(shaderType);
	GLint dataSize = strlen(soucreCode);
	glShaderSource(shaderId, 1, (const GLchar**)&soucreCode, &dataSize);
	glCompileShader(shaderId);
	int infoLogLength = 1024;
	char infoLog[infoLogLength];
	glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER:\n%s%s\n\n", soucreCode, infoLog);
		return false;
	}
	glAttachShader(GLname, shaderId);
    glDeleteShader(shaderId);
    return true;
}

bool ShaderProgram::loadShaderProgram(const char* fileName) {
    std::string url("Shaders/");
    url += fileName;
    url += ".glsl";
    
    FILE *fp = fopen(url.c_str(), "rb");
	if(!fp) {
		printf("The file %s couldn't be found.", url.c_str());
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize+1];
	fread(data, 1, dataSize, fp);
    fclose(fp);
    
    char* seperator = strstr(data, seperatorString);
    if(!seperator) {
        printf("The file %s doesn't contain any seperators.", url.c_str());
        return false;
    }
    *seperator = 0;
    data[dataSize] = 0;
    
    loadShader(GL_VERTEX_SHADER, data);
	loadShader(GL_FRAGMENT_SHADER, seperator+strlen(seperatorString));
    
    return true;
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
    char buffer[64], samplerIndex;
    GLint location;
    for(samplerIndex = 0; samplerIndex < 4; samplerIndex ++) {
        sprintf(buffer, "sampler%d", samplerIndex);
        location = glGetUniformLocation(GLname, buffer);
        if(location < 0) break;
        glUniform1i(location, samplerIndex);
    }
    
    for(samplerIndex = 0; samplerIndex < maxLightCount*2; samplerIndex ++) {
        sprintf(buffer, "shadowMaps[%d]", samplerIndex);
        location = glGetUniformLocation(GLname, buffer);
        if(location < 0) break;
        glUniform1i(location, samplerIndex+3);
    }
}

void ShaderProgram::use () {
	glUseProgram(GLname);
    currentShaderProgram = this;
    setUniformMatrix4("modelMat", &modelMat);
    Matrix4 normalMat = Matrix4(modelMat).normalize();
    setUniformMatrix3("normalMat", &normalMat);
    if(currentCam) {
        setUniformMatrix4("viewMat", &currentCam->viewMat);
        setUniformMatrix4("shadowMat", &currentCam->shadowMat);
        Matrix4 modelViewMat(modelMat * currentCam->viewMat);
        setUniformMatrix4("modelViewMat", &modelViewMat);
    }
}

void ShaderProgram::setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, false, stride, data);
}

void ShaderProgram::setUniformI(const char* name, int value) {
    glUniform1i(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUniformF(const char* name, float value) {
    glUniform1f(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUniformVec3(const char* name, Vector3 value) {
    glUniform3f(glGetUniformLocation(GLname, name), value.x, value.y, value.z);
}

void ShaderProgram::setUniformMatrix3(const char* name, btMatrix3x3* mat) {
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

void ShaderProgram::setUniformMatrix3(const char* name, Matrix4* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[9];
    mat->getOpenGLMatrix3(matData);
    glUniformMatrix3fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, btTransform* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, Matrix4* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix4(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, Matrix4* mat, unsigned int count) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[count*16];
    for(unsigned int i = 0; i < count; i ++)
        mat[i].getOpenGLMatrix4(matData+i*16);
    glUniformMatrix4fv(location, count, false, matData);
}

ShaderProgram& ShaderProgram::operator=(const ShaderProgram &b) {
	GLname = b.GLname;
	return *this;
}

Matrix4 modelMat;
ShaderProgram *mainShaderProgram, *shadowShaderProgram,
              *mainSkeletonShaderProgram, *shadowSkeletonShaderProgram,
              *spriteShaderProgram, *currentShaderProgram;