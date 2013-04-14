//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Matrix4.h"
#include "Links.h"

#ifndef ShaderProgram_h
#define ShaderProgram_h

//! A class to store a color used for graphics
class Color4 {
    public:
    float r, //!< The red channel (0.0 - 1.0)
          g, //!< The green channel (0.0 - 1.0)
          b, //!< The blue channel (0.0 - 1.0)
          a; //!< The transparency channel (0.0 - 1.0)
    //! A black color
    Color4() : r(0), g(0), b(0), a(1) {};
    //! A gray color
    Color4(float gray) : r(gray), g(gray), b(gray), a(1) {};
    //! A transparent gray color
    Color4(float gray, float aB) : r(gray), g(gray), b(gray), a(aB) {};
    //! A rgb color
    Color4(float rB, float gB, float bB) : r(rB), g(gB), b(bB), a(1) {};
    //! A rgb color
    Color4(btVector3 vec) : r(vec.x()), g(vec.y()), b(vec.z()), a(1) {};
    //! A transparent rgb color
    Color4(float rB, float gB, float bB, float aB) : r(rB), g(gB), b(bB), a(aB) {};
    Color4& operator=(const Color4& B);
    //! Converts this Color4 to a btVector3
    btVector3 getVector();
    //! Converts this Color4 to a SDL_Color
    SDL_Color getSDL();
};

#define POSITION_ATTRIBUTE 0
#define TEXTURE_COORD_ATTRIBUTE 1
#define COLOR_ATTRIBUTE 1
#define NORMAL_ATTRIBUTE 2
#define WEIGHT_ATTRIBUTE 3
#define JOINT_ATTRIBUTE 4
#define VELOCITY_ATTRIBUTE 5

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
    void setUniformMatrix3(const char* name, const btMatrix3x3* mat, bool transpose = true);
    //! Sets a mat3uniform
    void setUniformMatrix3(const char* name, const btTransform* mat, bool transpose = true);
    //! Sets a mat4 uniform
    void setUniformMatrix4(const char* name, const Matrix4* mat);
    //! Sets a mat4 uniform
    void setUniformMatrix4(const char* name, const btTransform* mat);
    //! Sets a mat4 array uniform
    void setUniformMatrix4(const char* name, const btTransform* mat, unsigned int count);
};

extern btTransform modelMat;
extern ShaderProgram *shaderPrograms[79], *currentShaderProgram;

enum ShaderProgramNames {
    //Static Shaders
    normalMapGenSP = 0,
    blurSP = 1,
    spotShadowCircleLightSP = 2,
    spriteSP = 3,
    colorSP = 4,
    deferredCombineSP = 5,
    //G-Buffer Shaders
    solidGSP = 6,
    skeletalGSP,
    solidAnimatedGSP,
    skeletalAnimatedGSP,
    solidBumpGSP,
    skeletalBumpGSP,
    solidAnimatedBumpGSP,
    skeletalAnimatedBumpGSP,
    //G-Buffer Shaders (Glass)
    solidGlassGSP = 14,
    skeletalGlassGSP,
    solidAnimatedGlassGSP,
    skeletalAnimatedGlassGSP,
    solidBumpGlassGSP,
    skeletalBumpGlassGSP,
    solidAnimatedBumpGlassGSP,
    skeletalAnimatedBumpGlassGSP,
    //G-Buffer Shaders (Mirror)
    solidMirrorGSP = 22,
    skeletalMirrorGSP,
    solidAnimatedMirrorGSP,
    skeletalAnimatedMirrorGSP,
    solidBumpMirrorGSP,
    skeletalBumpMirrorGSP,
    solidAnimatedBumpMirrorGSP,
    skeletalAnimatedBumpMirrorGSP,
    //G-Buffer Shaders (Glass, Mirror)
    solidGlassMirrorGSP = 30,
    skeletalGlassMirrorGSP,
    solidAnimatedGlassMirrorGSP,
    skeletalAnimatedGlassMirrorGSP,
    solidBumpGlassMirrorGSP,
    skeletalBumpGlassMirrorGSP,
    solidAnimatedBumpGlassMirrorGSP,
    skeletalAnimatedBumpGlassMirrorGSP,
    //G-Buffer Shaders (Reflection)
    solidReflectionGSP = 38,
    skeletalReflectionGSP,
    solidAnimatedReflectionGSP,
    skeletalAnimatedReflectionGSP,
    solidBumpReflectionGSP,
    skeletalBumpReflectionGSP,
    solidAnimatedBumpReflectionGSP,
    skeletalAnimatedBumpReflectionGSP,
    //G-Buffer Shaders (Glass, Reflection)
    solidGlassReflectionGSP = 46,
    skeletalGlassReflectionGSP,
    solidAnimatedGlassReflectionGSP,
    skeletalAnimatedGlassReflectionGSP,
    solidBumpGlassReflectionGSP,
    skeletalBumpGlassReflectionGSP,
    solidAnimatedBumpGlassReflectionGSP,
    skeletalAnimatedBumpGlassReflectionGSP,
    waterGSP = 54,
    terrainGSP = 55,
    //Shadow Map Generators
    solidShadowSP = 56,
    skeletalShadowSP = 57,
    solidAnimatedShadowSP = 58,
    skeletalAnimatedShadowSP = 59,
    solidParabolidShadowSP = 60,
    skeletalParabolidShadowSP = 61,
    solidAnimatedParabolidShadowSP = 62,
    skeletalAnimatedParabolidShadowSP = 63,
    //Illumination Shaders
    directionalLightSP = 64,
    directionalShadowLightSP = 65,
    spotLightSP = 66,
    spotShadowLightSP = 67,
    positionalLightSP = 68,
    positionalShadowLightSP = 69,
    positionalShadowDualLightSP = 70,
    //Post Effect Shaders
    ssaoSP = 71,
    ssaoCombineSP = 72,
    deferredCombineTransparentSP = 73,
    edgeSmoothSP = 74,
    depthOfFieldSP = 75,
    particleCalculateSP = 76,
    particleDrawSP = 77,
    particleDrawAnimatedSP = 78
};

//! Compiles all shader programs which are not influenced by graphic options
void loadStaticShaderPrograms();

//! (Re)compiles all shader programs which change their behavior with graphic options
void loadDynamicShaderPrograms();

#endif
