//
//  Model.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FileManager.h"
#import "rapidxml_print.hpp"

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define jointsPerVertex 3
#define maxJointsCount 64
//#define reExportModelFiles

Mesh::Mesh() {
    elementsCount = 0;
    vbo = ibo = 0;
    postions = texcoords = normals = -1;
    tangents = bitangents = weightJoints = -1;
    diffuse = normalMap = effectMap = NULL;
}

Mesh::~Mesh() {
    if(vbo) glDeleteBuffers(1, &vbo);
    if(ibo) glDeleteBuffers(1, &ibo);
    if(diffuse) fileManager.releaseTexture(diffuse);
    if(normalMap) fileManager.releaseTexture(normalMap);
    if(effectMap) fileManager.releaseTexture(effectMap);
}

void Mesh::draw() {
    if(!vbo || elementsCount == 0) {
        printf("ERROR: No vertex data to draw in mesh.\n");
        return;
    }
    if(postions == -1) {
        printf("ERROR: No postions data to draw in mesh.\n");
        return;
    }
    
    if(diffuse) diffuse->use(0);
    if(normalMap) normalMap->use(1);
    if(effectMap) effectMap->use(2);
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    unsigned int byteStride = 3;
    if(texcoords >= 0) byteStride += 2;
    if(normals >= 0) byteStride += 3;
    if(tangents >= 0) byteStride += 3;
    if(bitangents >= 0) byteStride += 3;
    if(weightJoints >= 0) byteStride += jointsPerVertex*2;
    byteStride *= sizeof(float);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(postions*sizeof(float)));
    if(texcoords >= 0) currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(texcoords*sizeof(float)));
    if(normals >= 0) currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(normals*sizeof(float)));
    if(tangents >= 0) currentShaderProgram->setAttribute(TANGENT_ATTRIBUTE, 3, byteStride, (float*)(tangents*sizeof(float)));
    if(bitangents >= 0) currentShaderProgram->setAttribute(BITANGENT_ATTRIBUTE, 3, byteStride, (float*)(bitangents*sizeof(float)));
    if(weightJoints >= 0) {
        currentShaderProgram->setAttribute(WEIGHT_ATTRIBUTE, jointsPerVertex, byteStride, (float*)(weightJoints*sizeof(float)));
        currentShaderProgram->setAttribute(JOINT_ATTRIBUTE, jointsPerVertex, byteStride, (float*)((weightJoints+jointsPerVertex)*sizeof(float)));
    }
    
    if(ibo) {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
        glDrawElements(GL_TRIANGLES, elementsCount, GL_UNSIGNED_INT, NULL);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    }else{
        glDrawArrays(GL_TRIANGLES, 0, elementsCount);
    }
}

struct FloatArray {
    float* data;
    unsigned int count, stride;
};

struct IntArray {
    int* data;
    unsigned int count;
};

struct VertexReference {
    FloatArray* source;
    unsigned int offset;
    char* name;
};

struct Material {
    std::string diffuseURL, normalMapURL, effectMapURL;
};

static void readFloatStr(char* dataStr, FloatArray& array) {
    array.data = new float[array.count];
    
    char *lastPos = dataStr;
    for(unsigned int i = 0; i < array.count; i ++) {
        dataStr = strchr(lastPos, ' ');
        if(dataStr) *dataStr = 0;
        sscanf(lastPos, "%f", &array.data[i]);
        lastPos = dataStr+1;
    }
    
    return;
}

static void readIntStr(char* dataStr, IntArray& array) {
    if(!array.data) array.data = new int[array.count];
    
    char *lastPos = dataStr;
    for(unsigned int i = 0; i < array.count; i ++) {
        dataStr = strchr(lastPos, ' ');
        if(dataStr) *dataStr = 0;
        sscanf(lastPos, "%d", &array.data[i]);
        lastPos = dataStr+1;
    }
    
    return;
}

static Matrix4 readTransform(rapidxml::xml_node<xmlUsedCharType>* dataNode) {
    Matrix4 result, mat;
    result.setIdentity();
    rapidxml::xml_node<xmlUsedCharType>* transformNode = dataNode->first_node();
    while(transformNode) {
        if(strcmp(transformNode->name(), "matrix") == 0) {
            FloatArray matrixData;
            matrixData.count = 16;
            readFloatStr(transformNode->value(), matrixData);
            result = Matrix4(matrixData.data).getTransposed();
            delete [] matrixData.data;
        }else if(strcmp(transformNode->name(), "translate") == 0) {
            FloatArray vectorData;
            vectorData.count = 3;
            readFloatStr(transformNode->value(), vectorData);
            mat.setIdentity();
            mat.translate(Vector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]));
            result = mat * result;
            delete [] vectorData.data;
        }else if(strcmp(transformNode->name(), "rotate") == 0) {
            FloatArray vectorData;
            vectorData.count = 4;
            readFloatStr(transformNode->value(), vectorData);
            mat.setIdentity();
            mat.rotateV(Vector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]), vectorData.data[3]/180.0*M_PI);
            result = mat * result;
            delete [] vectorData.data;
        }else if(strcmp(transformNode->name(), "scale") == 0) {
            FloatArray vectorData;
            vectorData.count = 3;
            readFloatStr(transformNode->value(), vectorData);
            mat.setIdentity();
            mat.scale(Vector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]));
            result = mat * result;
            delete [] vectorData.data;
        }
        transformNode = transformNode->next_sibling();
    }
    return result;
}

static Bone* readBone(rapidxml::xml_document<xmlUsedCharType>* exportDoc, Skeleton& skeleton, Matrix4& deTransform, Matrix4& parentMat, rapidxml::xml_node<xmlUsedCharType>* dataNode, rapidxml::xml_node<xmlUsedCharType>* reMeshNode) {
    rapidxml::xml_attribute<xmlUsedCharType> *typeAttribute, *sidAttribute = dataNode->first_attribute("sid");
    Bone *bone = new Bone;
    bone->name = sidAttribute->value();
    bone->staticMat = readTransform(dataNode);
    #ifdef reExportModelFiles
    char matrixData[256], *matrixDataPos = matrixData;
    float matFloatData[16];
    Matrix4 reTransform = Matrix4(bone->staticMat) * deTransform;
    reTransform.getOpenGLMatrix4(matFloatData);
    for(unsigned int i = 0; i < 16; i ++) {
        if(i > 0) *(matrixDataPos ++) = ' ';
        matrixDataPos += sprintf(matrixDataPos, "%1.8f", matFloatData[i]);
    }
    rapidxml::xml_node<xmlUsedCharType> *reMatrixNode = exportDoc->allocate_node(rapidxml::node_element, "matrix", exportDoc->allocate_string(matrixData)),
                                        *reDataNode = exportDoc->allocate_node(rapidxml::node_element, "node");
    reDataNode->append_attribute(exportDoc->allocate_attribute("type", "JOINT"));
    reDataNode->append_attribute(exportDoc->allocate_attribute("sid", sidAttribute->value()));
    reDataNode->append_node(reMatrixNode);
    reMeshNode->append_node(reDataNode);
    #endif
    bone->staticMat *= parentMat;
    rapidxml::xml_node<xmlUsedCharType> *childNode = dataNode->first_node("node");
    while(childNode) {
        typeAttribute = childNode->first_attribute("type");
        sidAttribute = childNode->first_attribute("sid");
        if(!typeAttribute || !sidAttribute || strcmp(typeAttribute->value(), "JOINT") != 0) continue;
        #ifdef reExportModelFiles
        bone->children.push_back(readBone(exportDoc, skeleton, deTransform, bone->staticMat, childNode, reDataNode));
        #else
        bone->children.push_back(readBone(exportDoc, skeleton, deTransform, bone->staticMat, childNode, NULL));
        #endif
        childNode = childNode->next_sibling("node");
    }
    skeleton.bones[bone->name] = bone;
    return bone;
}

Model::Model() {
    useCounter = 1;
    skeleton = NULL;
}

Model::~Model() {
    for(unsigned int i = 0; i < meshes.size(); i ++)
        delete meshes[i];
    
    if(skeleton) {
        std::map<std::string, Bone*>::iterator iterator;
        for(iterator = skeleton->bones.begin(); iterator != skeleton->bones.end(); iterator ++)
            delete iterator->second;
        delete skeleton;
    }
}

bool Model::loadCollada(FilePackage* filePackage, const char* filePath) {
    if(meshes.size() > 0) return false;
    
    rapidxml::xml_document<xmlUsedCharType> doc, exportDoc;
    unsigned int fileSize;
    char* fileData = parseXmlFile(doc, filePath, fileSize);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *collada, *library, *geometry, *meshNode, *source, *dataNode;
    rapidxml::xml_attribute<xmlUsedCharType> *dataAttribute;
    std::map<std::string, std::string> textureURLs;
    std::map<std::string, Material> materials;
    FloatArray skinData;
    skinData.data = NULL;
    char upAxis = AXIS_Y, *id;
    Matrix4 modelTransformMat;
    Mesh* mesh;
    
    collada = doc.first_node("COLLADA");
    if(!collada) goto endParsingXML;
    #ifdef reExportModelFiles
    rapidxml::xml_node<xmlUsedCharType> *reCollada, *reLibrary, *reGeometry, *reMeshNode, *reSource, *reDataNode;
    exportDoc.append_node(reCollada = doc.allocate_node(rapidxml::node_element, "COLLADA"));
    #endif
    
    //Load Assert
    dataNode = collada->first_node("asset");
    if(dataNode) {
        dataNode = dataNode->first_node("up_axis");
        if(!dataNode) goto endParsingXML;
        if(strcmp(dataNode->value(), "X_UP") == 0) {
            upAxis = AXIS_X;
        }else if(strcmp(dataNode->value(), "Y_UP") == 0) {
            upAxis = AXIS_Y;
        }else if(strcmp(dataNode->value(), "Z_UP") == 0) {
            upAxis = AXIS_Z;
        }
    }
    #ifdef reExportModelFiles
    reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "asset"));
    reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "contributor"));
    reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "authoring_tool", ENGINE_NAME));
    reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "up_axis", "Y_UP"));
    #endif
    modelTransformMat.setIdentity();
    if(upAxis == AXIS_X)
        modelTransformMat.rotateZ(M_PI_2);
    else if(upAxis == AXIS_Z)
        modelTransformMat.rotateX(M_PI_2);
    
    //Load TextureURLs
    library = collada->first_node("library_images");
    if(library) {
        #ifdef reExportModelFiles
        reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_images"));
        #endif
        geometry = library->first_node("image");
        while(geometry) {
            dataAttribute = geometry->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            meshNode = geometry->first_node("init_from");
            if(!meshNode) goto endParsingXML;
            textureURLs[dataAttribute->value()] = meshNode->value();
            geometry = geometry->next_sibling("image");
            #ifdef reExportModelFiles
            reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "image"));
            reGeometry->append_attribute(doc.allocate_attribute("id", dataAttribute->value()));
            reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "init_from", meshNode->value()));
            #endif
        }
    }
    
    //Load Materials
    library = collada->first_node("library_effects");
    if(library) {
        #ifdef reExportModelFiles
        reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_effects"));
        #endif
        geometry = library->first_node("effect");
        while(geometry) {
            dataAttribute = geometry->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            Material material;
            meshNode = geometry->first_node("profile_COMMON");
            if(!meshNode) goto endParsingXML;
            #ifdef reExportModelFiles
            reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "effect"));
            reGeometry->append_attribute(doc.allocate_attribute("id", id));
            reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "profile_COMMON"));
            #endif
            std::map<std::string, std::string> surfaceURLs, samplerURLs;
            
            //Find Surfaces
            source = meshNode->first_node("newparam");
            while(source) {
                #ifdef reExportModelFiles
                reSource = doc.allocate_node(rapidxml::node_element, "newparam");
                #endif
                dataNode = source->first_node("surface");
                if(dataNode) {
                    dataAttribute = dataNode->first_attribute("type");
                    if(!dataAttribute || strcmp(dataAttribute->value(), "2D") != 0) break;
                    dataNode = dataNode->first_node("init_from");
                    dataAttribute = source->first_attribute("sid");
                    if(!dataNode || !dataAttribute) break;
                    std::map<std::string, std::string>::iterator iterator = textureURLs.find(dataNode->value());
                    if(iterator == textureURLs.end()) {
                        printf("ERROR: No texture by id %s found.\n", dataNode->value());
                        goto endParsingXML;
                    }
                    surfaceURLs[dataAttribute->value()] = iterator->second;
                    #ifdef reExportModelFiles
                    reSource->append_attribute(doc.allocate_attribute("sid", dataAttribute->value()));
                    reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "surface"));
                    reDataNode->append_node(doc.allocate_node(rapidxml::node_element, "init_from", dataNode->value()));
                    reDataNode->append_attribute(doc.allocate_attribute("type", "2D"));
                    reMeshNode->append_node(reSource);
                    #endif
                }
                source = source->next_sibling("newparam");
            }
            
            //Find Samplers
            source = meshNode->first_node("newparam");
            while(source) {
                #ifdef reExportModelFiles
                reSource = doc.allocate_node(rapidxml::node_element, "newparam");
                #endif
                dataNode = source->first_node("sampler2D");
                if(dataNode) {
                    dataNode = dataNode->first_node("source");
                    dataAttribute = source->first_attribute("sid");
                    if(!dataNode || !dataAttribute) break;
                    std::map<std::string, std::string>::iterator iterator = surfaceURLs.find(dataNode->value());
                    if(iterator == surfaceURLs.end()) {
                        printf("ERROR: No surface by id %s found.\n", dataNode->value());
                        goto endParsingXML;
                    }
                    samplerURLs[dataAttribute->value()] = iterator->second;
                    #ifdef reExportModelFiles
                    reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "sampler2D"));
                    reDataNode->append_node(doc.allocate_node(rapidxml::node_element, "source", dataNode->value()));
                    reSource->append_attribute(doc.allocate_attribute("sid", dataAttribute->value()));
                    reMeshNode->append_node(reSource);
                    #endif
                }
                source = source->next_sibling("newparam");
            }
            
            //Find Technique
            source = meshNode->first_node("technique");
            if(!source) goto endParsingXML;
            source = source->first_node();
            if(!source) goto endParsingXML;
            #ifdef reExportModelFiles
            reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "technique"));
            reSource->append_node(reSource = doc.allocate_node(rapidxml::node_element, "phong"));
            #endif
            dataNode = source->first_node("diffuse");
            if(dataNode) {
                #ifdef reExportModelFiles
                reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "diffuse"));
                #endif
                dataNode = dataNode->first_node("texture");
                if(dataNode) {
                    dataAttribute = dataNode->first_attribute("texture");
                    if(!dataAttribute) goto endParsingXML;
                    std::map<std::string, std::string>::iterator iterator = samplerURLs.find(dataAttribute->value());
                    if(iterator == samplerURLs.end()) {
                        printf("ERROR: No sampler by id %s found.\n", dataAttribute->value());
                        goto endParsingXML;
                    }
                    material.diffuseURL = iterator->second;
                    #ifdef reExportModelFiles
                    reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "texture"));
                    reDataNode->append_attribute(doc.allocate_attribute("texture", dataAttribute->value()));
                    #endif
                }
            }
            materials[id] = material;
            geometry = geometry->next_sibling("effect");
        }
    }
    
    library = collada->first_node("library_materials");
    if(library) {
        #ifdef reExportModelFiles
        reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_materials"));
        #endif
        geometry = library->first_node("material");
        while(geometry) {
            dataAttribute = geometry->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            meshNode = geometry->first_node("instance_effect");
            if(!meshNode) goto endParsingXML;
            dataAttribute = meshNode->first_attribute("url");
            if(!dataAttribute) goto endParsingXML;
            std::map<std::string, Material>::iterator iterator = materials.find(dataAttribute->value()+1);
            if(iterator == materials.end()) {
                printf("ERROR: No material by id %s found.\n", dataAttribute->value()+1);
                goto endParsingXML;
            }
            materials[id] = iterator->second;
            materials.erase(iterator);
            #ifdef reExportModelFiles
            reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "material"));
            reGeometry->append_attribute(doc.allocate_attribute("id", id));
            reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "instance_effect"));
            reMeshNode->append_attribute(doc.allocate_attribute("url", dataAttribute->value()));
            #endif
            geometry = geometry->next_sibling("material");
        }
    }
    
    library = collada->first_node("library_visual_scenes");
    if(library) {
        #ifdef reExportModelFiles
        reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_visual_scenes"));
        #endif
        geometry = library->first_node("visual_scene");
        while(geometry) {
            #ifdef reExportModelFiles
            reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "visual_scene"));
            #endif
            meshNode = geometry->first_node("node");
            while(meshNode) {
                #ifdef reExportModelFiles
                reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "node"));
                #endif
                
                //Map Materials
                source = meshNode->first_node("instance_controller");
                if(!source) source = meshNode->first_node("instance_geometry");
                if(source) {
                    #ifdef reExportModelFiles
                    reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, source->name()));
                    #endif
                    dataNode = source->first_node("bind_material");
                    if(dataNode) {
                        dataNode = dataNode->first_node("technique_common");
                        if(!dataNode) goto endParsingXML;
                        dataNode = dataNode->first_node("instance_material");
                        if(!dataNode) goto endParsingXML;
                        dataAttribute = dataNode->first_attribute("symbol");
                        if(!dataAttribute) goto endParsingXML;
                        id = dataAttribute->value();
                        dataAttribute = dataNode->first_attribute("target");
                        if(!dataAttribute) goto endParsingXML;
                        std::map<std::string, Material>::iterator iterator = materials.find(dataAttribute->value()+1);
                        if(iterator == materials.end()) {
                            printf("ERROR: No material by id %s found.\n", dataAttribute->value()+1);
                            goto endParsingXML;
                        }
                        materials[id] = iterator->second;
                        #ifdef reExportModelFiles
                        reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "bind_material"));
                        reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "technique_common"));
                        reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "instance_material"));
                        reDataNode->append_attribute(doc.allocate_attribute("symbol", id));
                        reDataNode->append_attribute(doc.allocate_attribute("target", dataAttribute->value()));
                        #endif
                    }
                }
                
                //Load Skeletons
                source = meshNode->first_node("node");
                if(source) {
                    dataAttribute = source->first_attribute("type");
                    if(dataAttribute && strcmp(dataAttribute->value(), "JOINT") == 0) {
                        if(skeleton) {
                            printf("ERROR: More than one skeleton found.\n");
                            goto endParsingXML;
                        }
                        skeleton = new Skeleton();
                        #ifdef reExportModelFiles
                        skeleton->rootBone = readBone(&exportDoc, *skeleton, modelTransformMat, modelTransformMat, source, reMeshNode);
                        #else
                        skeleton->rootBone = readBone(&exportDoc, *skeleton, modelTransformMat, modelTransformMat, source, NULL);
                        #endif
                        if(skeleton->bones.size() > maxJointsCount) {
                            printf("ERROR: More joints (%lu) found than supported (%d).\n", skeleton->bones.size(), maxJointsCount);
                            goto endParsingXML;
                        }
                    }
                }
                
                meshNode = meshNode->next_sibling("node");
            }
            geometry = geometry->next_sibling("visual_scene");
        }
    }
    
    //Load Controllers
    library = collada->first_node("library_controllers");
    if(library) {
        geometry = library->first_node("controller");
        if(!skeleton || !geometry) {
            printf("ERROR: Controller found but no skeleton loaded.\n");
            goto endParsingXML;
        }
        meshNode = geometry->first_node("skin");
        if(!meshNode) goto endParsingXML;
        #ifdef reExportModelFiles
        reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_controllers"));
        reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "controller"));
        reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "skin"));
        #endif
        
        std::vector<Bone*> boneIndexMap(skeleton->bones.size());
        FloatArray weightData;
        weightData.data = NULL;
        source = meshNode->first_node("source");
        while(source) {
            dataNode = source->first_node("technique_common");
            if(!dataNode) goto endParsingXML;
            dataNode = dataNode->first_node("accessor");
            if(!dataNode) goto endParsingXML;
            dataNode = dataNode->first_node("param");
            if(!dataNode) goto endParsingXML;
            dataAttribute = dataNode->first_attribute("name");
            if(!dataAttribute) goto endParsingXML;
            
            id = dataAttribute->value();
            if(strcmp(id, "JOINT") != 0 && strcmp(id, "WEIGHT") != 0) {
                source = source->next_sibling("source");
                continue;
            }
            #ifdef reExportModelFiles
            reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "source"));
            reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "technique_common"));
            reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "accessor"));
            reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "param"));
            reDataNode->append_attribute(doc.allocate_attribute("name", id));
            #endif
            
            if(strcmp(id, "JOINT") == 0) {
                dataNode = source->first_node("Name_array");
                if(!dataNode) goto endParsingXML;
                dataAttribute = dataNode->first_attribute("count");
                if(!dataAttribute) goto endParsingXML;
                #ifdef reExportModelFiles
                reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "Name_array", doc.allocate_string(dataNode->value())));
                reDataNode->append_attribute(doc.allocate_attribute("count", dataAttribute->value()));
                #endif
                unsigned int count;
                sscanf(dataAttribute->value(), "%d", &count);
                if(count != skeleton->bones.size()) {
                    printf("ERROR: Controller found but bone count does not match the bone count of skeleton.\n");
                    goto endParsingXML;
                }
                char *dataStr = dataNode->value(), *lastPos = dataStr;
                for(unsigned int i = 0; i < skeleton->bones.size(); i ++) {
                    dataStr = strchr(lastPos, ' ');
                    if(dataStr) *dataStr = 0;
                    std::map<std::string, Bone*>::iterator boneIterator = skeleton->bones.find(lastPos);
                    if(boneIterator == skeleton->bones.end()) {
                        printf("ERROR: No bone by name %s found.\n", lastPos);
                        goto endParsingXML;
                    }
                    boneIndexMap[i] = boneIterator->second;
                    boneIterator->second->jointIndex = i;
                    lastPos = dataStr+1;
                }
            }else if(strcmp(id, "WEIGHT") == 0) {
                dataNode = source->first_node("float_array");
                if(!dataNode) goto endParsingXML;
                dataAttribute = dataNode->first_attribute("count");
                if(!dataAttribute) goto endParsingXML;
                #ifdef reExportModelFiles
                reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "float_array", doc.allocate_string(dataNode->value())));
                reDataNode->append_attribute(doc.allocate_attribute("count", dataAttribute->value()));
                #endif
                sscanf(dataAttribute->value(), "%d", &weightData.count);
                readFloatStr(dataNode->value(), weightData);
            }
            source = source->next_sibling("source");
        }
        if(!weightData.data) {
            printf("ERROR: No vertex weights found.\n");
            goto endParsingXML;
        }
        
        source = meshNode->first_node("vertex_weights");
        if(!source) goto endParsingXML;
        #ifdef reExportModelFiles
        reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "vertex_weights"));
        #endif
        
        int jointOffset = 1, weightOffset = -1;
        dataNode = source->first_node("input");
        while(dataNode) {
            dataAttribute = dataNode->first_attribute("semantic");
            if(!dataAttribute) goto endParsingXML;
            #ifdef reExportModelFiles
            reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "input"));
            reDataNode->append_attribute(doc.allocate_attribute("semantic", dataAttribute->value()));
             #endif
            if(strcmp(dataAttribute->value(), "JOINT") == 0) {
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &jointOffset);
                #ifdef reExportModelFiles
                reDataNode->append_attribute(doc.allocate_attribute("offset", dataAttribute->value()));
                #endif
            }else if(strcmp(dataAttribute->value(), "WEIGHT") == 0) {
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &weightOffset);
                #ifdef reExportModelFiles
                reDataNode->append_attribute(doc.allocate_attribute("offset", dataAttribute->value()));
                #endif
            }else{
                printf("ERROR: Unknown input semantic %s found.\n", dataAttribute->value());
                goto endParsingXML;
            }
            dataNode = dataNode->next_sibling("input");
        }
        
        if(jointOffset < 0 || weightOffset < 0) {
            printf("ERROR: Joint- or weight-offset missing in vertex_weights.\n");
            goto endParsingXML;
        }
        
        IntArray vcount, skinIndecies;
        vcount.data = NULL;
        skinIndecies.data = NULL;
        dataAttribute = source->first_attribute("count");
        if(!dataAttribute) goto endParsingXML;
        #ifdef reExportModelFiles
        reSource->append_attribute(doc.allocate_attribute("count", dataAttribute->value()));
        #endif
        sscanf(dataAttribute->value(), "%d", &vcount.count);
        dataNode = source->first_node("vcount");
        if(!dataNode) goto endParsingXML;
        #ifdef reExportModelFiles
        reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "vcount", doc.allocate_string(dataNode->value())));
        #endif
        readIntStr(dataNode->value(), vcount);
        skinIndecies.count = 0;
        for(unsigned int i = 0; i < vcount.count; i ++) {
            if(vcount.data[i] > jointsPerVertex) {
                printf("ERROR: More joints per vertex (%d) found than supported (%d).\n", vcount.data[i], jointsPerVertex);
                goto endParsingXML;
            }
            skinIndecies.count += vcount.data[i]*2;
        }
        dataNode = source->first_node("v");
        if(!dataNode) goto endParsingXML;
        #ifdef reExportModelFiles
        reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "v", doc.allocate_string(dataNode->value())));
        #endif
        readIntStr(dataNode->value(), skinIndecies);
        skinData.count = vcount.count*jointsPerVertex*2;
        skinData.stride = jointsPerVertex*2;
        skinData.data = new float[vcount.count*jointsPerVertex*2];
        skinIndecies.count = 0;
        for(unsigned int i = 0; i < vcount.count; i ++) {
            for(unsigned int j = 0; j < jointsPerVertex; j ++) {
                if(j < vcount.data[i]) {
                    skinData.data[jointsPerVertex*2*i+j] = weightData.data[skinIndecies.data[skinIndecies.count+j*2+weightOffset]];
                    skinData.data[jointsPerVertex*2*i+jointsPerVertex+j] = skinIndecies.data[skinIndecies.count+j*2+jointOffset];
                }else{
                    skinData.data[jointsPerVertex*2*i+j] = 0.0;
                    skinData.data[jointsPerVertex*2*i+jointsPerVertex+j] = 0.0;
                }
            }
            skinIndecies.count += vcount.data[i]*2;
        }
        
        delete [] vcount.data;
        delete [] skinIndecies.data;
        delete [] weightData.data;
        
        geometry = geometry->next_sibling("controllers");
        if(!skeleton) {
            printf("ERROR: More than one controller found.\n");
            goto endParsingXML;
        }
    }
    if(skeleton && !skinData.data) {
        printf("ERROR: No skin for skeleton found.\n");
        goto endParsingXML;
    }
    
    //Load Geometries
    library = collada->first_node("library_geometries");
    if(!library) goto endParsingXML;
    #ifdef reExportModelFiles
    reCollada->append_node(reLibrary = doc.allocate_node(rapidxml::node_element, "library_geometries"));
    #endif
    
    geometry = library->first_node("geometry");
    while(geometry) {
        meshNode = geometry->first_node("mesh");
        if(!meshNode) goto endParsingXML;
        #ifdef reExportModelFiles
        reLibrary->append_node(reGeometry = doc.allocate_node(rapidxml::node_element, "geometry"));
        reGeometry->append_node(reMeshNode = doc.allocate_node(rapidxml::node_element, "mesh"));
        #endif
        
        //Load Sources
        std::map<std::string, FloatArray> floatArrays;
        source = meshNode->first_node("source");
        while(source) {
            dataNode = source->first_node("float_array");
            if(!dataNode) goto endParsingXML;
            dataAttribute = dataNode->first_attribute("count");
            if(!dataAttribute) goto endParsingXML;
            #ifdef reExportModelFiles
            char* countStr = dataAttribute->value();
            #endif
            FloatArray floatArray;
            sscanf(dataAttribute->value(), "%d", &floatArray.count);
            if(!floatArray.count) goto endParsingXML;
            readFloatStr(dataNode->value(), floatArray);
            dataNode = source->first_node("technique_common");
            if(!dataNode) goto endParsingXML;
            dataNode = dataNode->first_node("accessor");
            if(!dataNode) goto endParsingXML;
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            dataAttribute = dataNode->first_attribute("stride");
            if(!dataAttribute) goto endParsingXML;
            sscanf(dataAttribute->value(), "%d", &floatArray.stride);
            #ifdef reExportModelFiles
            reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "source"));
            reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "technique_common"));
            reDataNode->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "accessor"));
            reSource->append_attribute(doc.allocate_attribute("id", id));
            reDataNode->append_attribute(doc.allocate_attribute("stride", dataAttribute->value()));
            #endif
            
            floatArrays[id] = floatArray;
            if(floatArray.stride == 3 && upAxis != AXIS_Y) {
                for(unsigned int i = 0; i < floatArray.count/floatArray.stride; i ++) {
                    Vector3 vec = Vector3(floatArray.data[i*3], floatArray.data[i*3+1], floatArray.data[i*3+2]) * modelTransformMat;
                    floatArray.data[i*3  ] = vec.x;
                    floatArray.data[i*3+1] = vec.y;
                    floatArray.data[i*3+2] = vec.z;
                }
            }
            char floatArrayData[16*floatArray.count], *floatArrayPos = floatArrayData;
            for(unsigned int i = 0; i < floatArray.count; i ++) {
                if(i > 0) *(floatArrayPos ++) = ' ';
                floatArrayPos += sprintf(floatArrayPos, "%4.8f", floatArray.data[i]);
            }
            
            #ifdef reExportModelFiles
            reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "float_array", doc.allocate_string(floatArrayData)));
            reDataNode->append_attribute(doc.allocate_attribute("count", countStr));
            #endif
            
            floatArray.count /= floatArray.stride;
            if(floatArray.stride == 2)
                for(unsigned int i = 0; i < floatArray.count; i ++)
                    floatArray.data[i*2+1] = 1.0-floatArray.data[i*2+1];
            source = source->next_sibling("source");
        }
        
        //Load Vertices
        std::map<std::string, std::vector<VertexReference>* > vertexReferenceArrays;
        std::map<std::string, std::vector<VertexReference>* >::iterator iteratorB;
        source = meshNode->first_node("vertices");
        while(source) {
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            vertexReferenceArrays[id] = new std::vector<VertexReference>();
            #ifdef reExportModelFiles
            reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "vertices"));
            reSource->append_attribute(doc.allocate_attribute("id", id));
            #endif
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                dataAttribute = dataNode->first_attribute("semantic");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.name = dataAttribute->value();
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.source = &floatArrays[dataAttribute->value()+1];
                vertexReferenceArrays[id]->push_back(vertexReference);
                #ifdef reExportModelFiles
                reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "input"));
                reDataNode->append_attribute(doc.allocate_attribute("source", dataAttribute->value()));
                reDataNode->append_attribute(doc.allocate_attribute("semantic", vertexReference.name));
                #endif
                dataNode = dataNode->next_sibling("input");
            }
            source = source->next_sibling("vertices");
        }
        
        //Load Triangles
        source = meshNode->first_node("triangles");
        if(!source) source = meshNode->first_node("polylist");
        while(source) {
            dataAttribute = source->first_attribute("count");
            if(!dataAttribute) goto endParsingXML;
            mesh = new Mesh();
            meshes.push_back(mesh);
            sscanf(dataAttribute->value(), "%d", &mesh->elementsCount);
            #ifdef reExportModelFiles
            reMeshNode->append_node(reSource = doc.allocate_node(rapidxml::node_element, "triangles"));
            reSource->append_attribute(doc.allocate_attribute("count", dataAttribute->value()));
            #endif
            
            //Set material
            dataAttribute = source->first_attribute("material");
            if(!dataAttribute) goto endParsingXML;
            std::map<std::string, Material>::iterator material = materials.find(dataAttribute->value());
            if(material == materials.end()) {
                printf("ERROR: No material by id %s found.\n", dataAttribute->value());
                goto endParsingXML;
            }
            if(material->second.diffuseURL.size() > 0) {
                mesh->diffuse = filePackage->getTexture(material->second.diffuseURL.c_str());
                mesh->diffuse->uploadToVRAM();
                mesh->diffuse->unloadFromRAM();
            }
            #ifdef reExportModelFiles
            reSource->append_attribute(doc.allocate_attribute("material", dataAttribute->value()));
            #endif
            
            unsigned int dataIndex, valueIndex, indexCount = 0, strideIndex = 0;
            std::map<std::string, VertexReference> vertexReferences;
            
            dataNode = source->first_node("input");
            while(dataNode) {
                unsigned int offset;
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &offset);
                #ifdef reExportModelFiles
                reSource->append_node(reDataNode = doc.allocate_node(rapidxml::node_element, "input"));
                reDataNode->append_attribute(doc.allocate_attribute("offset", dataAttribute->value()));
                dataAttribute = dataNode->first_attribute("semantic");
                if(!dataAttribute) goto endParsingXML;
                reDataNode->append_attribute(doc.allocate_attribute("semantic", dataAttribute->value()));
                #endif
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                #ifdef reExportModelFiles
                reDataNode->append_attribute(doc.allocate_attribute("source", dataAttribute->value()));
                #endif
                iteratorB = vertexReferenceArrays.find(dataAttribute->value()+1);
                if(iteratorB == vertexReferenceArrays.end()) {
                    VertexReference vertexReference;
                    vertexReference.source = &floatArrays[dataAttribute->value()+1];
                    vertexReference.offset = offset;
                    dataAttribute = dataNode->first_attribute("semantic");
                    if(!dataAttribute) goto endParsingXML;
                    vertexReference.name = dataAttribute->value();
                    vertexReferences[vertexReference.name] = vertexReference;
                }else{
                    std::vector<VertexReference>* vertexReferenceArray = iteratorB->second;
                    for(unsigned int i = 0; i < vertexReferenceArray->size(); i ++) {
                        (*vertexReferenceArray)[i].offset = offset;
                        vertexReferences[(*vertexReferenceArray)[i].name] = (*vertexReferenceArray)[i];
                    }
                }
                indexCount ++;
                dataNode = dataNode->next_sibling("input");
            }
            
            if(indexCount == 0) {
                printf("ERROR: No indecies found.\n");
                goto endParsingXML;
            }
            
            std::map<std::string, VertexReference>::iterator iterator;
            if(skeleton) {
                iterator = vertexReferences.find("POSITION");
                if(iterator == vertexReferences.end()) goto endParsingXML;
                VertexReference vertexReference;
                vertexReference.source = &skinData;
                vertexReference.offset = iterator->second.offset;
                vertexReference.name = (char*) "WEIGHTJOINT";
                vertexReferences[vertexReference.name] = vertexReference;
            }
            
            for(iterator = vertexReferences.begin(); iterator != vertexReferences.end(); iterator ++) {
                if(strcmp(iterator->second.name, "POSITION") == 0) {
                    mesh->postions = strideIndex;
                }else if(strcmp(iterator->second.name, "TEXCOORD") == 0) {
                    mesh->texcoords = strideIndex;
                }else if(strcmp(iterator->second.name, "NORMAL") == 0) {
                    mesh->normals = strideIndex;
                }else if(strcmp(iterator->second.name, "TANGENT") == 0) {
                    mesh->tangents = strideIndex;
                }else if(strcmp(iterator->second.name, "BITANGENT") == 0) {
                    mesh->bitangents = strideIndex;
                }else if(strcmp(iterator->second.name, "WEIGHTJOINT") == 0) {
                    mesh->weightJoints = strideIndex;
                }else{
                    printf("ERROR: Unknown vertex type: %s\n", iterator->second.name);
                    goto endParsingXML;
                }
                strideIndex += iterator->second.source->stride;
            }
            
            //Triangulator
            IntArray indexBuffer, indexCountBuffer;
            indexBuffer.data = NULL;
            indexCountBuffer.data = NULL;
            dataNode = source->first_node("vcount");
            if(dataNode) {
                #ifdef reExportModelFiles
                reSource->append_node(doc.allocate_node(rapidxml::node_element, "vcount", doc.allocate_string(dataNode->value())));
                #endif
                
                indexCountBuffer.data = NULL;
                indexCountBuffer.count = mesh->elementsCount;
                readIntStr(dataNode->value(), indexCountBuffer);
                mesh->elementsCount = 0;
                
                for(unsigned int i = 0; i < indexCountBuffer.count; i ++)
                    mesh->elementsCount += indexCountBuffer.data[i]-2;
                indexBuffer.count = (mesh->elementsCount+2*indexCountBuffer.count)*indexCount;
                mesh->elementsCount *= 3;
                
                if(mesh->elementsCount == indexCountBuffer.count*3) { //All Elements are Triangles
                    delete [] indexCountBuffer.data;
                    indexCountBuffer.data = NULL;
                    #ifdef reExportModelFiles
                    reSource->remove_node(reSource->first_node("vcount"));
                    #endif
                }else{ //Triangulation needed
                    indexBuffer.data = new int[mesh->elementsCount*indexCount];
                }
            }else{
                mesh->elementsCount *= 3;
                indexBuffer.count = indexCount*mesh->elementsCount;
            }
            
            dataNode = source->first_node("p");
            if(!dataNode) goto endParsingXML;
            #ifdef reExportModelFiles
            reSource->append_node(doc.allocate_node(rapidxml::node_element, "p", doc.allocate_string(dataNode->value())));
            #endif
            readIntStr(dataNode->value(), indexBuffer);
            
            if(indexCountBuffer.data) {
                unsigned int finishedElements = 0, indexBufferSize = indexBuffer.count;
                indexBuffer.count = indexCount*mesh->elementsCount;
                mesh->elementsCount = 0;
                for(int i = indexCountBuffer.count-1; i >= 0; i --) {
                    unsigned int copyCount = indexCountBuffer.data[i]*indexCount,
                                 expandCount = (indexCountBuffer.data[i]-2)*3*indexCount,
                                 indexAuxBuffer[copyCount], index;
                    
                    index = indexBufferSize-finishedElements-copyCount;
                    for(unsigned int j = 0; j < copyCount; j ++)
                        indexAuxBuffer[j] = indexBuffer.data[index+j];
                    
                    for(unsigned int j = 0; j < indexCountBuffer.data[i]-2; j ++)
                        for(unsigned int k = 0; k < indexCount; k ++) {
                            index = indexBuffer.count-mesh->elementsCount*indexCount-expandCount+j*indexCount*3+k;
                            indexBuffer.data[index] = indexAuxBuffer[k];
                            indexBuffer.data[index+indexCount] = indexAuxBuffer[k+indexCount*(j+1)];
                            indexBuffer.data[index+indexCount*2] = indexAuxBuffer[k+indexCount*(j+2)];
                        }
                    
                    finishedElements += indexCountBuffer.data[i]*indexCount;
                    mesh->elementsCount += (indexCountBuffer.data[i]-2)*3;
                }/*
                #ifdef reExportModelFiles
                char indexBufferData[indexBuffer.count*8], *indexBufferPos = indexBufferData;
                for(unsigned int i = 0; i < indexBuffer.count; i ++) {
                    if(i > 0) *(indexBufferPos ++) = ' ';
                    indexBufferPos += sprintf(indexBufferPos, "%d", indexBuffer.data[i]);
                }
                reSource->first_node("p")->value(doc.allocate_string(indexBufferData));
                char indexBufferLen[16];
                sprintf(indexBufferLen, "%d", mesh->elementsCount/3);
                dataAttribute = reSource->first_attribute("count");
                dataAttribute->value(doc.allocate_string(indexBufferLen));
                #endif
                */
            }
            
            //Indecizer
            float dataBuffer[strideIndex*mesh->elementsCount];
            glGenBuffers(1, &mesh->vbo);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
            if(indexCount > 1) { //Use IndexBuffer
                int combinationIndex, combinationCount = 0;
                unsigned int indecies[mesh->elementsCount];
                for(unsigned int i = 0; i < mesh->elementsCount; i ++) {
                    combinationIndex = -1;
                    for(unsigned int j = 0; j < combinationCount && combinationIndex == -1; j ++) {
                        combinationIndex = j;
                        for(unsigned int k = 0; k < indexCount; k ++)
                            if(indexBuffer.data[j*indexCount+k] != indexBuffer.data[i*indexCount+k]) {
                                combinationIndex = -1;
                                break;
                            }
                    }
                    if(combinationIndex == -1) {
                        combinationIndex = combinationCount ++;
                        for(unsigned int k = 0; k < indexCount; k ++)
                            indexBuffer.data[combinationIndex*indexCount+k] = indexBuffer.data[i*indexCount+k];
                        dataIndex = combinationIndex*strideIndex;
                        for(iterator = vertexReferences.begin(); iterator != vertexReferences.end(); iterator ++) {
                            valueIndex = indexBuffer.data[i*indexCount+iterator->second.offset]*iterator->second.source->stride;
                            for(unsigned char j = 0; j < iterator->second.source->stride; j ++)
                                dataBuffer[dataIndex ++] = iterator->second.source->data[valueIndex+j];
                        }
                    }
                    indecies[i] = combinationIndex;
                }
                
                glBufferData(GL_ARRAY_BUFFER, combinationCount*strideIndex*sizeof(float), dataBuffer, GL_STATIC_DRAW);
                glGenBuffers(1, &mesh->ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->elementsCount*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
                if(combinationCount*strideIndex*sizeof(float)+mesh->elementsCount*sizeof(unsigned int) > mesh->elementsCount*strideIndex*sizeof(float)) {
                    printf("Loading COLLADA, Index-Mode is contra-productive: Used %lu bytes, but %lu would be necessary.", combinationCount*strideIndex*sizeof(float)+mesh->elementsCount*sizeof(unsigned int), mesh->elementsCount*strideIndex*sizeof(float));
                }
            }else{ //Don't use IndexBuffer
                dataIndex = 0;
                for(unsigned int i = 0; i < mesh->elementsCount; i ++)
                    for(iterator = vertexReferences.begin(); iterator != vertexReferences.end(); iterator ++) {
                        valueIndex = indexBuffer.data[i*indexCount+iterator->second.offset]*iterator->second.source->stride;
                        for(unsigned char j = 0; j < iterator->second.source->stride; j ++)
                            dataBuffer[dataIndex ++] = iterator->second.source->data[valueIndex+j];
                    }
                glBufferData(GL_ARRAY_BUFFER, mesh->elementsCount*strideIndex*sizeof(float), dataBuffer, GL_STATIC_DRAW);
            }
            
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            delete [] indexBuffer.data;
            rapidxml::xml_node<xmlUsedCharType>* sourceB = source->next_sibling("triangles");
            if(!sourceB) sourceB = source->next_sibling("polylist");
            source = sourceB;
        }
        
        //Clean up
        std::map<std::string, FloatArray>::iterator iterator;
        for(iterator = floatArrays.begin(); iterator != floatArrays.end(); iterator ++)
            delete [] iterator->second.data;
        for(iteratorB = vertexReferenceArrays.begin(); iteratorB != vertexReferenceArrays.end(); iteratorB ++)
            delete iteratorB->second;
        
        geometry = geometry->next_sibling("geometry");
        if(skeleton && geometry) {
            printf("ERROR: More than one geometry but only one controller found.\n");
            goto endParsingXML;
        }
    }
    
    //Export optimized file
    #ifdef reExportModelFiles
    {
        char exportFileData[fileSize*2],
             *end = rapidxml::print(exportFileData, exportDoc, 0);
        *end = 0;
        
        std::string url(filePath);
        url += ".rex";
        FILE* fp = fopen(url.c_str(), "w");
        if(!fp) {
            printf("The file %s couldn't be found.", url.c_str());
            goto endParsingXML;
        }
        fwrite(exportFileData, 1, end-exportFileData, fp);
        fclose(fp);
    }
    #endif
    
    endParsingXML:
    if(skinData.data) delete skinData.data;
    doc.clear();
    exportDoc.clear();
    delete [] fileData;
    return true;
}

void Model::draw(float discardDensity) {
    if(renderingState == RenderingScreen)
        mainShaderProgram->use();
    else if(renderingState == RenderingShadow)
        shadowShaderProgram->use();
    currentShaderProgram->setUniformF("discardDensity", discardDensity);
    lightManager.setLights();
    
    for(unsigned int i = 0; i < meshes.size(); i ++)
        meshes[i]->draw();
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glDisableVertexAttribArray(TANGENT_ATTRIBUTE);
    glDisableVertexAttribArray(BITANGENT_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Model::draw(float discardDensity, SkeletonPose* skeletonPose) {
    if(!skeleton) {
        printf("ERROR: No Skeleton for pose found.\n");
        return;
    }
    
    if(renderingState == RenderingScreen)
        mainSkeletonShaderProgram->use();
    else if(renderingState == RenderingShadow)
        shadowSkeletonShaderProgram->use();
    currentShaderProgram->setUniformF("discardDensity", discardDensity);
    lightManager.setLights();
    
    Matrix4* mats = new Matrix4[skeleton->bones.size()];
    skeletonPose->calculateDisplayMatrix(skeleton->rootBone, NULL, mats);
    currentShaderProgram->setUniformMatrix4("jointMats", mats, skeleton->bones.size());
    delete [] mats;
    for(unsigned int i = 0; i < meshes.size(); i ++)
        meshes[i]->draw();
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glDisableVertexAttribArray(WEIGHT_ATTRIBUTE);
    glDisableVertexAttribArray(JOINT_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

SkeletonPose::SkeletonPose(Skeleton* skeleton) {
    std::map<std::string, Bone*>::iterator boneIterator;
    for(boneIterator = skeleton->bones.begin(); boneIterator != skeleton->bones.end(); boneIterator ++) {
        BonePose* bonePose = new BonePose;
        bonePose->poseMat.setIdentity();
        bonePose->dynamicMat.setIdentity();
        bonePoses[boneIterator->second->name] = bonePose;
    }
}

SkeletonPose::~SkeletonPose() {
    std::map<std::string, BonePose*>::iterator bonePoseIterator;
    for(bonePoseIterator = bonePoses.begin(); bonePoseIterator != bonePoses.end(); bonePoseIterator ++)
        delete bonePoseIterator->second;
}

void SkeletonPose::calculateBonePose(Bone* bone, Bone* parentBone) {
    BonePose* bonePose = bonePoses[bone->name];
    bonePose->dynamicMat.setIdentity();
    if(parentBone) {
        bonePose->dynamicMat = bonePose->poseMat;
        bonePose->dynamicMat.translate(bone->staticMat.pos-parentBone->staticMat.pos);
        bonePose->dynamicMat *= bonePoses[parentBone->name]->dynamicMat;
    }else{
        bonePose->dynamicMat = bonePose->poseMat;
        bonePose->dynamicMat.translate(bone->staticMat.pos);
    }
    for(unsigned int i = 0; i < bone->children.size(); i ++)
        calculateBonePose(bone->children[i], bone);
}

void SkeletonPose::calculateDisplayMatrix(Bone* bone, Bone* parentBone, Matrix4* mats) {
    Matrix4 mat;
    mat.setIdentity();
    mat.translate(bone->staticMat.pos*-1);
    mats[bone->jointIndex] = mat * bonePoses[bone->name]->dynamicMat;
    for(unsigned int i = 0; i < bone->children.size(); i ++)
        calculateDisplayMatrix(bone->children[i], bone, mats);
}