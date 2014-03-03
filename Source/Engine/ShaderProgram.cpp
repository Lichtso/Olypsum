//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "ObjectManager.h"
//#include "FileManager.h"

const char *seperatorString = "#separator\n", *includeString = "#include ";

ShaderProgram::ShaderProgram() {
    GLname = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    if(currentShaderProgram == this)
        currentShaderProgram = NULL;
    
	glDeleteProgram(GLname);
}

bool ShaderProgram::loadShader(GLuint shaderType, const char* sourceCode, std::vector<const char*>& macros) {
	unsigned int shaderId = glCreateShader(shaderType);
    
    std::string source;
    {
        std::ostringstream sourceStream;
        sourceStream << "#version 150\n";
        switch(shaderType) {
            case GL_VERTEX_SHADER:
                sourceStream << "#define GL_VERTEX_SHADER\n";
                break;
            case GL_FRAGMENT_SHADER:
                sourceStream << "#define GL_FRAGMENT_SHADER\n";
                break;
            case GL_GEOMETRY_SHADER:
                sourceStream << "#define GL_GEOMETRY_SHADER\n";
                break;
        }
        for(unsigned int m = 0; m < macros.size(); m ++)
            sourceStream << std::string("#define ")+macros[m]+"\n";
        sourceStream << sourceCode;
        source = sourceStream.str();
    }
    
    size_t pos = 0, prevPos;
    while(true) {
        pos = prevPos = source.find(includeString, pos);
        if(pos == std::string::npos) break;
        pos += strlen(includeString);
        unsigned int macroLength = source.find('\n', pos)-pos;
        std::unique_ptr<char[]> data = readFile(resourcesPath+"Shaders/"+source.substr(pos, macroLength), true);
        source.replace(prevPos, strlen(includeString)+macroLength, data.get());
    }
    
    const GLchar* sourceStr = source.c_str();
	glShaderSource(shaderId, 1, &sourceStr, NULL);
	glCompileShader(shaderId);
	int infoLogLength = 1024;
	char infoLog[infoLogLength];
	glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
        log(shader_log, std::string(infoLog)+sourceStr+"\n");
		return false;
	}
	glAttachShader(GLname, shaderId);
    glDeleteShader(shaderId);
    return true;
}

bool ShaderProgram::loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*> macros) {
    GLsizei shaderCount;
    glGetProgramiv(GLname, GL_ATTACHED_SHADERS, &shaderCount);
    GLuint shaders[shaderCount];
    glGetAttachedShaders(GLname, shaderCount, &shaderCount, shaders);
    for(size_t i = 0; i < shaderCount; i ++)
        glDetachShader(GLname, shaders[i]);
    
    std::unique_ptr<char[]> data = readFile(resourcesPath+"Shaders/"+fileName+".glsl", true);
    char* dataPos = data.get();
    for(GLenum shaderType: shaderTypes) {
        char* seperator = strstr(dataPos, seperatorString);
        if(seperator) {
            *seperator = 0;
            if(!loadShader(shaderType, dataPos, macros))
                return false;
            dataPos = seperator+strlen(seperatorString);
        }else
            if(!loadShader(shaderType, dataPos, macros))
                return false;
    }
    
    return true;
}

void ShaderProgram::addAttribute(unsigned int index, const char* name) {
    glBindAttribLocation(GLname, index, name);
}

void ShaderProgram::addFragDataLocations(std::vector<const char*> names) {
    for(unsigned i = 0; i < names.size(); i ++)
        glBindFragDataLocation(GLname, i, names[i]);
}

void ShaderProgram::setTransformFeedbackVaryings(unsigned int count, const char** varyings) {
    glTransformFeedbackVaryings(GLname, count, varyings, GL_INTERLEAVED_ATTRIBS);
}

bool ShaderProgram::link() {
    cachedUniforms.clear();
	glLinkProgram(GLname);
    
    int infoLogLength = 1024;
	char infoLog[infoLogLength];
    glGetProgramInfoLog(GLname, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
        log(shader_log, infoLog);
		return false;
	}
    
    glUseProgram(GLname);
    char buffer[64], samplerIndex;
    GLint location;
    for(samplerIndex = 0; samplerIndex < 8; samplerIndex ++) {
        sprintf(buffer, "sampler%d", samplerIndex);
        location = glGetUniformLocation(GLname, buffer);
        if(location < 0) break;
        glUniform1i(location, samplerIndex);
    }
    
    return true;
}

void ShaderProgram::use () {
	glUseProgram(GLname);
    currentShaderProgram = this;
    setUniformMatrix4("modelMat", &modelMat);
    if(checkUniformExistence("normalMat")) {
        btMatrix3x3 normalMat = Matrix4(modelMat).getNormalMatrix();
        setUniformMatrix3("normalMat", &normalMat);
    }
    if(currentCam) {
        Matrix4 transform = currentCam->getCamMatrix();
        setUniformF("depthNear", 1.0-currentCam->nearPlane);
        setUniformF("depthFar", 2.0/log2(currentCam->farPlane/currentCam->nearPlane));
        setUniformMatrix4("viewMat", &currentCam->viewMat);
        setUniformMatrix4("camMat", &transform);
        if(checkUniformExistence("modelViewMat")) {
            Matrix4 projectionMat = Matrix4(modelMat) * currentCam->viewMat;
            setUniformMatrix4("modelViewMat", &projectionMat);
        }
    }
    PlaneReflective* planeReflective = dynamic_cast<PlaneReflective*>(objectManager.currentReflective);
    if(planeReflective && checkUniformExistence("clipPlane[0]"))
        setUniformVec4("clipPlane[0]", planeReflective->plane);
}

bool ShaderProgram::linked() {
    GLint status;
    glGetProgramiv(GLname, GL_LINK_STATUS, &status);
    return status;
}

GLint ShaderProgram::getUniformLocation(const char* name) {
    auto iterator = cachedUniforms.find(name);
    if(iterator != cachedUniforms.end())
        return iterator->second;
    GLint value = glGetUniformLocation(GLname, name);
    cachedUniforms[name] = value;
    return value;
}

bool ShaderProgram::checkUniformExistence(const char* name) {
    return getUniformLocation(name) > -1;
}

void ShaderProgram::setUniformI(const char* name, int value) {
    glUniform1i(getUniformLocation(name), value);
}

void ShaderProgram::setUniformF(const char* name, btScalar value) {
    glUniform1f(getUniformLocation(name), value);
}

void ShaderProgram::setUniformVec2(const char* name, btScalar x, btScalar y) {
    glUniform2f(getUniformLocation(name), x, y);
}

void ShaderProgram::setUniformVec3(const char* name, const btVector3& value) {
    glUniform3f(getUniformLocation(name), value.x(), value.y(), value.z());
}

void ShaderProgram::setUniformVec4(const char* name, const btVector3& value) {
    glUniform4f(getUniformLocation(name), value.x(), value.y(), value.z(), value.w());
}

void ShaderProgram::setUniformMatrix3(const char* name, const btMatrix3x3* mat) {
    GLint location = getUniformLocation(name);
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

void ShaderProgram::setUniformMatrix3(const char* name, const btTransform* mat) {
    GLint location = getUniformLocation(name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getBasis().getOpenGLSubMatrix(matData);
    matData[3] = matData[4];
    matData[4] = matData[5];
    matData[5] = matData[6];
    matData[6] = mat->getOrigin().x();
    matData[7] = mat->getOrigin().y();
    matData[8] = 1.0;
    glUniformMatrix3fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, const Matrix4* mat) {
    GLint location = getUniformLocation(name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, const btTransform* mat) {
    GLint location = getUniformLocation(name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, const btTransform* mat, unsigned int count) {
    GLint location = getUniformLocation(name);
    if(location < 0) return;
    btScalar matData[count*16];
    for(unsigned int i = 0; i < count; i ++)
        mat[i].getOpenGLMatrix(matData+i*16);
    glUniformMatrix4fv(location, count, false, matData);
}

btTransform modelMat;
ShaderProgram *shaderPrograms[], *currentShaderProgram;

std::vector<GLenum> shaderTypeVertex = { GL_VERTEX_SHADER };
std::vector<GLenum> shaderTypeVertexFragment = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
std::vector<GLenum> shaderTypeVertexFragmentGeometry = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER };

std::vector<const char*> colorFragOut = { "colorOut" };
std::vector<const char*> gBufferOut = { "colorOut", "materialOut", "normalOut", "positionOut" };
std::vector<const char*> gBufferFiveOut = { "colorOut", "materialOut", "normalOut", "positionOut", "specularOut" };
std::vector<const char*> lightFragsOut = { "diffuseOut", "specularOut" };

void loadStaticShaderPrograms() {
    for(unsigned int p = 0; p < sizeof(shaderPrograms)/sizeof(void*); p ++)
        shaderPrograms[p] = new ShaderProgram();
    
    shaderPrograms[normalMapGenSP]->loadShaderProgram("normalMap", shaderTypeVertexFragment, { });
    shaderPrograms[normalMapGenSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[normalMapGenSP]->addFragDataLocations({ "normalOut" });
    shaderPrograms[normalMapGenSP]->link();
    
    shaderPrograms[blurSP]->loadShaderProgram("postScreenBlur", shaderTypeVertexFragment, { });
    shaderPrograms[blurSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[blurSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[blurSP]->link();
    
    shaderPrograms[circleMaskSP]->loadShaderProgram("circleMask", shaderTypeVertexFragment, { });
    shaderPrograms[circleMaskSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[circleMaskSP]->link();
    
    shaderPrograms[spriteSP]->loadShaderProgram("sprite", shaderTypeVertexFragment, { });
    shaderPrograms[spriteSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spriteSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[spriteSP]->link();
    
    shaderPrograms[monochromeSP]->loadShaderProgram("gBufferMonochrome", shaderTypeVertexFragment, { });
    shaderPrograms[monochromeSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[monochromeSP]->addFragDataLocations(gBufferOut);
    shaderPrograms[monochromeSP]->link();
    
    char blendingQualityMacro[32];
    for(unsigned int i = 0; i < 3; i ++) {
        sprintf(blendingQualityMacro, "BLENDING_QUALITY %d", i);
        shaderPrograms[deferredCombineSP0+i]->loadShaderProgram("deferredShader", shaderTypeVertexFragment, { blendingQualityMacro });
        shaderPrograms[deferredCombineSP0+i]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[deferredCombineSP0+i]->addFragDataLocations(colorFragOut);
        shaderPrograms[deferredCombineSP0+i]->link();
    }
    
    shaderPrograms[terrainGSP]->loadShaderProgram("gBufferTerrain", shaderTypeVertexFragment, { });
    shaderPrograms[terrainGSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[terrainGSP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[terrainGSP]->addFragDataLocations(gBufferFiveOut);
    shaderPrograms[terrainGSP]->link();
    
    //Shadow Map Generators
    for(unsigned int p = 0; p < 8; p ++) {
        std::vector<const char*> macros;
        if(p % 2 == 0)
            macros.push_back("SKELETAL_ANIMATION 0");
        else
            macros.push_back("SKELETAL_ANIMATION 1");
        
        if(p % 4 < 2)
            macros.push_back("TEXTURE_ANIMATION 0");
        else
            macros.push_back("TEXTURE_ANIMATION 1");
        
        if(p % 8 < 4)
            macros.push_back("PARABOLID 0");
        else
            macros.push_back("PARABOLID 1");
        
        shaderPrograms[solidShadowSP+p]->loadShaderProgram("shadow", shaderTypeVertexFragment, macros);
        shaderPrograms[solidShadowSP+p]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[solidShadowSP+p]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
        if(p % 2 == 1) {
            shaderPrograms[solidShadowSP+p]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
            shaderPrograms[solidShadowSP+p]->addAttribute(JOINT_ATTRIBUTE, "joints");
        }
        shaderPrograms[solidShadowSP+p]->link();
    }
    
    //Shadowless Illumination Shaders
    shaderPrograms[directionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment,
                                                          { "LIGHT_TYPE 1", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[directionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalLightSP]->addFragDataLocations(lightFragsOut);
    shaderPrograms[directionalLightSP]->link();
    
    shaderPrograms[spotLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment,
                                                   { "LIGHT_TYPE 2", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[spotLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotLightSP]->addFragDataLocations(lightFragsOut);
    shaderPrograms[spotLightSP]->link();
    
    shaderPrograms[positionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment,
                                                         { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[positionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[positionalLightSP]->addFragDataLocations(lightFragsOut);
    shaderPrograms[positionalLightSP]->link();
    
    //Post Effects
    shaderPrograms[edgeSmoothSP]->loadShaderProgram("postEdgeSmooth", shaderTypeVertexFragment, { });
    shaderPrograms[edgeSmoothSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[edgeSmoothSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[edgeSmoothSP]->link();
    
    //Particle Systems
    const char* varyings[] = { "vPosition", "vVelocity" };
    shaderPrograms[advanceParticlesSP]->loadShaderProgram("advanceParticles", shaderTypeVertex, { });
    shaderPrograms[advanceParticlesSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[advanceParticlesSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
    shaderPrograms[advanceParticlesSP]->setTransformFeedbackVaryings(2, varyings);
    shaderPrograms[advanceParticlesSP]->link();
    
    shaderPrograms[particlesSP]->loadShaderProgram("gBufferParticles", shaderTypeVertexFragmentGeometry, { "TEXTURE_ANIMATION 0" });
    shaderPrograms[particlesSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[particlesSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
    shaderPrograms[particlesSP]->addFragDataLocations(gBufferOut);
    shaderPrograms[particlesSP]->link();
    
    shaderPrograms[particlesAnimatedSP]->loadShaderProgram("gBufferParticles", shaderTypeVertexFragmentGeometry, { "TEXTURE_ANIMATION 1" });
    shaderPrograms[particlesAnimatedSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[particlesAnimatedSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
    shaderPrograms[particlesAnimatedSP]->addFragDataLocations(gBufferOut);
    shaderPrograms[particlesAnimatedSP]->link();
}

void updateGBufferShaderPrograms() {
    char bumpMappingMacro[32], blendingQualityMacro[32];
    sprintf(bumpMappingMacro, "BUMP_MAPPING %d", optionsState.surfaceQuality);
    sprintf(blendingQualityMacro, "BLENDING_QUALITY %d", optionsState.blendingQuality);
    
    for(unsigned int p = 0; p < 48; p ++) {
        std::vector<const char*> macros;
        
        if(p % 2 < 1)
            macros.push_back("SKELETAL_ANIMATION 0");
        else
            macros.push_back("SKELETAL_ANIMATION 1");
        
        if(p % 4 < 2)
            macros.push_back("TEXTURE_ANIMATION 0");
        else
            macros.push_back("TEXTURE_ANIMATION 1");
        
        if(p % 8 < 4)
            macros.push_back("BUMP_MAPPING 0");
        else
            macros.push_back(((p % 2 == 1 || p % 16 >= 8) && optionsState.surfaceQuality > 1) ? "BUMP_MAPPING 1" : bumpMappingMacro);
        
        if(p % 16 < 8)
            macros.push_back("BLENDING_QUALITY 0");
        else
            macros.push_back(blendingQualityMacro);
        
        switch(p / 16) {
            case 0:
            macros.push_back("REFLECTION_TYPE 0");
            break;
            case 1:
            macros.push_back("REFLECTION_TYPE 1");
            break;
            case 2:
            macros.push_back("REFLECTION_TYPE 2");
            break;
        }
        
        shaderPrograms[solidGSP+p]->loadShaderProgram("gBuffer",
                                                      (p % 8 < 4 || optionsState.surfaceQuality == 0) ? shaderTypeVertexFragment : shaderTypeVertexFragmentGeometry,
                                                      macros);
        shaderPrograms[solidGSP+p]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[solidGSP+p]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
        shaderPrograms[solidGSP+p]->addAttribute(NORMAL_ATTRIBUTE, "normal");
        if(p % 2 == 1) {
            shaderPrograms[solidGSP+p]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
            shaderPrograms[solidGSP+p]->addAttribute(JOINT_ATTRIBUTE, "joints");
        }
        shaderPrograms[solidGSP+p]->addFragDataLocations(gBufferFiveOut);
        shaderPrograms[solidGSP+p]->link();
    }
    
    shaderPrograms[waterGSP]->loadShaderProgram("gBuffer", shaderTypeVertexFragmentGeometry, { "SKELETAL_ANIMATION 0", blendingQualityMacro, "BUMP_MAPPING 4", "TEXTURE_ANIMATION 1", "REFLECTION_TYPE 1" });
    shaderPrograms[waterGSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[waterGSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[waterGSP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[waterGSP]->addFragDataLocations(gBufferFiveOut);
    shaderPrograms[waterGSP]->link();
}

void updateIlluminationShaderPrograms() {
    char shadowQualityMacro[32];
    sprintf(shadowQualityMacro, "SHADOW_QUALITY %d", optionsState.shadowQuality);
    
    shaderPrograms[directionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 1", "SHADOWS_ACTIVE 1", shadowQualityMacro });
    shaderPrograms[directionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalShadowLightSP]->addFragDataLocations(lightFragsOut);
    shaderPrograms[directionalShadowLightSP]->link();
    
    shaderPrograms[spotShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 2", "SHADOWS_ACTIVE 1", shadowQualityMacro });
    shaderPrograms[spotShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotShadowLightSP]->addFragDataLocations(lightFragsOut);
    shaderPrograms[spotShadowLightSP]->link();
    
    if(optionsState.cubemapsEnabled) {
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 3", shadowQualityMacro });
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->addFragDataLocations(lightFragsOut);
        shaderPrograms[positionalShadowLightSP]->link();
        shaderPrograms[positionalShadowLightSP]->use();
    }else{
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 1", shadowQualityMacro });
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->addFragDataLocations(lightFragsOut);
        shaderPrograms[positionalShadowLightSP]->link();
        
        shaderPrograms[positionalShadowDualLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 2", shadowQualityMacro });
        shaderPrograms[positionalShadowDualLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowDualLightSP]->addFragDataLocations(lightFragsOut);
        shaderPrograms[positionalShadowDualLightSP]->link();
    }
    
    for(auto lightObject : objectManager.lightObjects)
        lightObject->deleteShadowMap();
}

void updateSSAOShaderPrograms() {
    if(!optionsState.ssaoQuality) return;
    
    char scaleMacro[32], ssaoQualityMacro[32];
    sprintf(scaleMacro, "SSAO_SCALE %f", (float)optionsState.videoScale);
    sprintf(ssaoQualityMacro, "SSAO_QUALITY %d", optionsState.ssaoQuality);
    
    shaderPrograms[ssaoSP]->loadShaderProgram("postSSAO1", shaderTypeVertexFragment, { ssaoQualityMacro, scaleMacro });
    shaderPrograms[ssaoSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[ssaoSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[ssaoSP]->link();
    shaderPrograms[ssaoSP]->use();
    
    unsigned char samples = 32;
    float pSphere[samples*3];
    for(unsigned char i = 0; i < samples; i ++) {
        btVector3 vec(frand(-1.0, 1.0), frand(-1.0, 1.0), frand(0.0, 1.0));
        vec.normalize();
        vec *= frand(0.1, 1.0);
        pSphere[i*3  ] = vec.x();
        pSphere[i*3+1] = vec.y();
        pSphere[i*3+2] = vec.y();
    }
    glUniform3fv(glGetUniformLocation(shaderPrograms[ssaoSP]->GLname, "pSphere"), samples, pSphere);
    
    shaderPrograms[ssaoCombineSP]->loadShaderProgram("postSSAO2", shaderTypeVertexFragment, { ssaoQualityMacro, scaleMacro });
    shaderPrograms[ssaoCombineSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[ssaoCombineSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[ssaoCombineSP]->link();
}

void updateDOFShaderProgram() {
    char depthOfFieldMacro[32];
    sprintf(depthOfFieldMacro, "DOF_QUALITY %d", optionsState.depthOfFieldQuality);
    
    shaderPrograms[depthOfFieldFogSP]->loadShaderProgram("postDepthOfField", shaderTypeVertexFragment, { depthOfFieldMacro, "FOG_ACTIVE 1" });
    shaderPrograms[depthOfFieldFogSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[depthOfFieldFogSP]->addFragDataLocations(colorFragOut);
    shaderPrograms[depthOfFieldFogSP]->link();
    
    if(optionsState.depthOfFieldQuality) {
        shaderPrograms[depthOfFieldSP]->loadShaderProgram("postDepthOfField", shaderTypeVertexFragment, { depthOfFieldMacro, "FOG_ACTIVE 0"  });
        shaderPrograms[depthOfFieldSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[depthOfFieldSP]->addFragDataLocations(colorFragOut);
        shaderPrograms[depthOfFieldSP]->link();
    }
}