//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Light.h"
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
    bool loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*> macros);
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
extern ShaderProgram *shaderPrograms[31], *currentShaderProgram;
extern float screenBlurFactor;
extern bool edgeSmoothEnabled, fullScreenEnabled, cubemapsEnabled;
extern unsigned char depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, particleCalcTarget;

enum ShaderProgramNames {
    normalMapGenSP = 0,
    blurSP = 1,
    spotShadowCircleLightSP = 2,
    spriteSP = 3,
    colorSP = 4,
    solidGeometrySP = 5,
    solidBumpGeometrySP = 6,
    solidShadowSP = 7,
    solidParabolidShadowSP = 8,
    skeletalGeometrySP = 9,
    skeletalBumpGeometrySP = 10,
    skeletalShadowSP = 11,
    skeletalParabolidShadowSP = 12,
    directionalLightSP = 13,
    directionalShadowLightSP = 14,
    spotLightSP = 15,
    spotShadowLightSP = 16,
    positionalLightSP = 17,
    positionalShadowLightSP = 18,
    positionalShadowDualLightSP = 19,
    ssaoSP = 20,
    deferredCombineSP = 21,
    edgeSmoothSP = 22,
    depthOfFieldSP = 23,
    particleDrawSP = 24,
    particleCalculateSP = 25,
    glassGeometrySP = 26,
    glassBumpGeometrySP = 27,
    glassSkeletalGeometrySP = 28,
    glassSkeletalBumpGeometrySP = 29,
    waterSP = 30
};

void loadStaticShaderPrograms();
void loadDynamicShaderPrograms();

#endif
