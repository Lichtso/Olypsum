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
#define COLOR_ATTRIBUTE 1
#define NORMAL_ATTRIBUTE 2
#define WEIGHT_ATTRIBUTE 3
#define JOINT_ATTRIBUTE 4
#define VELOCITY_ATTRIBUTE 5

class ShaderProgram {
    public:
	GLuint GLname;
	ShaderProgram();
	~ShaderProgram();
	bool loadShader(GLuint shaderType, const char* soucreCode, std::vector<const char*>* macros);
    bool loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*>* macros);
    void addAttribute(unsigned int index, const char* attributeName);
	void link();
	void use();
    void setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data);
    void setTransformFeedbackVaryings(unsigned int count, const char** varyings);
    bool checkUniformExistence(const char* name);
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
extern ShaderProgram *shaderPrograms[26], *currentShaderProgram;
extern bool edgeSmoothEnabled, fullScreenEnabled, cubeShadowsEnabled;
extern unsigned char depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, particleQuality;

enum ShaderProgramNames {
    spriteSP = 0,
    solidGeometrySP = 1,
    solidBumpGeometrySP = 2,
    solidShadowSP = 3,
    solidParabolidShadowSP = 4,
    skeletalGeometrySP = 5,
    skeletalBumpGeometrySP = 6,
    skeletalShadowSP = 7,
    skeletalParabolidShadowSP = 8,
    directionalLightSP = 9,
    directionalShadowLightSP = 10,
    spotLightSP = 11,
    spotShadowLightSP = 12,
    spotShadowCircleLightSP = 13,
    positionalLightSP = 14,
    positionalShadowLightSP = 15,
    positionalShadowDualLightSP = 16,
    normalMapGenSP = 17,
    blurSP = 18,
    ssaoSP = 19,
    deferredCombineSP = 20,
    edgeSmoothSP = 21,
    depthOfFieldSP = 22,
    colorSP = 23,
    particleDrawSP = 24,
    particleCalculateSP = 25
};

void loadShaderPrograms();

#endif
