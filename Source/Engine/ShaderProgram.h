//
//  ShaderProgram.h
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Matrix4.h"
#import "Object.h"

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
     */
    bool loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*> macros);
    //! Sets the a attribute that will be linked
    void addAttribute(unsigned int index, const char* attributeName);
    //! Sets the transform feedback varings that will be linked
    void setTransformFeedbackVaryings(unsigned int count, const char** varyings);
    //! Links all shaders to a complete shader programm
	void link();
    //! Sets this ShaderProgram as the currentShaderProgram and updates the transformation uniforms
	void use();
    //! Enables and sets a attribute
    void setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data);
    //! Checks if a uniform existis
    bool checkUniformExistence(const char* name);
    //! Sets a single integer uniform
    void setUniformI(const char* name, int value);
    //! Sets a single float uniform
    void setUniformF(const char* name, btScalar value);
    //! Sets a float[2] uniform
    void setUniformVec2(const char* name, btScalar x, btScalar y);
    //! Sets a float[3] uniform
    void setUniformVec3(const char* name, btVector3 value);
    //! Sets a 3x3 matrix uniform
    void setUniformMatrix3(const char* name, const btMatrix3x3* mat);
    //! Sets a 4x4 matrix uniform
    void setUniformMatrix4(const char* name, const Matrix4* mat);
    //! Sets a 4x4 matrix uniform
    void setUniformMatrix4(const char* name, const btTransform* mat);
    //! Sets a 4x4 matrix array uniform
    void setUniformMatrix4(const char* name, const btTransform* mat, unsigned int count);
};

extern btTransform modelMat;
extern ShaderProgram *shaderPrograms[44], *currentShaderProgram;
extern float screenBlurFactor, globalVolume, musicVolume;
extern bool edgeSmoothEnabled, fullScreenEnabled, cubemapsEnabled;
extern unsigned char depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, blendingQuality, particleCalcTarget;

enum ShaderProgramNames {
    //Static Shaders
    normalMapGenSP = 0,
    blurSP = 1,
    spotShadowCircleLightSP = 2,
    spriteSP = 3,
    colorSP = 4,
    //G-Buffer Shaders
    solidGSP = 5,
    skeletalGSP = 6,
    solidAnimatedGSP = 7,
    skeletalAnimatedGSP = 8,
    solidBumpGSP = 9,
    skeletalBumpGSP = 10,
    solidAnimatedBumpGSP = 11,
    skeletalAnimatedBumpGSP = 12,
    solidGlassGSP = 13,
    skeletalGlassGSP = 14,
    solidAnimatedGlassGSP = 15,
    skeletalAnimatedGlassGSP = 16,
    solidBumpGlassGSP = 17,
    skeletalBumpGlassGSP = 18,
    solidAnimatedBumpGlassGSP = 19,
    skeletalAnimatedBumpGlassGSP = 20,
    animatedWaterSP = 21,
    //Shadow Map Generators
    solidShadowSP = 22,
    skeletalShadowSP = 23,
    solidAnimatedShadowSP = 24,
    skeletalAnimatedShadowSP = 25,
    solidParabolidShadowSP = 26,
    skeletalParabolidShadowSP = 27,
    solidAnimatedParabolidShadowSP = 28,
    skeletalAnimatedParabolidShadowSP = 29,
    //Illumination Shaders
    directionalLightSP = 30,
    directionalShadowLightSP = 31,
    spotLightSP = 32,
    spotShadowLightSP = 33,
    positionalLightSP = 34,
    positionalShadowLightSP = 35,
    positionalShadowDualLightSP = 36,
    //Post Effect Shaders
    ssaoSP = 37,
    deferredCombineSP = 38,
    deferredCombineTransparentSP = 39,
    edgeSmoothSP = 40,
    depthOfFieldSP = 41,
    particleDrawSP = 42,
    particleCalculateSP = 43
};

//! Compiles all shader programs which are not influenced by graphic options
void loadStaticShaderPrograms();

//! (Re)compiles all shader programs which change their behavior with graphic options
void loadDynamicShaderPrograms();

#endif
