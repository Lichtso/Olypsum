//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef ShaderProgram_h
#define ShaderProgram_h
#define POSITION_ATTRIBUTE 0
#define TEXTURE_COORD_ATTRIBUTE 1
#define COLOR_ATTRIBUTE 1
#define NORMAL_ATTRIBUTE 2
#define WEIGHT_ATTRIBUTE 3
#define JOINT_ATTRIBUTE 4
#define VELOCITY_ATTRIBUTE 5

#include "Links.h"

//! A shader program used for graphics
class ShaderProgram {
    std::map<const char*, GLint> cachedUniforms;
    public:
	GLuint GLname; //!< The OpenGL identifier
	ShaderProgram();
	~ShaderProgram();
    //! Loads a single shader and compiles it. This method is automaticly called by loadShaderProgram()
	bool loadShader(GLuint shaderType, const char* soucreCode, std::vector<const char*>& macros);
    /*! Loads all shaders from a file
     @param fileName The file which contains the shaders
     @param shaderTypes A array of shader types one entry for each shader
     @param macros A array of macros to be inserted in every shader
     @return Success
     */
    bool loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*> macros);
    //! Sets the a attribute that will be linked
    void addAttribute(unsigned int index, const char* name);
    //! Sets the fragment buffers that will be linked
    void addFragDataLocations(std::vector<const char*> names);
    //! Sets the transform feedback varings that will be linked
    void setTransformFeedbackVaryings(unsigned int count, const char** varyings);
    //! Links all shaders to a complete shader programm, returns success
	bool link();
    //! Sets this ShaderProgram as the currentShaderProgram and updates the transformation uniforms
	void use();
    //! Is this ShaderProgram linked and ready
	bool linked();
    //! Looks up the GLname of a given uniform
    GLint getUniformLocation(const char* name);
    //! Checks if a uniform existis
    bool checkUniformExistence(const char* name);
    //! Sets a single integer uniform
    void setUniformI(const char* name, int value);
    //! Sets a single float uniform
    void setUniformF(const char* name, btScalar value);
    //! Sets a vec2 uniform
    void setUniformVec2(const char* name, btScalar x, btScalar y);
    //! Sets a vec3 uniform
    void setUniformVec3(const char* name, btVector3 value);
    //! Sets a vec4 uniform
    void setUniformVec4(const char* name, btVector3 value);
    //! Sets a mat3 uniform
    void setUniformMatrix3(const char* name, const btMatrix3x3* mat);
    //! Sets a mat3uniform
    void setUniformMatrix3(const char* name, const btTransform* mat);
    //! Sets a mat4 uniform
    void setUniformMatrix4(const char* name, const Matrix4* mat);
    //! Sets a mat4 uniform
    void setUniformMatrix4(const char* name, const btTransform* mat);
    //! Sets a mat4 array uniform
    void setUniformMatrix4(const char* name, const btTransform* mat, unsigned int count);
};

extern btTransform modelMat; //!< The model transformation to be set in the next ShaderProgram::use() call
extern ShaderProgram *shaderPrograms[81], //!< All available ShaderPrograms
                     *currentShaderProgram; //!< The active ShaderProgram

enum ShaderProgramName {
    //Static Shaders
    normalMapGenSP = 0,
    blurSP,
    circleMaskSP,
    spriteSP,
    monochromeSP,
    deferredCombineSP0,
    deferredCombine1SP,
    deferredCombine2SP,
    //G-Buffer Shaders
    solidGSP = 8,
    skeletalGSP,
    solidAnimatedGSP,
    skeletalAnimatedGSP,
    solidBumpGSP,
    skeletalBumpGSP,
    solidAnimatedBumpGSP,
    skeletalAnimatedBumpGSP,
    //G-Buffer Shaders (Glass)
    solidGlassGSP = 16,
    skeletalGlassGSP,
    solidAnimatedGlassGSP,
    skeletalAnimatedGlassGSP,
    solidBumpGlassGSP,
    skeletalBumpGlassGSP,
    solidAnimatedBumpGlassGSP,
    skeletalAnimatedBumpGlassGSP,
    //G-Buffer Shaders (Mirror)
    solidMirrorGSP = 24,
    skeletalMirrorGSP,
    solidAnimatedMirrorGSP,
    skeletalAnimatedMirrorGSP,
    solidBumpMirrorGSP,
    skeletalBumpMirrorGSP,
    solidAnimatedBumpMirrorGSP,
    skeletalAnimatedBumpMirrorGSP,
    //G-Buffer Shaders (Glass, Mirror)
    solidGlassMirrorGSP = 32,
    skeletalGlassMirrorGSP,
    solidAnimatedGlassMirrorGSP,
    skeletalAnimatedGlassMirrorGSP,
    solidBumpGlassMirrorGSP,
    skeletalBumpGlassMirrorGSP,
    solidAnimatedBumpGlassMirrorGSP,
    skeletalAnimatedBumpGlassMirrorGSP,
    //G-Buffer Shaders (Reflection)
    solidReflectionGSP = 40,
    skeletalReflectionGSP,
    solidAnimatedReflectionGSP,
    skeletalAnimatedReflectionGSP,
    solidBumpReflectionGSP,
    skeletalBumpReflectionGSP,
    solidAnimatedBumpReflectionGSP,
    skeletalAnimatedBumpReflectionGSP,
    //G-Buffer Shaders (Glass, Reflection)
    solidGlassReflectionGSP = 48,
    skeletalGlassReflectionGSP,
    solidAnimatedGlassReflectionGSP,
    skeletalAnimatedGlassReflectionGSP,
    solidBumpGlassReflectionGSP,
    skeletalBumpGlassReflectionGSP,
    solidAnimatedBumpGlassReflectionGSP,
    skeletalAnimatedBumpGlassReflectionGSP,
    waterGSP,
    terrainGSP,
    //Shadow Map Generators
    solidShadowSP = 58,
    skeletalShadowSP,
    solidAnimatedShadowSP,
    skeletalAnimatedShadowSP,
    solidParabolidShadowSP,
    skeletalParabolidShadowSP,
    solidAnimatedParabolidShadowSP,
    skeletalAnimatedParabolidShadowSP,
    //Illumination Shaders
    directionalLightSP = 66,
    directionalShadowLightSP,
    spotLightSP,
    spotShadowLightSP,
    positionalLightSP,
    positionalShadowLightSP,
    positionalShadowDualLightSP,
    //Post Effect Shaders
    ssaoSP = 73,
    ssaoCombineSP,
    edgeSmoothSP,
    depthOfFieldSP,
    depthOfFieldFogSP,
    advanceParticlesSP,
    particlesSP,
    particlesAnimatedSP
};

//! Compiles all shader programs which are not influenced by any graphic options
void loadStaticShaderPrograms();

//! (Re)compiles all gBuffer related shader programs
void updateGBufferShaderPrograms();

//! (Re)compiles all shadow casting light sources related shader programs
void updateIlluminationShaderPrograms();

//! (Re)compiles all SSAO related shader programs
void updateSSAOShaderPrograms();

//! (Re)compiles the depthOfFieldSP
void updateDOFShaderProgram();

#endif
