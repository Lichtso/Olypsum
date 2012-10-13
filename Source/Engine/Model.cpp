//
//  Model.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "WorldManager.h"

#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2
#define jointsPerVertex 3
#define maxJointsCount 64

Mesh::Mesh() {
    elementsCount = 0;
    vbo = ibo = 0;
    transparent = false;
    postions = texcoords = normals = weightJoints = -1;
    diffuse = effectMap = heightMap = NULL;
}

Mesh::~Mesh() {
    if(vbo) glDeleteBuffers(1, &vbo);
    if(ibo) glDeleteBuffers(1, &ibo);
    if(diffuse) fileManager.releaseTexture(diffuse);
    if(effectMap) fileManager.releaseTexture(effectMap);
    if(heightMap) fileManager.releaseTexture(heightMap);
}

void Mesh::draw(ObjectBase* object) {
    if(!vbo || elementsCount == 0) {
        log(error_log, "No vertex data to draw in mesh.");
        return;
    }
    if(postions == -1) {
        log(error_log, "No postions data to draw in mesh.");
        return;
    }
    
    //AnimatedObject* animatedObject = (object->type != ObjectInstance_Animated && object->type != ObjectInstance_NPC) ? NULL : (AnimatedObject*) object;
    AnimatedObject* animatedObject = (weightJoints == 0) ? NULL : (AnimatedObject*) object;
    
    if(diffuse)
        diffuse->use(GL_TEXTURE_2D, 0);
    if(effectMap)
        effectMap->use(GL_TEXTURE_2D, 1);
    else{
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    if(heightMap)
        heightMap->use(GL_TEXTURE_2D, 2);
    else{
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    
    if(!lightManager.currentShadowLight) {
        if(!transparent || object->type != ObjectInstance_Water) {
            if(heightMap) {
                if(animatedObject) {
                    if(transparent && blendingQuality > 0)
                        shaderPrograms[glassSkeletalBumpGeometrySP]->use();
                    else
                        shaderPrograms[skeletalBumpGeometrySP]->use();
                }else{
                    if(transparent && blendingQuality > 0)
                        shaderPrograms[glassBumpGeometrySP]->use();
                    else
                        shaderPrograms[solidBumpGeometrySP]->use();
                }
            }else{
                if(animatedObject) {
                    if(transparent && blendingQuality > 0)
                        shaderPrograms[glassSkeletalGeometrySP]->use();
                    else
                        shaderPrograms[skeletalGeometrySP]->use();
                }else{
                    if(transparent && blendingQuality > 0)
                        shaderPrograms[glassGeometrySP]->use();
                    else
                        shaderPrograms[solidGeometrySP]->use();
                }
            }
        }else if(object->type == ObjectInstance_Water) {
            WaterObject* waterObject = (WaterObject*) object;
            shaderPrograms[waterSP]->use();
            char str[64];
            for(unsigned int i = 0; i < MAX_WAVES; i ++) {
                bool active = (i < waterObject->waves.size());
                sprintf(str, "waveLen[%d]", i);
                currentShaderProgram->setUniformF(str, active ? (1.0/waterObject->waves[i].length) : 0.0);
                sprintf(str, "waveAge[%d]", i);
                currentShaderProgram->setUniformF(str, active ? waterObject->waves[i].age/waterObject->waves[i].length*waterObject->waveSpeed-M_PI*2.0 : 0.0);
                sprintf(str, "waveAmp[%d]", i);
                currentShaderProgram->setUniformF(str, active ? waterObject->waves[i].ampitude*(1.0-waterObject->waves[i].age/waterObject->waves[i].maxAge) : 0.0);
                sprintf(str, "waveOri[%d]", i);
                if(active)
                    currentShaderProgram->setUniformVec2(str, waterObject->waves[i].originX, waterObject->waves[i].originY);
                else
                    currentShaderProgram->setUniformVec2(str, 0.0, 0.0);
            }
        }
    }
    currentShaderProgram->setUniformF("discardDensity", object->getDiscardDensity());
    if(animatedObject)
        currentShaderProgram->setUniformMatrix4("jointMats", animatedObject->skeletonPose->mats, animatedObject->skeletonPose->skeleton->bones.size());
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    unsigned int byteStride = 3;
    if(texcoords >= 0) byteStride += 2;
    if(normals >= 0) byteStride += 3;
    if(weightJoints >= 0) byteStride += jointsPerVertex*2;
    byteStride *= sizeof(float);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(postions*sizeof(float)));
    if(texcoords >= 0) currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(texcoords*sizeof(float)));
    if(normals >= 0) currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(normals*sizeof(float)));
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
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glDisableVertexAttribArray(WEIGHT_ATTRIBUTE);
    glDisableVertexAttribArray(JOINT_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

struct VertexReference {
    XMLValueArray<float>* source;
    unsigned int offset;
    char* name;
};

struct Material {
    bool transparent;
    std::string diffuseURL, effectMapURL, heightMapURL;
};

static Bone* readBone(Skeleton& skeleton, Matrix4& deTransform, Matrix4& parentMat, rapidxml::xml_node<xmlUsedCharType>* dataNode) {
    rapidxml::xml_attribute<xmlUsedCharType> *typeAttribute, *sidAttribute = dataNode->first_attribute("sid");
    Bone *bone = new Bone;
    bone->name = sidAttribute->value();
    bone->staticMat.readTransform(dataNode);
    bone->staticMat *= parentMat;
    rapidxml::xml_node<xmlUsedCharType> *childNode = dataNode->first_node("node");
    while(childNode) {
        typeAttribute = childNode->first_attribute("type");
        sidAttribute = childNode->first_attribute("sid");
        if(!typeAttribute || !sidAttribute || strcmp(typeAttribute->value(), "JOINT") != 0) continue;
        bone->children.push_back(readBone(skeleton, deTransform, bone->staticMat, childNode));
        childNode = childNode->next_sibling("node");
    }
    skeleton.bones[bone->name] = bone;
    return bone;
}

static std::string readTextureURL(std::map<std::string, std::string> samplerURLs, rapidxml::xml_node<xmlUsedCharType>* dataNode) {
    dataNode = dataNode->first_node("texture");
    if(dataNode) {
        rapidxml::xml_attribute<xmlUsedCharType>* dataAttribute = dataNode->first_attribute("texture");
        if(!dataAttribute) return std::string();
        std::map<std::string, std::string>::iterator iterator = samplerURLs.find(dataAttribute->value());
        if(iterator == samplerURLs.end()) {
            log(error_log, std::string("No sampler by id ")+dataAttribute->value()+" found.");
            return std::string();
        }
        return iterator->second;
    }
    return std::string();
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
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, filePath, true);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *collada, *library, *geometry, *meshNode, *source, *dataNode;
    rapidxml::xml_attribute<xmlUsedCharType> *dataAttribute;
    std::map<std::string, std::string> textureURLs;
    std::map<std::string, Material> materials;
    XMLValueArray<float> skinData;
    char upAxis = AXIS_Y, *id;
    Matrix4 modelTransformMat;
    Mesh* mesh;
    
    collada = doc.first_node("COLLADA");
    if(!collada) goto endParsingXML;
    
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
    modelTransformMat.setIdentity();
    if(upAxis == AXIS_X)
        modelTransformMat.rotateZ(M_PI_2);
    else if(upAxis == AXIS_Z)
        modelTransformMat.rotateX(M_PI_2);
    
    //Load TextureURLs
    library = collada->first_node("library_images");
    if(library) {
        geometry = library->first_node("image");
        while(geometry) {
            dataAttribute = geometry->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            meshNode = geometry->first_node("init_from");
            if(!meshNode) goto endParsingXML;
            textureURLs[dataAttribute->value()] = meshNode->value();
            geometry = geometry->next_sibling("image");
        }
    }
    
    //Load Materials
    library = collada->first_node("library_effects");
    if(library) {
        geometry = library->first_node("effect");
        while(geometry) {
            dataAttribute = geometry->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            Material material;
            material.transparent = false;
            meshNode = geometry->first_node("profile_COMMON");
            if(!meshNode) goto endParsingXML;
            std::map<std::string, std::string> surfaceURLs, samplerURLs;
            
            //Find Surfaces
            source = meshNode->first_node("newparam");
            while(source) {
                dataNode = source->first_node("surface");
                if(dataNode) {
                    dataAttribute = dataNode->first_attribute("type");
                    if(!dataAttribute || strcmp(dataAttribute->value(), "2D") != 0) break;
                    dataNode = dataNode->first_node("init_from");
                    dataAttribute = source->first_attribute("sid");
                    if(!dataNode || !dataAttribute) break;
                    std::map<std::string, std::string>::iterator iterator = textureURLs.find(dataNode->value());
                    if(iterator == textureURLs.end()) {
                        log(error_log, std::string("No texture by id ")+dataNode->value()+" found.");
                        goto endParsingXML;
                    }
                    surfaceURLs[dataAttribute->value()] = iterator->second;
                }
                source = source->next_sibling("newparam");
            }
            
            //Find Samplers
            source = meshNode->first_node("newparam");
            while(source) {
                dataNode = source->first_node("sampler2D");
                if(dataNode) {
                    dataNode = dataNode->first_node("source");
                    dataAttribute = source->first_attribute("sid");
                    if(!dataNode || !dataAttribute) break;
                    std::map<std::string, std::string>::iterator iterator = surfaceURLs.find(dataNode->value());
                    if(iterator == surfaceURLs.end()) {
                        log(error_log, std::string("No surface by id ")+dataNode->value()+" found.");
                        goto endParsingXML;
                    }
                    samplerURLs[dataAttribute->value()] = iterator->second;
                }
                source = source->next_sibling("newparam");
            }
            
            //Find Technique
            source = meshNode->first_node("technique");
            if(!source) goto endParsingXML;
            source = source->first_node();
            if(!source) goto endParsingXML;
            while(source) {
                while(source->first_node("technique"))
                    source = source->first_node("technique");
                
                if((dataNode = source->first_node("diffuse")))
                    material.diffuseURL = readTextureURL(samplerURLs, dataNode);
                if((dataNode = source->first_node("specular")))
                    material.effectMapURL = readTextureURL(samplerURLs, dataNode);
                if((dataNode = source->first_node("bump")))
                    material.heightMapURL = readTextureURL(samplerURLs, dataNode);
                if((dataNode = source->first_node("index_of_refraction"))) {
                    dataNode = dataNode->first_node();
                    if(dataNode) {
                        float value;
                        sscanf(dataNode->value(), "%f", &value);
                        if(value > 1.0) material.transparent = true;
                    }
                }
                source = source->next_sibling();
            }
            materials[id] = material;
            geometry = geometry->next_sibling("effect");
        }
    }
    
    library = collada->first_node("library_materials");
    if(library) {
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
                log(error_log, std::string("No material by id ")+(dataAttribute->value()+1)+" found.");
                goto endParsingXML;
            }
            materials[id] = iterator->second;
            materials.erase(iterator);
            geometry = geometry->next_sibling("material");
        }
    }
    
    library = collada->first_node("library_visual_scenes");
    if(library) {
        geometry = library->first_node("visual_scene");
        while(geometry) {
            meshNode = geometry->first_node("node");
            while(meshNode) {
                //Map Materials
                source = meshNode->first_node("instance_controller");
                if(!source) source = meshNode->first_node("instance_geometry");
                if(source) {
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
                            log(error_log, std::string("No material by id ")+(dataAttribute->value()+1)+" found.");
                            goto endParsingXML;
                        }
                        materials[id] = iterator->second;
                    }
                }
                
                //Load Skeletons
                source = meshNode->first_node("node");
                if(source) {
                    dataAttribute = source->first_attribute("type");
                    if(dataAttribute && strcmp(dataAttribute->value(), "JOINT") == 0) {
                        if(skeleton) {
                            log(error_log, "More than one skeleton found.");
                            goto endParsingXML;
                        }
                        skeleton = new Skeleton();
                        skeleton->rootBone = readBone(*skeleton, modelTransformMat, modelTransformMat, source);
                        if(skeleton->bones.size() > maxJointsCount) {
                            char buffer[128];
                            sprintf(buffer, "More joints (%lu) found than supported (%d).\n", skeleton->bones.size(), maxJointsCount);
                            log(error_log, buffer);
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
            log(error_log, "Controller found but no skeleton loaded.");
            goto endParsingXML;
        }
        meshNode = geometry->first_node("skin");
        if(!meshNode) goto endParsingXML;
        
        std::vector<Bone*> boneIndexMap(skeleton->bones.size());
        XMLValueArray<float> weightData;
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
            
            if(strcmp(id, "JOINT") == 0) {
                dataNode = source->first_node("Name_array");
                if(!dataNode) goto endParsingXML;
                dataAttribute = dataNode->first_attribute("count");
                if(!dataAttribute) goto endParsingXML;
                unsigned int count;
                sscanf(dataAttribute->value(), "%d", &count);
                if(count != skeleton->bones.size()) {
                    log(error_log, "Controller found but bone count does not match the bone count of skeleton.");
                    goto endParsingXML;
                }
                char *dataStr = dataNode->value(), *lastPos = dataStr;
                for(unsigned int i = 0; i < skeleton->bones.size(); i ++) {
                    dataStr = strchr(lastPos, ' ');
                    if(dataStr) *dataStr = 0;
                    std::map<std::string, Bone*>::iterator boneIterator = skeleton->bones.find(lastPos);
                    if(boneIterator == skeleton->bones.end()) {
                        log(error_log, std::string("No bone by name ")+lastPos+" found.");
                        goto endParsingXML;
                    }
                    boneIndexMap[i] = boneIterator->second;
                    boneIterator->second->jointIndex = i;
                    lastPos = dataStr+1;
                }
            }else if(strcmp(id, "WEIGHT") == 0) {
                weightData.readString(source->first_node("float_array"), "%f");
            }
            source = source->next_sibling("source");
        }
        if(!weightData.data) {
            log(error_log, "No vertex weights found.");
            goto endParsingXML;
        }
        
        source = meshNode->first_node("vertex_weights");
        if(!source) goto endParsingXML;
        
        int jointOffset = 1, weightOffset = -1;
        dataNode = source->first_node("input");
        while(dataNode) {
            dataAttribute = dataNode->first_attribute("semantic");
            if(!dataAttribute) goto endParsingXML;
            if(strcmp(dataAttribute->value(), "JOINT") == 0) {
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &jointOffset);
            }else if(strcmp(dataAttribute->value(), "WEIGHT") == 0) {
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &weightOffset);
            }else{
                log(error_log, std::string("Unknown input semantic ")+dataAttribute->value()+" found.");
                goto endParsingXML;
            }
            dataNode = dataNode->next_sibling("input");
        }
        
        if(jointOffset < 0 || weightOffset < 0) {
            log(error_log, "Joint- or weight-offset missing in vertex_weights.");
            goto endParsingXML;
        }
        
        XMLValueArray<int> vcount, skinIndecies;
        dataAttribute = source->first_attribute("count");
        if(!dataAttribute) goto endParsingXML;
        sscanf(dataAttribute->value(), "%d", &vcount.count);
        if(!vcount.readString(source->first_node("vcount"), vcount.count, "%d"))
            goto endParsingXML;
        skinIndecies.count = 0;
        for(unsigned int i = 0; i < vcount.count; i ++) {
            if(vcount.data[i] > jointsPerVertex) {
                char buffer[128];
                sprintf(buffer, "More joints per vertex (%d) found than supported (%d).\n", vcount.data[i], jointsPerVertex);
                log(error_log, buffer);
                goto endParsingXML;
            }
            skinIndecies.count += vcount.data[i]*2;
        }
        if(!skinIndecies.readString(source->first_node("v"), skinIndecies.count, "%d"))
            goto endParsingXML;
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
        
        geometry = geometry->next_sibling("controllers");
        if(!skeleton) {
            log(error_log, "More than one controller found.");
            goto endParsingXML;
        }
    }
    if(skeleton && !skinData.data) {
        log(error_log, "No skin for skeleton found.");
        goto endParsingXML;
    }
    
    //Load Geometries
    library = collada->first_node("library_geometries");
    if(!library) goto endParsingXML;
    
    geometry = library->first_node("geometry");
    while(geometry) {
        meshNode = geometry->first_node("mesh");
        if(!meshNode) goto endParsingXML;
        
        //Load Sources
        std::map<std::string, XMLValueArray<float>*> floatArrays;
        source = meshNode->first_node("source");
        while(source) {
            XMLValueArray<float>* floatArray = new XMLValueArray<float>();
            if(!floatArray->readString(source->first_node("float_array"), "%f"))
                goto endParsingXML;
            dataNode = source->first_node("technique_common");
            if(!dataNode) goto endParsingXML;
            dataNode = dataNode->first_node("accessor");
            if(!dataNode) goto endParsingXML;
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            floatArrays[dataAttribute->value()] = floatArray;
            dataAttribute = dataNode->first_attribute("stride");
            if(!dataAttribute) goto endParsingXML;
            sscanf(dataAttribute->value(), "%d", &floatArray->stride);
            
            if(floatArray->stride == 3 && upAxis != AXIS_Y) {
                for(unsigned int i = 0; i < floatArray->count/floatArray->stride; i ++) {
                    Vector3 vec = Vector3(floatArray->data[i*3], floatArray->data[i*3+1], floatArray->data[i*3+2]) * modelTransformMat;
                    floatArray->data[i*3  ] = vec.x;
                    floatArray->data[i*3+1] = vec.y;
                    floatArray->data[i*3+2] = vec.z;
                }
            }
            char floatArrayData[16*floatArray->count], *floatArrayPos = floatArrayData;
            for(unsigned int i = 0; i < floatArray->count; i ++) {
                if(i > 0) *(floatArrayPos ++) = ' ';
                floatArrayPos += sprintf(floatArrayPos, "%4.8f", floatArray->data[i]);
            }
            
            floatArray->count /= floatArray->stride;
            if(floatArray->stride == 2)
                for(unsigned int i = 0; i < floatArray->count; i ++)
                    floatArray->data[i*2+1] = 1.0-floatArray->data[i*2+1];
            source = source->next_sibling("source");
        }
        
        //Load Vertices
        std::map<std::string, std::vector<VertexReference>*> vertexReferenceArrays;
        source = meshNode->first_node("vertices");
        while(source) {
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            vertexReferenceArrays[id] = new std::vector<VertexReference>();
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                dataAttribute = dataNode->first_attribute("semantic");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.name = dataAttribute->value();
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.source = floatArrays[dataAttribute->value()+1];
                vertexReferenceArrays[id]->push_back(vertexReference);
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
            
            //Set material
            dataAttribute = source->first_attribute("material");
            if(!dataAttribute) goto endParsingXML;
            std::map<std::string, Material>::iterator material = materials.find(dataAttribute->value());
            if(material == materials.end()) {
                log(error_log, std::string("No material by id ")+dataAttribute->value()+" found.");
                goto endParsingXML;
            }
            mesh->transparent = material->second.transparent;
            mesh->diffuse = (material->second.diffuseURL.size()) ? filePackage->getTexture(material->second.diffuseURL.c_str(), GL_COMPRESSED_RGBA) : NULL;
            mesh->effectMap = (material->second.effectMapURL.size()) ? filePackage->getTexture(material->second.effectMapURL.c_str(), GL_COMPRESSED_RGB) : NULL;
            mesh->heightMap = (material->second.heightMapURL.size()) ? filePackage->getTexture(material->second.heightMapURL.c_str(), GL_NORMAL_MAP) : NULL;
            
            unsigned int dataIndex, valueIndex, indexCount = 0, strideIndex = 0;
            std::map<std::string, VertexReference> vertexReferences;
            dataNode = source->first_node("input");
            while(dataNode) {
                unsigned int offset;
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &offset);
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                decltype(vertexReferenceArrays)::iterator iterator = vertexReferenceArrays.find(dataAttribute->value()+1);
                if(iterator == vertexReferenceArrays.end()) {
                    VertexReference vertexReference;
                    vertexReference.source = floatArrays[dataAttribute->value()+1];
                    vertexReference.offset = offset;
                    dataAttribute = dataNode->first_attribute("semantic");
                    if(!dataAttribute) goto endParsingXML;
                    vertexReference.name = dataAttribute->value();
                    vertexReferences[vertexReference.name] = vertexReference;
                }else{
                    std::vector<VertexReference>* vertexReferenceArray = iterator->second;
                    for(unsigned int i = 0; i < vertexReferenceArray->size(); i ++) {
                        (*vertexReferenceArray)[i].offset = offset;
                        vertexReferences[(*vertexReferenceArray)[i].name] = (*vertexReferenceArray)[i];
                    }
                }
                indexCount ++;
                dataNode = dataNode->next_sibling("input");
            }
            
            if(indexCount == 0) {
                log(error_log, "No indecies found.");
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
                }else if(strcmp(iterator->second.name, "WEIGHTJOINT") == 0) {
                    mesh->weightJoints = strideIndex;
                }else{
                    log(error_log, std::string("Unknown vertex type: ")+iterator->second.name);
                    goto endParsingXML;
                }
                strideIndex += iterator->second.source->stride;
            }
            
            //Triangulator
            XMLValueArray<int> indexBuffer, indexCountBuffer;
            dataNode = source->first_node("vcount");
            if(dataNode) {
                indexCountBuffer.readString(dataNode, mesh->elementsCount, "%d");
                mesh->elementsCount = 0;
                for(unsigned int i = 0; i < indexCountBuffer.count; i ++)
                    mesh->elementsCount += indexCountBuffer.data[i]-2;
                indexBuffer.count = (mesh->elementsCount+2*indexCountBuffer.count)*indexCount;
                mesh->elementsCount *= 3;
                
                if(mesh->elementsCount == indexCountBuffer.count*3) //All Elements are Triangles
                    indexCountBuffer.clear();
                else //Triangulation needed
                    indexBuffer.data = new int[mesh->elementsCount*indexCount];
                
            }else{
                mesh->elementsCount *= 3;
                indexBuffer.count = indexCount*mesh->elementsCount;
            }
            
            if(!indexBuffer.readString(source->first_node("p"), indexBuffer.count, "%d"))
                goto endParsingXML;
            
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
                }
                indexCountBuffer.clear();
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
                /*if(combinationCount*strideIndex*sizeof(float)+mesh->elementsCount*sizeof(unsigned int) > mesh->elementsCount*strideIndex*sizeof(float)) {
                 char buffer[128];
                 sprintf(buffer, "Loading COLLADA, Index-Mode is contra-productive: Used %lu bytes, but %lu would be necessary.", combinationCount*strideIndex*sizeof(float)+mesh->elementsCount*sizeof(unsigned int), mesh->elementsCount*strideIndex*sizeof(float));
                 log(warning_log, buffer);
                 }*/
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
            
            rapidxml::xml_node<xmlUsedCharType>* sourceB = source->next_sibling("triangles");
            if(!sourceB) sourceB = source->next_sibling("polylist");
            source = sourceB;
        }
        
        //Clean up
        for(auto iterator = vertexReferenceArrays.begin(); iterator != vertexReferenceArrays.end(); iterator ++)
            delete iterator->second;
        for(auto iterator = floatArrays.begin(); iterator != floatArrays.end(); iterator ++)
            delete iterator->second;
        
        geometry = geometry->next_sibling("geometry");
        if(skeleton && geometry) {
            log(error_log, "More than one geometry but only one controller found.");
            goto endParsingXML;
        }
    }
    
    endParsingXML:
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    return true;
}

void Model::draw(ObjectBase* object) {
    if(lightManager.currentShadowLight) {
        lightManager.currentShadowLight->selectShaderProgram(skeleton);
        for(unsigned int i = 0; i < meshes.size(); i ++)
            meshes[i]->draw(object);
    }else for(unsigned int i = 0; i < meshes.size(); i ++) {
        if(meshes[i]->transparent && blendingQuality > 0) {
            TransparentMesh* tMesh = new TransparentMesh();
            tMesh->object = object;
            tMesh->mesh = meshes[i];
            objectManager.transparentAccumulator.push_back(tMesh);
            continue;
        }
        meshes[i]->draw(object);
    }
}

SkeletonPose::SkeletonPose(Skeleton* skeletonB) {
    skeleton = skeletonB;
    mats = new Matrix4[skeleton->bones.size()];
    std::map<std::string, Bone*>::iterator boneIterator;
    for(boneIterator = skeleton->bones.begin(); boneIterator != skeleton->bones.end(); boneIterator ++) {
        bonePoses[boneIterator->second->name] = Matrix4();
        bonePoses[boneIterator->second->name].setIdentity();
    }
}

SkeletonPose::~SkeletonPose() {
    delete [] mats;
}

void SkeletonPose::calculateBonePose(Bone* bone, Bone* parentBone) {
    mats[bone->jointIndex] = bonePoses[bone->name];
    if(parentBone) {
        mats[bone->jointIndex].translate(bone->staticMat.pos-parentBone->staticMat.pos);
        mats[bone->jointIndex] *= mats[parentBone->jointIndex];
    }else
        mats[bone->jointIndex].translate(bone->staticMat.pos);
    for(unsigned int i = 0; i < bone->children.size(); i ++)
        calculateBonePose(bone->children[i], bone);
    Matrix4 mat;
    mat.setIdentity();
    mat.translate(bone->staticMat.pos*-1);
    mats[bone->jointIndex] = mat * mats[bone->jointIndex];
}

void SkeletonPose::calculate() {
    calculateBonePose(skeleton->rootBone, NULL);
}