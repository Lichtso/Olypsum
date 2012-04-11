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
    
    glBindBuffer(GL_ARRAY_BUFFER, vboF);
    unsigned int byteStride = 3;
    if(texcoords) byteStride += 2;
    if(normals) byteStride += 3;
    if(tangents) byteStride += 3;
    if(bitangents) byteStride += 3;
    byteStride *= sizeof(float);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, byteStride, (float*)(postions*sizeof(float)));
    if(texcoords) currentShaderProgram->setAttribute(TEXTURE_COORD_ATTRIBUTE, 2, byteStride, (float*)(texcoords*sizeof(float)));
    if(normals) currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, byteStride, (float*)(normals*sizeof(float)));
    if(tangents) currentShaderProgram->setAttribute(TANGENT_ATTRIBUTE, 3, byteStride, (float*)(tangents*sizeof(float)));
    if(bitangents) currentShaderProgram->setAttribute(BITANGENT_ATTRIBUTE, 3, byteStride, (float*)(bitangents*sizeof(float)));
    
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

static bool readFloatStr(char* dataStr, FloatArray& array) {
    array.data = new float[array.count];
    
    char *lastPos = dataStr;
    for(unsigned int i = 0; i < array.count; i ++) {
        dataStr = strchr(lastPos, ' ');
        if(!dataStr) return false;
        *dataStr = 0;
        sscanf(lastPos, "%f", &array.data[i]);
        lastPos = dataStr+1;
    }
    
    return true;
}

static bool readIntStr(char* dataStr, IntArray& array) {
    array.data = new int[array.count];
    
    char *lastPos = dataStr;
    for(unsigned int i = 0; i < array.count; i ++) {
        dataStr = strchr(lastPos, ' ');
        if(!dataStr) return false;
        *dataStr = 0;
        sscanf(lastPos, "%d", &array.data[i]);
        lastPos = dataStr+1;
    }
    
    return true;
}

Model::Model() {
    useCounter = 1;
}

Model::~Model() {
    for(unsigned int i = 0; i < meshes.size(); i ++)
        delete meshes[i];
}

bool Model::loadCollada(const char* filePath) {
    if(meshes.size() > 0) return false;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    if(!parseXmlFile(doc, filePath)) return false;
    
    rapidxml::xml_node<xmlUsedCharType> *collada, *library_geometries, *geometry, *meshNode, *source, *dataNode;
    rapidxml::xml_attribute<xmlUsedCharType> *dataAttribute;
    Mesh* mesh;
    char* id;
    
    collada = doc.first_node("COLLADA");
    if(!collada) goto endParsingXML;
    
    library_geometries = collada->first_node("library_geometries");
    if(!library_geometries) goto endParsingXML;
    
    //Load Meshes
    geometry = library_geometries->first_node("geometry");
    while(geometry) {
        meshNode = geometry->first_node("mesh");
        if(!meshNode) goto endParsingXML;
        
        //Load Sources
        std::map<char*, FloatArray> floatArrays;
        source = meshNode->first_node("source");
        while(source) {
            dataNode = source->first_node("float_array");
            if(!dataNode) goto endParsingXML;
            dataAttribute = dataNode->first_attribute("count");
            if(!dataAttribute) goto endParsingXML;
            FloatArray floatArray;
            sscanf(dataAttribute->value(), "%d", &floatArray.count);
            if(!floatArray.count || !readFloatStr(dataNode->value(), floatArray)) goto endParsingXML;
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
            source = source->next_sibling("source");
        }
        
        //Load Vertices
        std::map<char*, std::vector<VertexReference>* > vertexReferenceArrays;
        std::map<char*, std::vector<VertexReference>* >::iterator iteratorB;
        source = meshNode->first_node("vertices");
        while(source) {
            dataAttribute = source->first_attribute("id");
            if(!dataAttribute) goto endParsingXML;
            id = dataAttribute->value();
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                dataAttribute = source->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.source = &floatArrays[dataAttribute->value()+1];
                dataAttribute = source->first_attribute("semantic");
                if(!dataAttribute) goto endParsingXML;
                vertexReference.name = dataAttribute->value();
                vertexReferenceArrays[id] = new std::vector<VertexReference>();
                vertexReferenceArrays[id]->push_back(vertexReference);
                dataNode = dataNode->next_sibling("input");
            }
            source = source->next_sibling("vertices");
        }
        
        //Load Triangles
        source = meshNode->first_node("triangles");
        while(source) {
            dataAttribute = source->first_attribute("count");
            if(!dataAttribute) goto endParsingXML;
            mesh = new Mesh();
            meshes.push_back(mesh);
            sscanf(dataAttribute->value(), "%d", &mesh->elementsCount);
            mesh->elementsCount *= 3;
            dataAttribute = source->first_attribute("material");
            if(!dataAttribute) goto endParsingXML;
            //TODO: Load texture
            
            unsigned int dataIndex, indexCount = 0, strideIndex = 0;
            std::map<char*, VertexReference> vertexReferences;
            
            dataNode = source->first_node("input");
            while(dataNode) {
                VertexReference vertexReference;
                unsigned int offset;
                dataAttribute = source->first_attribute("offset");
                if(!dataAttribute) goto endParsingXML;
                sscanf(dataAttribute->value(), "%d", &offset);
                dataAttribute = source->first_attribute("source");
                if(!dataAttribute) goto endParsingXML;
                iteratorB = vertexReferenceArrays.find(dataAttribute->value()+1);
                if(iteratorB == vertexReferenceArrays.end()) {
                    vertexReference.source = &floatArrays[dataAttribute->value()+1];
                    vertexReference.offset = offset;
                    dataAttribute = source->first_attribute("semantic");
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
            
            dataNode = source->first_node("p");
            if(!dataNode) goto endParsingXML;
            IntArray indexBuffer;
            indexBuffer.count = indexCount*mesh->elementsCount;
            if(!readIntStr(dataNode->value(), indexBuffer)) goto endParsingXML;
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
                            dataBuffer[dataIndex] = iterator->second.source->data[indexBuffer.data[i*indexCount+iterator->second.offset]];
                            dataIndex ++;
                        }
                    }
                    indecies[i] = combinationIndex;
                }
                glBufferData(GL_ARRAY_BUFFER, combinationCount*strideIndex*sizeof(float), dataBuffer, GL_STATIC_DRAW);
                glGenBuffers(1, &mesh->ibo);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->ibo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh->elementsCount*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
            }else{ //Don't use IndexBuffer
                dataIndex = 0;
                for(unsigned int i = 0; i < mesh->elementsCount; i ++)
                    for(iterator = vertexReferences.begin(); iterator != vertexReferences.end(); iterator ++) {
                        dataBuffer[dataIndex] = iterator->second.source->data[indexBuffer.data[i*indexCount+iterator->second.offset]];
                        dataIndex ++;
                    }
                glBufferData(GL_ARRAY_BUFFER, mesh->elementsCount*strideIndex*sizeof(float), dataBuffer, GL_STATIC_DRAW);
            }
            glBindBuffer(GL_ARRAY_BUFFER, 0);
            delete [] indexBuffer.data;
            source = source->next_sibling("triangles");
        }
        
        //Clean up
        std::map<char*, FloatArray>::iterator iterator;
        for(iterator = floatArrays.begin(); iterator != floatArrays.end(); iterator ++)
            delete [] iterator->second.data;
        for(iteratorB = vertexReferenceArrays.begin(); iteratorB != vertexReferenceArrays.end(); iteratorB ++)
            delete iteratorB->second;
        
        geometry = geometry->next_sibling("geometry");
    }
    
    endParsingXML:
    doc.clear();
    return true;
}

void Model::draw() {
    for(unsigned int i = 0; i < meshes.size(); i ++)
        meshes[i]->draw();
}