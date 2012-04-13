//
//  Model.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "FileManager.h"

Mesh::Mesh() {
    elementsCount = 0;
    vboF = vboI = ibo = 0;
    postions = texcoords = normals = -1;
    tangents = bitangents = -1;
    diffuse = normalMap = NULL;
}

Mesh::~Mesh() {
    if(vboF) glDeleteBuffers(1, &vboF);
    if(vboI) glDeleteBuffers(1, &vboI);
    if(ibo) glDeleteBuffers(1, &ibo);
    if(diffuse) fileManager.releaseTexture(diffuse);
    if(normalMap) fileManager.releaseTexture(normalMap);
}

void Mesh::draw() {
    if(!vboF || elementsCount == 0) {
        printf("ERROR: No vertex data to draw in mesh.\n");
        return;
    }
    if(postions == -1) {
        printf("ERROR: No postions data to draw in mesh.\n");
        return;
    }
    
    if(diffuse) diffuse->use(0);
    if(normalMap) normalMap->use(1);
    
    glBindBuffer(GL_ARRAY_BUFFER, vboF);
    unsigned int byteStride = 3;
    if(texcoords >= 0) byteStride += 2;
    if(normals >= 0) byteStride += 3;
    if(tangents >= 0) byteStride += 3;
    if(bitangents >= 0) byteStride += 3;
    byteStride *= sizeof(float);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(postions*sizeof(float)));
    if(texcoords >= 0) currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(texcoords*sizeof(float)));
    if(normals >= 0) currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(normals*sizeof(float)));
    if(tangents >= 0) currentShaderProgram->setAttribute(TANGENT_ATTRIBUTE, 3, byteStride, (float*)(tangents*sizeof(float)));
    if(bitangents >= 0) currentShaderProgram->setAttribute(BITANGENT_ATTRIBUTE, 3, byteStride, (float*)(bitangents*sizeof(float)));
    
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
    glDisableVertexAttribArray(TANGENT_ATTRIBUTE);
    glDisableVertexAttribArray(BITANGENT_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}



#define AXIS_X 0
#define AXIS_Y 1
#define AXIS_Z 2

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
        //printf("%p %d => %s\n", lastPos, i, lastPos);
        lastPos = dataStr+1;
    }
    
    return;
}

Model::Model() {
    useCounter = 1;
}

Model::~Model() {
    for(unsigned int i = 0; i < meshes.size(); i ++)
        delete meshes[i];
}

bool Model::loadCollada(FilePackage* filePackage, const char* filePath) {
    if(meshes.size() > 0) return false;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    char* fileData = parseXmlFile(doc, filePath);
    if(!fileData) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *collada, *library, *geometry, *meshNode, *source, *dataNode;
    rapidxml::xml_attribute<xmlUsedCharType> *dataAttribute;
    std::map<std::string, std::string> textureURLs;
    std::map<std::string, Material> materials;
    
    char upAxis = AXIS_Y, *id;
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
                        printf("ERROR: No texture by id %s found.\n", dataNode->value());
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
                        printf("ERROR: No surface by id %s found.\n", dataNode->value());
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
            
            dataNode = source->first_node("diffuse");
            if(dataNode) {
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
                }
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
                printf("ERROR: No material by id %s found.\n", dataAttribute->value()+1);
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
                source = meshNode->first_node("instance_controller");
                if(!source) source = meshNode->first_node("instance_geometry");
                if(!source) {
                    meshNode = meshNode->next_sibling("node");
                    continue;
                }
                dataNode = source->first_node("bind_material");
                if(!dataNode) {
                    meshNode = meshNode->next_sibling("node");
                    continue;
                }
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
                meshNode = meshNode->next_sibling("node");
            }
            geometry = geometry->next_sibling("visual_scene");
        }
    }
    
    
    //Load Geometries
    library = collada->first_node("library_geometries");
    if(!library) goto endParsingXML;
    
    geometry = library->first_node("geometry");
    while(geometry) {
        meshNode = geometry->first_node("mesh");
        if(!meshNode) goto endParsingXML;
        
        //Load Sources
        std::map<std::string, FloatArray> floatArrays;
        source = meshNode->first_node("source");
        while(source) {
            dataNode = source->first_node("float_array");
            if(!dataNode) goto endParsingXML;
            dataAttribute = dataNode->first_attribute("count");
            if(!dataAttribute) goto endParsingXML;
            FloatArray floatArray;
            sscanf(dataAttribute->value(), "%d", &floatArray.count);
            if(!floatArray.count) goto endParsingXML;
            readFloatStr(dataNode->value(), floatArray);
            dataNode = source->first_node("technique_common");
            if(!dataNode) goto endParsingXML;
            dataNode = dataNode->first_node("accessor");
            if(!dataNode) goto endParsingXML;
            dataAttribute = dataNode->first_attribute("stride");
            if(!dataAttribute) goto endParsingXML;
            sscanf(dataAttribute->value(), "%d", &floatArray.stride);
            floatArray.count /= floatArray.stride;
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            floatArrays[dataAttribute->value()] = floatArray;
            if(floatArray.stride == 3 && upAxis != AXIS_Y) {
                Matrix4 mat;
                mat.setIdentity();
                
                if(upAxis == AXIS_X)
                    mat.rotateZ(M_PI_2);
                else if(upAxis == AXIS_Z)
                    mat.rotateX(M_PI_2);
                
                for(unsigned int i = 0; i < floatArray.count; i ++) {
                    Vector3 vec = Vector3(floatArray.data[i*3], floatArray.data[i*3+1], floatArray.data[i*3+2]) * mat;
                    floatArray.data[i*3  ] = vec.x;
                    floatArray.data[i*3+1] = vec.y;
                    floatArray.data[i*3+2] = vec.z;
                }
            }else if(floatArray.stride == 2)
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
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.source = &floatArrays[dataAttribute->value()+1];
                dataAttribute = dataNode->first_attribute("semantic");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.name = dataAttribute->value();
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
                printf("ERROR: No material by id %s found.\n", dataAttribute->value());
                goto endParsingXML;
            }
            if(material->second.diffuseURL.size() > 0) {
                mesh->diffuse = filePackage->getTexture(material->second.diffuseURL.c_str());
                mesh->diffuse->uploadToVRAM();
                mesh->diffuse->unloadFromRAM();
            }
            
            unsigned int dataIndex, valueIndex, indexCount = 0, strideIndex = 0;
            std::map<char*, VertexReference> vertexReferences;
            
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                unsigned int offset;
                dataAttribute = dataNode->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &offset);
                dataAttribute = dataNode->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                iteratorB = vertexReferenceArrays.find(dataAttribute->value()+1);
                if(iteratorB == vertexReferenceArrays.end()) {
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
            
            std::map<char*, VertexReference>::iterator iterator;
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
                indexCountBuffer.data = NULL;
                indexCountBuffer.count = mesh->elementsCount;
                readIntStr(dataNode->value(), indexCountBuffer);
                mesh->elementsCount = 0;
                
                for(unsigned int i = 0; i < indexCountBuffer.count; i ++)
                    mesh->elementsCount += indexCountBuffer.data[i]-2;
                indexBuffer.count = (mesh->elementsCount+2*indexCountBuffer.count)*indexCount;
                mesh->elementsCount *= 3;
                
                if(mesh->elementsCount == indexCountBuffer.count*3) { //All Elements are Triangles
                    printf("Loading COLLADA: Vcount found but useless, please delete the <vcount> tag.\n");
                    delete [] indexCountBuffer.data;
                    indexCountBuffer.data = NULL;
                }else{ //Triangulation needed
                    indexBuffer.data = new int[mesh->elementsCount*indexCount];
                }
            }else{
                mesh->elementsCount *= 3;
                indexBuffer.count = indexCount*mesh->elementsCount;
            }
            
            dataNode = source->first_node("p");
            if(!dataNode) goto endParsingXML;
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
                }
                printf("Loading COLLADA: Vcount found: automatic triangulation.\n");
                printf("Please replace this element with: <p>");
                for(unsigned int i = 0; i < indexBuffer.count; i ++)
                    printf("%d ", indexBuffer.data[i]);
                printf("</p>, set count=\"%d\" and delete the <vcount> tag.\n", mesh->elementsCount/3);
            }
            
            //Indecizer
            float dataBuffer[strideIndex*mesh->elementsCount];
            glGenBuffers(1, &mesh->vboF);
            glBindBuffer(GL_ARRAY_BUFFER, mesh->vboF);
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
    }
    
    endParsingXML:
    doc.clear();
    delete [] fileData;
    return true;
}

void Model::draw() {
    for(unsigned int i = 0; i < meshes.size(); i ++)
        meshes[i]->draw();
}