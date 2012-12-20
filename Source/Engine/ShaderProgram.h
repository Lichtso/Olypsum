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
	bool loadShader(GLuint shaderType, const char* soucreCode, std::vector<const char*>* macros);
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
extern ShaderProgram *shaderPrograms[32], *currentShaderProgram;
extern float screenBlurFactor;
extern bool edgeSmoothEnabled, fullScreenEnabled, cubemapsEnabled;
extern unsigned char depthOfFieldQuality, bumpMappingQuality, shadowQuality, ssaoQuality, blendingQuality, particleCalcTarget;

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
    deferredCombineTransparentSP = 22,
    edgeSmoothSP = 23,
    depthOfFieldSP = 24,
    particleDrawSP = 25,
    particleCalculateSP = 26,
    glassGeometrySP = 27,
    glassBumpGeometrySP = 28,
    glassSkeletalGeometrySP = 29,
    glassSkeletalBumpGeometrySP = 30,
    waterSP = 31
};

//! Compiles all shader programs which are not influenced by graphic options
void loadStaticShaderPrograms();

//! (Re)compiles all shader programs which change their behavior with graphic options
void loadDynamicShaderPrograms();

#endif
