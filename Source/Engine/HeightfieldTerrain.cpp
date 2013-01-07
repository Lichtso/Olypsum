//
//  HeightfieldTerrain.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//
//

#import "LevelManager.h"

HeightfieldTerrain::HeightfieldTerrain(rapidxml::xml_node<char> *node, LevelLoader *levelLoader) :heights(NULL) {
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ibo);
    levelLoader->pushObject(this);
    
    diffuse = fileManager.initResource<Texture>(node->first_node("Diffuse"));
    if(!diffuse) return;
    diffuse->uploadTexture(GL_TEXTURE_2D_ARRAY_EXT, GL_COMPRESSED_RGB);
    glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if(diffuse->depth <= 1) {
        log(error_log, "Tried to construct HeightfieldTerrain with invalid \"EffectMap\" texture.");
        return;
    }
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("EffectMap");
    if(parameterNode) {
        effectMap = fileManager.initResource<Texture>(parameterNode);
        effectMap->uploadTexture(GL_TEXTURE_2D_ARRAY_EXT, GL_COMPRESSED_RGB);
        glTexParameteri(GL_TEXTURE_2D_ARRAY_EXT, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        if(effectMap->depth <= 1) {
            log(error_log, "Tried to construct HeightfieldTerrain with invalid \"EffectMap\" texture.");
            return;
        }
    }
    
    XMLValueArray<float> vecData;
    parameterNode = node->first_node("HeightsMap");
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    if(parameterNode) {
        std::shared_ptr<Texture> heightMap = fileManager.initResource<Texture>(parameterNode);
        if(!heightMap) return;
        width = heightMap->width;
        length = heightMap->height;
        heights = new float[width * length];
        unsigned char stride = heightMap->surface->format->BytesPerPixel, *data = static_cast<unsigned char*>(heightMap->surface->pixels);
        for(unsigned int y = 0; y < length; y ++)
            for(unsigned int x = 0; x < width; x ++)
                heights[y*width+x] = data[(y*width+x)*stride]/255.0;
    }else{
        parameterNode = node->first_node("Heights");
        if(!parameterNode) {
            log(error_log, "Tried to construct HeightfieldTerrain without \"Heights\"-node.");
            return;
        }
        attribute = parameterNode->first_attribute("width");
        if(!attribute) {
            log(error_log, "Tried to construct HeightfieldTerrain without \"width\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%d", &width);
        attribute = parameterNode->first_attribute("length");
        if(!attribute) {
            log(error_log, "Tried to construct HeightfieldTerrain without \"length\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%d", &length);
        vecData.readString(parameterNode->value(), "%f");
        if(vecData.count != width * length) {
            log(error_log, "Tried to construct HeightfieldTerrain with invalid \"Heights\"-node.");
            return;
        }
        heights = new float[width * length];
        memcpy(heights, vecData.data, width * length * sizeof(float));
        vecData.clear();
    }
    
    attribute = node->first_attribute("size");
    if(!attribute) {
        log(error_log, "Tried to construct HeightfieldTerrain without \"size\"-attribute.");
        return;
    }
    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(width, length, heights, 1.0/255.0,
                                                                              0.0, 1.0, 1, PHY_FLOAT, false);
    vecData.readString(attribute->value(), "%f");
    collisionShape->setLocalScaling(vecData.getVector3()*btVector3(2.0/width, 2.0, 2.0/length));
    
    body = new btCollisionObject();
    body->setCollisionShape(collisionShape);
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setUserPointer(this);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Static, CollisionMask_Object);
    
    attribute = node->first_attribute("textureScale");
    if(!attribute) {
        log(error_log, "Tried to construct HeightfieldTerrain without \"textureScale\"-attribute.");
        return;
    }
    vecData.readString(attribute->value(), "%f");
    textureScale.setX(vecData.data[0]);
    textureScale.setY(vecData.data[1]);
    textureScale.setZ(diffuse->depth);
    
    unsigned int index = 0, indeciesCount = (width-1) * (length-1) * 6;
    unsigned int* indecies = new unsigned int[indeciesCount];
    for(unsigned int y = 0; y < length-1; y ++)
        for(unsigned int x = 0; x < width-1; x ++) {
            indecies[index ++] = y*width + x;
            indecies[index ++] = (y+1)*width + x+1;
            indecies[index ++] = y*width + x+1;
            indecies[index ++] = y*width + x;
            indecies[index ++] = (y+1)*width + x;
            indecies[index ++] = (y+1)*width + x+1;
        }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indeciesCount*sizeof(unsigned int), indecies, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    delete [] indecies;
    
    updateModel();
}

HeightfieldTerrain::~HeightfieldTerrain() {
    glDeleteBuffers(1, &vbo);
    glDeleteBuffers(1, &ibo);
    if(heights) delete [] heights;
}

void HeightfieldTerrain::draw() {
    btVector3 size = body->getCollisionShape()->getLocalScaling();
    size *= btVector3(width, 1.0, length);
    modelMat.setIdentity();
    modelMat.setBasis(btMatrix3x3(btVector3(size.x(), 0, 0),
                                  btVector3(0, 0, size.y()),
                                  btVector3(0, size.z(), 0)));
    modelMat.setOrigin(size*-0.5);
    modelMat = getTransformation() * modelMat;
    
    if(objectManager.currentShadowLight) {
        unsigned int shaderProgram = solidShadowSP;
        if(dynamic_cast<PositionalLight*>(objectManager.currentShadowLight) && !cubemapsEnabled) shaderProgram += 4;
        shaderPrograms[shaderProgram]->use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
    }else{
        shaderPrograms[terrainGSP]->use();
        currentShaderProgram->setUniformVec3("textureScale", textureScale);
        diffuse->use(0);
        if(effectMap)
            effectMap->use(1);
        else{
            glActiveTexture(GL_TEXTURE1);
            glBindTexture(GL_TEXTURE_2D_ARRAY_EXT, 0);
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    currentShaderProgram->setAttribute(POSITION_ATTRIBUTE, 3, 6*sizeof(float), NULL);
    currentShaderProgram->setAttribute(NORMAL_ATTRIBUTE, 3, 6*sizeof(float), reinterpret_cast<float*>(3*sizeof(float)));
    glDrawElements(GL_TRIANGLES, width * length * 6, GL_UNSIGNED_INT, NULL);
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(NORMAL_ATTRIBUTE);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

btVector3 HeightfieldTerrain::getVertexAt(float* vertices, unsigned int x, unsigned int y) {
    unsigned int index = (y*width+x)*6;
    return btVector3(vertices[index], vertices[index+1], vertices[index+2]);
}

void HeightfieldTerrain::updateModel() {
    unsigned int index = 0, verticesCount = width * length * 6;
    float* vertices = new float[verticesCount];
    //Generate Positions
    float scaleX = 1.0 / (width-1), scaleY = 1.0 / (length-1);
    for(unsigned int y = 0; y < length; y ++)
        for(unsigned int x = 0; x < width; x ++) {
            vertices[index ++] = x * scaleX;
            vertices[index ++] = y * scaleY;
            vertices[index ++] = heights[y*width+x];
            index += 3;
        }
    index = 0; //Generate Normals
    btVector3 xAxis(1, 0, 0), yAxis(0, 1, 0);
    for(unsigned int y = 0; y < length; y ++)
        for(unsigned int x = 0; x < width; x ++) {
            btVector3 normal(0, 0, 0), pos = getVertexAt(vertices, x, y);
            if(x > 0)
                normal -= yAxis.cross(pos-getVertexAt(vertices, x-1, y));
            if(x < width-1)
                normal += yAxis.cross(pos-getVertexAt(vertices, x+1, y));
            if(y > 0)
                normal += xAxis.cross(pos-getVertexAt(vertices, x, y-1));
            if(y < length-1)
                normal -= xAxis.cross(pos-getVertexAt(vertices, x, y+1));
            normal.normalize();
            index += 3;
            vertices[index ++] = normal.x();
            vertices[index ++] = normal.y();
            vertices[index ++] = normal.z();
        }
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, verticesCount*sizeof(float), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    delete [] vertices;
}

rapidxml::xml_node<xmlUsedCharType>* HeightfieldTerrain::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("HeightfieldTerrain");
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    attribute->name("size");
    btHeightfieldTerrainShape* collisionShape = static_cast<btHeightfieldTerrainShape*>(body->getCollisionShape());
    btVector3 scale = collisionShape->getLocalScaling() * btVector3(width*0.5, 0.5, length*0.5);
    attribute->value(doc.allocate_string(stringOf(scale).c_str()));
    node->append_attribute(attribute);
    
    attribute = doc.allocate_attribute();
    attribute->name("textureScale");
    char buffer[64];
    sprintf(buffer, "%f %f", textureScale.x(), textureScale.y());
    attribute->value(doc.allocate_string(buffer));
    node->append_attribute(attribute);
    
    node->append_node(fileManager.writeResource(doc, "Diffuse", diffuse));
    if(effectMap)
        node->append_node(fileManager.writeResource(doc, "EffectMap", effectMap));
    
    rapidxml::xml_node<xmlUsedCharType>* heightsNode = doc.allocate_node(rapidxml::node_element);
    heightsNode->name("Heights");
    std::ostringstream data;
    for(unsigned int i = 0; i < width * length; i ++) {
        if(i > 0) data << " ";
        data << heights[i];
    }
    heightsNode->value(doc.allocate_string(data.str().c_str()));
    node->append_node(heightsNode);
    
    attribute = doc.allocate_attribute();
    attribute->name("width");
    sprintf(buffer, "%d", width);
    attribute->value(doc.allocate_string(buffer));
    heightsNode->append_attribute(attribute);
    
    attribute = doc.allocate_attribute();
    attribute->name("length");
    sprintf(buffer, "%d", length);
    attribute->value(doc.allocate_string(buffer));
    heightsNode->append_attribute(attribute);
    
    return node;
}