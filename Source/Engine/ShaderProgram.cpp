//
//  ShaderProgram.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 20.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Cam.h"

Color4& Color4::operator=(const Color4& B) {
    r = B.r;
    g = B.g;
    b = B.b;
    a = B.a;
    return *this;
}

btVector3 Color4::getVector() {
    return btVector3(r, g, b);
}

SDL_Color Color4::getSDL() {
    SDL_Color B;
    B.r = r*255;
    B.g = g*255;
    B.b = b*255;
    B.unused = a*255;
    return B;
}

const char *seperatorString = "#separator\n", *parameterString = "#parameter ";

ShaderProgram::ShaderProgram() {
    GLname = glCreateProgram();
}

ShaderProgram::~ShaderProgram() {
    if(currentShaderProgram == this)
        currentShaderProgram = NULL;
    
	glDeleteProgram(GLname);
}

bool ShaderProgram::loadShader(GLuint shaderType, const char* soucreCode, std::vector<const char*>& macros) {
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
                    log(shader_log, std::string(soucreCode)+"\nThe shader does contain a illegal parameter value for key GL_GEOMETRY_INPUT_TYPE: "+parameterValue);
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
                    log(shader_log, std::string(soucreCode)+"\nThe shader does contain a illegal parameter value for key GL_GEOMETRY_OUTPUT_TYPE: "+parameterValue);
                    return false;
                }
            }else{
                log(shader_log, std::string(soucreCode)+"\nThe shader does contain a unknown parameter: "+parameterKey);
                return false;
            }
            
            while(parameter <= parameterEnd) {
                *parameter = ' ';
                parameter ++;
            }
        }
    }
    
    std::string soucreStr = "#version 120\n";
    for(unsigned int m = 0; m < macros.size(); m ++)
        soucreStr += std::string("#define ")+macros[m]+"\n";
    
    soucreStr += soucreCode;
    const GLchar* soucre = soucreStr.c_str();
	glShaderSource(shaderId, 1, &soucre, NULL);
	glCompileShader(shaderId);
	int infoLogLength = 1024;
	char infoLog[infoLogLength];
	glGetShaderInfoLog(shaderId, infoLogLength, &infoLogLength, (GLchar*) &infoLog);
	if(infoLogLength > 0) {
        char* logStr = infoLog;
        int col, row;
        while(strncmp(logStr, "ERROR: ", 7) == 0) {
            char* seperatorA = strchr(logStr+7, ':');
            *(seperatorA ++) = 0;
            sscanf(logStr+7, "%d", &col);
            char* seperatorB = strchr(seperatorA, ':');
            *(seperatorB ++) = 0;
            sscanf(seperatorA, "%d", &row);
            char* seperatorC = strchr(seperatorB, '\n');
            *(seperatorC ++) = 0;
            
            const char *pos, *begin = NULL, *end = NULL, *middle = NULL;
            unsigned int line = 0;
            for(pos = soucreStr.c_str(); *pos != 0; pos ++) {
                if(*pos != '\n') continue;
                line ++;
                if(!begin && line >= row-3) {
                    begin = pos;
                }else if(line == row-1) {
                    middle = pos+1;
                }else if(!end && line >= row+3) {
                    end = pos;
                    break;
                }
            }
            if(!middle) middle = begin;
            if(!end) end = pos;
            char linesBuffer[end-begin+1];
            memcpy(linesBuffer, begin, end-begin);
            linesBuffer[middle-begin] = '>';
            linesBuffer[end-begin] = 0;
            
            log(shader_log, std::string("ERROR: ")+seperatorB+" (Line "+stringOf(row)+")\n"+linesBuffer+"\n");
            logStr = seperatorC;
        }
        
        //log(shader_log, infoLog);
		return false;
	}
	glAttachShader(GLname, shaderId);
    glDeleteShader(shaderId);
    return true;
}

bool ShaderProgram::loadShaderProgram(const char* fileName, std::vector<GLenum> shaderTypes, std::vector<const char*> macros) {
    std::string url("Shaders/");
    url += fileName;
    url += ".glsl";
    
    FILE *fp = fopen(url.c_str(), "rb");
	if(!fp) {
        log(shader_log, std::string("The file ")+"\n"+url.c_str()+" couldn't be found.");
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
        log(shader_log, infoLog);
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
        btMatrix3x3 normalMat = Matrix4(modelMat).getNormalMatrix();
        setUniformMatrix3("normalMat", &normalMat);
    }
    if(currentCam) {
        setUniformVec3("camPos", currentCam->getTransformation().getOrigin());
        setUniformMatrix4("viewMat", &currentCam->viewMat);
        if(checkUniformExistence("viewNormalMat")) {
            btMatrix3x3 viewNormalMat = currentCam->getTransformation().getBasis();
            setUniformMatrix3("viewNormalMat", &viewNormalMat);
        }
        if(checkUniformExistence("modelViewMat")) {
            Matrix4 projectionMat = Matrix4(modelMat) * currentCam->viewMat;
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

void ShaderProgram::setUniformF(const char* name, btScalar value) {
    glUniform1f(glGetUniformLocation(GLname, name), value);
}

void ShaderProgram::setUniformVec2(const char* name, btScalar x, btScalar y) {
    glUniform2f(glGetUniformLocation(GLname, name), x, y);
}

void ShaderProgram::setUniformVec3(const char* name, btVector3 value) {
    glUniform3f(glGetUniformLocation(GLname, name), value.x(), value.y(), value.z());
}

void ShaderProgram::setUniformMatrix3(const char* name, const btMatrix3x3* mat) {
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

void ShaderProgram::setUniformMatrix4(const char* name, const Matrix4* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, false, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, const btTransform* mat) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[16];
    mat->getOpenGLMatrix(matData);
    glUniformMatrix4fv(location, 1, true, matData);
}

void ShaderProgram::setUniformMatrix4(const char* name, const btTransform* mat, unsigned int count) {
    GLint location = glGetUniformLocation(GLname, name);
    if(location < 0) return;
    btScalar matData[count*16];
    for(unsigned int i = 0; i < count; i ++)
        mat[i].getOpenGLMatrix(matData+i*16);
    glUniformMatrix4fv(location, count, true, matData);
}

btTransform modelMat;
ShaderProgram *shaderPrograms[], *currentShaderProgram;
float screenBlurFactor = -1.0, globalVolume = 0.5, musicVolume = 0.5;
bool edgeSmoothEnabled = false, fullScreenEnabled = false, cubemapsEnabled = false;
unsigned char depthOfFieldQuality = 0, bumpMappingQuality = 1, shadowQuality = 1, ssaoQuality = 0, blendingQuality = 2, particleCalcTarget = 2;

void loadStaticShaderPrograms() {
    std::vector<GLenum> shaderTypeVertexFragment = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    
    for(unsigned int p = 0; p < sizeof(shaderPrograms)/sizeof(ShaderProgram*); p ++)
        shaderPrograms[p] = (p <= colorSP) ? new ShaderProgram() : NULL;
    
    shaderPrograms[normalMapGenSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 1" });
    shaderPrograms[normalMapGenSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[normalMapGenSP]->link();
    
    shaderPrograms[blurSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 2" });
    shaderPrograms[blurSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[blurSP]->link();
    
    shaderPrograms[spotShadowCircleLightSP]->loadShaderProgram("preProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 3" });
    shaderPrograms[spotShadowCircleLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotShadowCircleLightSP]->link();
    
    shaderPrograms[spriteSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 3" });
    shaderPrograms[spriteSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spriteSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[spriteSP]->link();
    
    shaderPrograms[colorSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 4" });
    shaderPrograms[colorSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[colorSP]->addAttribute(COLOR_ATTRIBUTE, "color");
    shaderPrograms[colorSP]->link();
}

void loadDynamicShaderPrograms() {
    for(unsigned int p = solidGSP; p < sizeof(shaderPrograms)/sizeof(ShaderProgram*); p ++) {
        if(shaderPrograms[p]) delete shaderPrograms[p];
        shaderPrograms[p] = new ShaderProgram();
    }
    
    std::vector<GLenum> shaderTypeVertex = { GL_VERTEX_SHADER };
    std::vector<GLenum> shaderTypeVertexFragment = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER };
    std::vector<GLenum> shaderTypeVertexFragmentGeometry = { GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, GL_GEOMETRY_SHADER_EXT };
    
    std::vector<const char*> shaderProgramMacros;
    char depthOfFieldMacro[32], ssaoQualityMacro[32], blendingQualityMacro[32], bumpMappingMacro[32], shadowQualityMacro[32];
    sprintf(depthOfFieldMacro, "DOF_QUALITY %d", depthOfFieldQuality);
    sprintf(ssaoQualityMacro, "SSAO_QUALITY %d", ssaoQuality);
    sprintf(blendingQualityMacro, "BLENDING_QUALITY %d", blendingQuality);
    sprintf(bumpMappingMacro, "BUMP_MAPPING %d", bumpMappingQuality);
    sprintf(shadowQualityMacro, "SHADOW_QUALITY %d", shadowQuality);
    
    //G-Buffer Shaders
    
    for(unsigned int p = 0; p < 16; p ++) {
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
            macros.push_back("BUMP_MAPPING 0");
        else
            macros.push_back(((p % 2 == 1 || p % 16 >= 8) && bumpMappingQuality > 1) ? "BUMP_MAPPING 1" : bumpMappingMacro);
        
        if(p % 16 < 8)
            macros.push_back("BLENDING_QUALITY 0");
        else
            macros.push_back(blendingQualityMacro);
        
        shaderPrograms[solidGSP+p]->loadShaderProgram("gBuffer",
                                                      (p % 8 < 4) ? shaderTypeVertexFragment : shaderTypeVertexFragmentGeometry,
                                                      macros);
        shaderPrograms[solidGSP+p]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[solidGSP+p]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
        shaderPrograms[solidGSP+p]->addAttribute(NORMAL_ATTRIBUTE, "normal");
        if(p % 2 == 1) {
            shaderPrograms[solidGSP+p]->addAttribute(WEIGHT_ATTRIBUTE, "weights");
            shaderPrograms[solidGSP+p]->addAttribute(JOINT_ATTRIBUTE, "joints");
        }
        shaderPrograms[solidGSP+p]->link();
    }
    
    shaderPrograms[waterAnimatedGSP]->loadShaderProgram("gBuffer", shaderTypeVertexFragmentGeometry, { "SKELETAL_ANIMATION 0", blendingQualityMacro, "BUMP_MAPPING 2", "TEXTURE_ANIMATION 1" });
    shaderPrograms[waterAnimatedGSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[waterAnimatedGSP]->addAttribute(TEXTURE_COORD_ATTRIBUTE, "texCoord");
    shaderPrograms[waterAnimatedGSP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
    shaderPrograms[waterAnimatedGSP]->link();
    
    shaderPrograms[terrainGSP]->loadShaderProgram("gBufferTerrain", shaderTypeVertexFragment, { });
    shaderPrograms[terrainGSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[terrainGSP]->addAttribute(NORMAL_ATTRIBUTE, "normal");
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
    
    //Illumination Shaders
    
    shaderPrograms[directionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 1", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[directionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalLightSP]->link();
    
    shaderPrograms[directionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 1", "SHADOWS_ACTIVE 1", shadowQualityMacro });
    shaderPrograms[directionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[directionalShadowLightSP]->link();
    
    shaderPrograms[spotLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 2", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[spotLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotLightSP]->link();
    
    shaderPrograms[spotShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 2", "SHADOWS_ACTIVE 1", shadowQualityMacro });
    shaderPrograms[spotShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[spotShadowLightSP]->link();
    
    shaderPrograms[positionalLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 0", "SHADOW_QUALITY 0" });
    shaderPrograms[positionalLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[positionalLightSP]->link();
    
    if(cubemapsEnabled) {
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 3", shadowQualityMacro });
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->link();
    }else{
        shaderPrograms[positionalShadowLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 1", shadowQualityMacro });
        shaderPrograms[positionalShadowLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowLightSP]->link();
        
        shaderPrograms[positionalShadowDualLightSP]->loadShaderProgram("deferredLight", shaderTypeVertexFragment, { "LIGHT_TYPE 3", "SHADOWS_ACTIVE 2", shadowQualityMacro });
        shaderPrograms[positionalShadowDualLightSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[positionalShadowDualLightSP]->link();
    }
    
    //Post Effect Shaders
    
    if(ssaoQuality) {
        unsigned char samples = 32;
        float pSphere[samples*2];
        for(unsigned char i = 0; i < samples*2; i ++)
            pSphere[i] = frand(-1.0, 1.0);
        for(unsigned char i = 0; i < samples; i ++) {
            btVector3 vec(pSphere[i*2], pSphere[i*2+1], 0.0);
            vec.normalize();
            vec *= frand(0.1, 1.0);
            pSphere[i*2  ] = vec.x();
            pSphere[i*2+1] = vec.y();
        }
        
        shaderPrograms[ssaoSP]->loadShaderProgram("ssao", shaderTypeVertexFragment, { ssaoQualityMacro });
        shaderPrograms[ssaoSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[ssaoSP]->link();
        shaderPrograms[ssaoSP]->use();
        glUniform2fv(glGetUniformLocation(shaderPrograms[ssaoSP]->GLname, "pSphere"), samples, pSphere);
    }
    
    shaderPrograms[deferredCombineSP]->loadShaderProgram("deferredShader", shaderTypeVertexFragment, { "BLENDING_QUALITY 0", ssaoQualityMacro });
    shaderPrograms[deferredCombineSP]->addAttribute(POSITION_ATTRIBUTE, "position");
    shaderPrograms[deferredCombineSP]->link();
    
    if(blendingQuality > 0) {
        shaderPrograms[deferredCombineTransparentSP]->loadShaderProgram("deferredShader", shaderTypeVertexFragment, { blendingQualityMacro, "SSAO_QUALITY 0" });
        shaderPrograms[deferredCombineTransparentSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[deferredCombineTransparentSP]->link();
    }
    
    if(edgeSmoothEnabled) {
        shaderPrograms[edgeSmoothSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 1" });
        shaderPrograms[edgeSmoothSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[edgeSmoothSP]->link();
    }
    
    if(depthOfFieldQuality) {
        shaderPrograms[depthOfFieldSP]->loadShaderProgram("postProcessing", shaderTypeVertexFragment, { "PROCESSING_TYPE 2", depthOfFieldMacro });
        shaderPrograms[depthOfFieldSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[depthOfFieldSP]->link();
    }
    
    if(particleCalcTarget > 0) {
        if(particleCalcTarget == 2) {
            const char* varyings[] = { "vPosition", "vVelocity" };
            shaderPrograms[particleCalculateSP]->loadShaderProgram("particleCalc", shaderTypeVertex, { });
            shaderPrograms[particleCalculateSP]->addAttribute(POSITION_ATTRIBUTE, "position");
            shaderPrograms[particleCalculateSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
            shaderPrograms[particleCalculateSP]->setTransformFeedbackVaryings(2, varyings);
            shaderPrograms[particleCalculateSP]->link();
        }
        
        shaderPrograms[particleDrawSP]->loadShaderProgram("particle", shaderTypeVertexFragmentGeometry, { "TEXTURE_ANIMATION 0" });
        shaderPrograms[particleDrawSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[particleDrawSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
        shaderPrograms[particleDrawSP]->link();
        
        shaderPrograms[particleDrawAnimatedSP]->loadShaderProgram("particle", shaderTypeVertexFragmentGeometry, { "TEXTURE_ANIMATION 1" });
        shaderPrograms[particleDrawAnimatedSP]->addAttribute(POSITION_ATTRIBUTE, "position");
        shaderPrograms[particleDrawAnimatedSP]->addAttribute(VELOCITY_ATTRIBUTE, "velocity");
        shaderPrograms[particleDrawAnimatedSP]->link();
    }
    
    mainFBO.init();
}