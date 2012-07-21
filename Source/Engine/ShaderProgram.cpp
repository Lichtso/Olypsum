//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "ShaderProgram.h"

const char *seperatorString = "#separator\n", *parameterString = "#parameter ";

ShaderProgram::ShaderProgram() {
    GLname = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    if(currentShaderProgram == this)
        currentShaderProgram = NULL;
    
	glDeleteProgram(GLname);
}

bool ShaderProgram::loadShader(GLuint shaderType, const char* soucreCode, std::vector<const char*>* macros) {
	unsigned int shaderId = glCreateShader(shaderType);
    
    if(shaderType == GL_GEOMETRY_SHADER_EXT) {
        char *parameter, *parameterEnd, *parameterKey, *parameterValue;
        while(true) {
            parameter = strstr(soucreCode, parameterString);
            if(!parameter) break;
            parameterKey = parameter+strlen(parameterString);
            parameterEnd = strchr(parameterKey, ' ');
            if(!parameterEnd) break;
            *parameterEnd = 0;
            parameterValue = parameterEnd+1;
            parameterEnd = strchr(parameterValue, '\n');
            if(!parameterEnd) break;
            *parameterEnd = 0;
            
            if(strcmp(parameterKey, "GL_GEOMETRY_VERTICES_OUT") == 0) {
                unsigned int count;
                sscanf(parameterValue, "%u", &count);
                glProgramParameteriEXT(GLname, GL_GEOMETRY_VERTICES_OUT_EXT, count);
            }else if(strcmp(parameterKey, "GL_GEOMETRY_INPUT_TYPE") == 0) {
                if(strcmp(parameterValue, "GL_TRIANGLES") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES);
                }else if(strcmp(parameterValue, "GL_TRIANGLES_ADJACENCY") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_INPUT_TYPE_EXT, GL_TRIANGLES_ADJACENCY_EXT);
                }else if(strcmp(parameterValue, "GL_LINES") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES);
                }else if(strcmp(parameterValue, "GL_LINES_ADJACENCY") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_INPUT_TYPE_EXT, GL_LINES_ADJACENCY_EXT);
                }else if(strcmp(parameterValue, "GL_POINTS") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_INPUT_TYPE_EXT, GL_POINTS);
                }else{
                    printf("SHADER:\n%s\nThe shader does contain a illegal parameter value for key GL_GEOMETRY_INPUT_TYPE: %s.\n", soucreCode, parameterValue);
                    return false;
                }
            }else if(strcmp(parameterKey, "GL_GEOMETRY_OUTPUT_TYPE") == 0) {
                if(strcmp(parameterValue, "GL_TRIANGLE_STRIP") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_TRIANGLE_STRIP);
                }else if(strcmp(parameterValue, "GL_LINE_STRIP") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_LINE_STRIP);
                }else if(strcmp(parameterValue, "GL_POINTS") == 0) {
                    glProgramParameteriEXT(GLname, GL_GEOMETRY_OUTPUT_TYPE_EXT, GL_POINTS);
                }else{
                    printf("SHADER:\n%s\nThe shader does contain a illegal parameter value for key GL_GEOMETRY_OUTPUT_TYPE: %s.\n", soucreCode, parameterValue);
                    return false;
                }
            }else{
                printf("SHADER:\n%s\nThe shader does contain a unknown parameter: %s.\n", soucreCode, parameterKey);
                return false;
            }
            
            while(parameter <= parameterEnd) {
                *parameter = ' ';
                parameter ++;
            }
        }
    }
    
    std::string soucreStr = "#version 120\n";
    for(unsigned int m = 0; m < macros->size(); m ++)
        soucreStr += std::string("#define ")+(*macros)[m]+"\n";
    
    soucreStr += soucreCode;
    const GLchar* soucre = soucreStr.c_str();
	glShaderSource(shaderId, 1, &soucre, NULL);
	glCompileShader(shaderId);
	int infoLogLength = 1024;
	char infoLog[infoLogLength];
	glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER:\n%s\n%s\n\n", soucre, infoLog);
		return false;
	}
	glAttachShader(GLname, shaderId);
    glDeleteShader(shaderId);
    return true;
}

bool ShaderProgram::loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*>* macros) {
    std::string url("Shaders/");
    url += fileName;
    url += ".glsl";
    
    FILE *fp = fopen(url.c_str(), "rb");
	if(!fp) {
		printf("SHADER:\nThe file %s couldn't be found.\n", url.c_str());
		return false;
	}
	fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize+1];
	fread(data, 1, dataSize, fp);
    fclose(fp);
    data[dataSize] = 0;
    
    char* dataPos = data;
    for(GLenum shaderType: shaderTypes) {
        char* seperator = strstr(dataPos, seperatorString);
        if(seperator) {
            *seperator = 0;
            loadShader(shaderType, dataPos, macros);
            dataPos = seperator+strlen(seperatorString);
        }else
            loadShader(shaderType, dataPos, macros);
    }
    
    return true;
}

void ShaderProgram::addAttribute(unsigned int index, const char* attributeName) {
    glBindAttribLocation(GLname, index, attributeName);
}

void ShaderProgram::setTransformFeedbackVaryings(unsigned int count, const char** varyings) {
    glTransformFeedbackVaryingsEXT(GLname, count, varyings, GL_INTERLEAVED_ATTRIBS_EXT);
}

void ShaderProgram::link() {
	glLinkProgram(GLname);
    
    int infoLogLength = 1024;
	char infoLog[infoLogLength];
    glGetProgramInfoLog(GLname, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
		printf("SHADER PROGRAM:\n%s", infoLog);
		return;
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
}

void ShaderProgram::use () {
	glUseProgram(GLname);
    currentShaderProgram = this;
    setUniformMatrix4("modelMat", &modelMat);
    if(checkUniformExistence("normalMat")) {
        Matrix4 normalMat = Matrix4(modelMat).normalize();
        setUniformMatrix3("normalMat", &normalMat);
    }
    if(currentCam) {
        setUniformVec3("camPos", currentCam->camMat.pos);
        setUniformMatrix4("viewMat", &currentCam->viewMat);
        if(checkUniformExistence("viewNormalMat")) {
            Matrix4 viewNormalMat = Matrix4(modelMat).normalize();
            setUniformMatrix3("viewNormalMat", &viewNormalMat);
        }
        if(checkUniformExistence("modelViewMat")) {
            Matrix4 projectionMat(modelMat * currentCam->viewMat);
            setUniformMatrix4("modelViewMat", &projectionMat);
        }
    }
}

void ShaderProgram::setAttribute(unsigned int index, unsigned int size, GLsizei stride, GLfloat* data) {
    glEnableVertexAttribArray(index);
    glVertexAttribPointer(index, size, GL_FLOAT, false, stride, data);
}

bool ShaderProgram::checkUniformExistence(const char* name) {
    return glGetUniformLocation(GLname, name) > -1;
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
ShaderProgram *shaderPrograms[], *currentShaderProgram;
bool edgeSmoothEnabled = false, fullScreenEnabled = false, cubeShadowsEnabled = false;
unsigned char depthOfFieldQuality = 0, bumpMappingQuality = 1, shadowQuality = 1, ssaoQuality = 0, particleQuality = 2;

void loadShaderPrograms() {
    for(unsigned int p = 0; p < sizeof(shaderPrograms)/sizeof(ShaderProgram*); p ++) {
        if(shaderPrograms[p]) delete shaderPrograms[p];
        shaderPrograms[p] = new ShaderProgram();
    }
    
    std::vector<GLenum> shaderTypeVertex;
    shaderTypeVertex.push_back(GL_VERTEX_SHADER);
    
    std::vector<GLenum> shaderTypeVertexFragment;
    shaderTypeVertexFragment.push_back(GL_VERTEX_SHADER);
    shaderTypeVertexFragment.push_back(GL_FRAGMENT_SHADER);
    
    std::vector<GLenum> shaderTypeVertexFragmentGeometry;
    shaderTypeVertexFragmentGeometry.push_back(GL_VERTEX_SHADER);
    shaderTypeVertexFragmentGeometry.push_back(GL_FRAGMENT_SHADER);
    shaderTypeVertexFragmentGeometry.push_back(GL_GEOMETRY_SHADER_EXT);
    
    std::vector<const char*> shaderProgramMacros;
    char depthOfFieldMacro[32], ssaoQualityMacro[32], bumpMappingMacro[32], shadowQualityMacro[32];
    sprintf(depthOfFieldMacro, "DOF_QUALITY %d", depthOfFieldQuality);
    sprintf(ssaoQualityMacro, "SSAO_QUALITY %d", ssaoQuality);
    sprintf(bumpMappingMacro, "BUMP_MAPPING %d", min(bumpMappingQuality, (unsigned char) 2));
    sprintf(shadowQualityMacro, "SHADOW_QUALITY %d", shadowQuality);
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 0");
    shaderProgramMacros.push_back("BUMP_MAPPING 0");
    
    shaderPrograms[solidGeometrySP]->loadShaderProgram("gBuffer", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[solidGeometrySP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[solidGeometrySP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[solidGeometrySP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[solidGeometrySP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 0");
    if(bumpMappingQuality == 3)
        shaderProgramMacros.push_back("BUMP_MAPPING 3");
    else
        shaderProgramMacros.push_back(bumpMappingMacro);
    shaderPrograms[solidBumpGeometrySP]->loadShaderProgram("gBuffer", (bumpMappingQuality) ? shaderTypeVertexFragmentGeometry : shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[solidBumpGeometrySP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[solidBumpGeometrySP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[solidBumpGeometrySP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[solidBumpGeometrySP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 0");
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 0");
    shaderPrograms[solidShadowSP]->loadShaderProgram("shadow", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[solidShadowSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[solidShadowSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[solidShadowSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 2");
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 0");
    shaderPrograms[solidParabolidShadowSP]->loadShaderProgram("shadow", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[solidParabolidShadowSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[solidParabolidShadowSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[solidParabolidShadowSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 1");
    shaderProgramMacros.push_back("BUMP_MAPPING 0");
    shaderPrograms[skeletalGeometrySP]->loadShaderProgram("gBuffer", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[skeletalGeometrySP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[skeletalGeometrySP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[skeletalGeometrySP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[skeletalGeometrySP]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    shaderPrograms[skeletalGeometrySP]->addAttribute(JOINT_ATTRIBUTE, "joints");
    shaderPrograms[skeletalGeometrySP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 1");
    shaderProgramMacros.push_back(bumpMappingMacro);
    shaderPrograms[skeletalBumpGeometrySP]->loadShaderProgram("gBuffer", (bumpMappingQuality) ? shaderTypeVertexFragmentGeometry : shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[skeletalBumpGeometrySP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[skeletalBumpGeometrySP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[skeletalBumpGeometrySP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[skeletalBumpGeometrySP]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    shaderPrograms[skeletalBumpGeometrySP]->addAttribute(JOINT_ATTRIBUTE, "joints");
    shaderPrograms[skeletalBumpGeometrySP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 0");
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 1");
    shaderPrograms[skeletalShadowSP]->loadShaderProgram("shadow", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[skeletalShadowSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[skeletalShadowSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[skeletalShadowSP]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    shaderPrograms[skeletalShadowSP]->addAttribute(JOINT_ATTRIBUTE, "joints");
    shaderPrograms[skeletalShadowSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 2");
    shaderProgramMacros.push_back("SKELETAL_ANIMATION 1");
    shaderPrograms[skeletalParabolidShadowSP]->loadShaderProgram("shadow", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[skeletalParabolidShadowSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[skeletalParabolidShadowSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[skeletalParabolidShadowSP]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
    shaderPrograms[skeletalParabolidShadowSP]->addAttribute(JOINT_ATTRIBUTE, "joints");
    shaderPrograms[skeletalParabolidShadowSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 1");
    shaderProgramMacros.push_back("SHADOWS_ACTIVE 0");
    shaderProgramMacros.push_back("SHADOW_QUALITY 0");
    shaderPrograms[directionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[directionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalLightSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 1");
    shaderProgramMacros.push_back("SHADOWS_ACTIVE 1");
    shaderProgramMacros.push_back(shadowQualityMacro);
    shaderPrograms[directionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[directionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalShadowLightSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 2");
    shaderProgramMacros.push_back("SHADOWS_ACTIVE 0");
    shaderProgramMacros.push_back("SHADOW_QUALITY 0");
    shaderPrograms[spotLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[spotLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotLightSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 2");
    shaderProgramMacros.push_back("SHADOWS_ACTIVE 1");
    shaderProgramMacros.push_back(shadowQualityMacro);
    shaderPrograms[spotShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[spotShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotShadowLightSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("PROCESSING_TYPE 3");
    shaderPrograms[spotShadowCircleLightSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[spotShadowCircleLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotShadowCircleLightSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("LIGHT_TYPE 3");
    shaderProgramMacros.push_back("SHADOWS_ACTIVE 0");
    shaderProgramMacros.push_back("SHADOW_QUALITY 0");
    shaderPrograms[positionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[positionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[positionalLightSP]->link();
    
    if(cubeShadowsEnabled) {
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back("LIGHT_TYPE 3");
        shaderProgramMacros.push_back("SHADOWS_ACTIVE 3");
        shaderProgramMacros.push_back(shadowQualityMacro);
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->link();
    }else{
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back("LIGHT_TYPE 3");
        shaderProgramMacros.push_back("SHADOWS_ACTIVE 1");
        shaderProgramMacros.push_back(shadowQualityMacro);
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->link();
        
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back("LIGHT_TYPE 3");
        shaderProgramMacros.push_back("SHADOWS_ACTIVE 2");
        shaderProgramMacros.push_back(shadowQualityMacro);
        shaderPrograms[positionalShadowDualLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[positionalShadowDualLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowDualLightSP]->link();
    }
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("PROCESSING_TYPE 1");
    shaderPrograms[normalMapGenSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[normalMapGenSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[normalMapGenSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("PROCESSING_TYPE 2");
    shaderPrograms[blurSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[blurSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[blurSP]->link();
    
    if(ssaoQuality) {
        unsigned char samples = 32;
        float pSphere[samples*2];
        for(unsigned char i = 0; i < samples*2; i ++)
            pSphere[i] = frand(-1.0, 1.0);
        for(unsigned char i = 0; i < samples; i ++) {
            Vector3 vec(pSphere[i*2], pSphere[i*2+1], 0.0);
            vec.normalize();
            vec *= frand(0.1, 1.0);
            pSphere[i*2  ] = vec.x;
            pSphere[i*2+1] = vec.y;
        }
        
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back(ssaoQualityMacro);
        shaderPrograms[ssaoSP]->loadShaderProgram("ssao", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[ssaoSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[ssaoSP]->link();
        shaderPrograms[ssaoSP]->use();
        glUniform2fv(glGetUniformLocation(shaderPrograms[ssaoSP]->GLname, "pSphere"), samples, pSphere);
    }
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back(ssaoQualityMacro);
    shaderPrograms[deferredCombineSP]->loadShaderProgram("deferredShader", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[deferredCombineSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[deferredCombineSP]->link();
    
    if(edgeSmoothEnabled) {
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back("PROCESSING_TYPE 1");
        shaderPrograms[edgeSmoothSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[edgeSmoothSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[edgeSmoothSP]->link();
    }
    
    if(depthOfFieldQuality) {
        shaderProgramMacros.clear();
        shaderProgramMacros.push_back("PROCESSING_TYPE 2");
        shaderProgramMacros.push_back(depthOfFieldMacro);
        shaderPrograms[depthOfFieldSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[depthOfFieldSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[depthOfFieldSP]->link();
    }
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("PROCESSING_TYPE 3");
    shaderPrograms[spriteSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[spriteSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spriteSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[spriteSP]->link();
    
    shaderProgramMacros.clear();
    shaderProgramMacros.push_back("PROCESSING_TYPE 4");
    shaderPrograms[colorSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, &shaderProgramMacros);
    shaderPrograms[colorSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[colorSP]->addAttribute(COLOR_ATTRIBUTE, "color");
    shaderPrograms[colorSP]->link();
    
    if(particleQuality > 0) {
        shaderProgramMacros.clear();
        shaderPrograms[particleDrawSP]->loadShaderProgram("particle", shaderTypeVertexFragment, &shaderProgramMacros);
        shaderPrograms[particleDrawSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[particleDrawSP]->link();
        
        if(particleQuality == 2) {
            const char* varyings[] = { "vPosition", "vVelocity" };
            shaderProgramMacros.clear();
            shaderPrograms[particleCalculateSP]->loadShaderProgram("particleCalc", shaderTypeVertex, &shaderProgramMacros);
            shaderPrograms[particleCalculateSP]->addAttribute(POSITION_ATTRIBUTE, "position");
            shaderPrograms[particleCalculateSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
            shaderPrograms[particleCalculateSP]->setTransformFeedbackVaryings(2, varyings);
            shaderPrograms[particleCalculateSP]->link();
        }
    }
}