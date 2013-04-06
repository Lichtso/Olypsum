//
//  TerrainObject.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//
//

#include "ScriptDisplayObject.h"

TerrainObject::TerrainObject(rapidxml::xml_node<char> *node, LevelLoader *levelLoader) :heights(NULL) {
    levelLoader->pushObject(this);
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = node->first_node("Diffuse");
    if(!parameterNode) {
        log(error_log, "Tried to construct TerrainObject without \"Diffuse\"-node.");
        return;
    }
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = parameterNode->first_attribute("src");
    if(!attribute) {
        log(error_log, "Found \"Diffuse\"-node without \"src\"-attribute.");
        return;
    }
    diffuse = fileManager.initResource<Texture>(attribute->value());
    diffuse->uploadTexture(GL_TEXTURE_2D_ARRAY, GL_COMPRESSED_RGB);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    if(diffuse->depth <= 1) {
        log(error_log, "Tried to construct TerrainObject with invalid \"EffectMap\" texture.");
        return;
    }
    
    parameterNode = node->first_node("EffectMap");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("src");
        if(!attribute) {
            log(error_log, "Found \"EffectMap\"-node without \"src\"-attribute.");
            return;
        }
        effectMap = fileManager.initResource<Texture>(attribute->value());
        effectMap->uploadTexture(GL_TEXTURE_2D_ARRAY, GL_COMPRESSED_RGB);
        glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        if(effectMap->depth <= 1) {
            log(error_log, "Tried to construct TerrainObject with invalid \"EffectMap\" texture.");
            return;
        }
    }
    
    parameterNode = node->first_node("HeightsMap");
    if(parameterNode) {
        attribute = parameterNode->first_attribute("src");
        if(!attribute) {
            log(error_log, "Found \"HeightsMap\"-node without \"src\"-attribute.");
            return;
        }
        std::shared_ptr<Texture> heightMap = fileManager.initResource<Texture>(attribute->value());
        if(!heightMap) return;
        width = heightMap->width;
        length = heightMap->height;
        bitDepth = 2;
        float scale = 1.0/pow(16.0, bitDepth);
        heights = new float[width * length];
        unsigned char stride = heightMap->surface->format->BytesPerPixel, *data = static_cast<unsigned char*>(heightMap->surface->pixels);
        for(unsigned int i = 0; i < width * length; i ++)
            heights[i] = scale*data[i*stride];
    }else{
        parameterNode = node->first_node("Heights");
        if(!parameterNode) {
            log(error_log, "Tried to construct TerrainObject without \"Heights\"-node.");
            return;
        }
        attribute = parameterNode->first_attribute("width");
        if(!attribute) {
            log(error_log, "Tried to construct TerrainObject without \"width\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%d", &width);
        attribute = parameterNode->first_attribute("length");
        if(!attribute) {
            log(error_log, "Tried to construct TerrainObject without \"length\"-attribute.");
            return;
        }
        sscanf(attribute->value(), "%d", &length);
        bitDepth = strlen(parameterNode->value()) / (width * length);
        float scale = 1.0/pow(16.0, bitDepth);
        std::string format = std::string("%")+stringOf(bitDepth)+'x';
        heights = new float[width * length];
        for(unsigned int value, i = 0; i < width * length; i ++) {
            sscanf(&parameterNode->value()[i*bitDepth], format.c_str(), &value);
            heights[i] = scale*value;
        }
    }
    
    parameterNode = node->first_node("Bounds");
    if(!parameterNode) {
        log(error_log, "Tried to construct TerrainObject without \"Bounds\"-node.");
        return;
    }
    XMLValueArray<float> vecData;
    vecData.readString(parameterNode->value(), "%f");
    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(width, length, heights, 1.0/255.0,
                                                                              0.0, 1.0, 1, PHY_FLOAT, false);
    collisionShape->setLocalScaling(vecData.getVector3()*btVector3(2.0/width, 2.0, 2.0/length));
    
    body = new btCollisionObject();
    body->setCollisionShape(collisionShape);
    body->setWorldTransform(BaseObject::readTransformtion(node, levelLoader));
    body->setUserPointer(this);
    objectManager.physicsWorld->addCollisionObject(body, CollisionMask_Static, CollisionMask_Object);
    
    parameterNode = node->first_node("TextureScale");
    if(!parameterNode) {
        log(error_log, "Tried to construct TerrainObject without \"TextureScale\"-node.");
        return;
    }
    vecData.readString(parameterNode->value(), "%f");
    textureScale.setX(vecData.data[0]);
    textureScale.setY(vecData.data[1]);
    textureScale.setZ(diffuse->depth);
    
    VertexArrayObject::Attribute attr;
    attr.size = 3;
    std::vector<VertexArrayObject::Attribute> attributes;
    attr.name = POSITION_ATTRIBUTE;
    attributes.push_back(attr);
    attr.name = NORMAL_ATTRIBUTE;
    attributes.push_back(attr);
    vao.init(attributes, true);
    
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
    vao.updateIndecies(indeciesCount, indecies, GL_UNSIGNED_INT, GL_STATIC_DRAW);
    delete [] indecies;
    
    updateModel();
}

TerrainObject::~TerrainObject() {
    if(heights) delete [] heights;
}

void TerrainObject::newScriptInstance() {
    v8::HandleScope handleScope;
    v8::Handle<v8::Value> external = v8::External::New(this);
    v8::Local<v8::Object> instance = scriptTerrainObject.functionTemplate->GetFunction()->NewInstance(1, &external);
    scriptInstance = v8::Persistent<v8::Object>::New(instance);
    scriptInstance->SetIndexedPropertiesToExternalArrayData(heights, v8::kExternalFloatArray, width*length);
}

void TerrainObject::draw() {
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
        if(objectManager.currentShadowIsParabolid) shaderProgram += 4;
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
            glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
        }
    }
    
    vao.draw();
}

btVector3 TerrainObject::getVertexAt(float* vertices, unsigned int x, unsigned int y) {
    unsigned int index = (y*width+x)*6;
    return btVector3(vertices[index], vertices[index+1], vertices[index+2]);
}

void TerrainObject::updateModel() {
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
    vao.updateVertices(verticesCount, vertices, GL_STATIC_DRAW);
    delete [] vertices;
}

rapidxml::xml_node<xmlUsedCharType>* TerrainObject::write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver) {
    rapidxml::xml_node<xmlUsedCharType>* node = BaseObject::write(doc, levelSaver);
    node->name("TerrainObject");
    
    rapidxml::xml_node<xmlUsedCharType>* parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("Bounds");
    btHeightfieldTerrainShape* collisionShape = static_cast<btHeightfieldTerrainShape*>(body->getCollisionShape());
    btVector3 size = collisionShape->getLocalScaling() * btVector3(width*0.5, 0.5, length*0.5);
    parameterNode->value(doc.allocate_string(stringOf(size).c_str()));
    node->append_node(parameterNode);
    
    parameterNode = doc.allocate_node(rapidxml::node_element);
    parameterNode->name("TextureScale");
    char buffer[64];
    sprintf(buffer, "%f %f", textureScale.x(), textureScale.y());
    parameterNode->value(doc.allocate_string(buffer));
    node->append_node(parameterNode);
    
    node->append_node(fileManager.writeResource(doc, "Diffuse", diffuse));
    if(effectMap)
        node->append_node(fileManager.writeResource(doc, "EffectMap", effectMap));
    
    rapidxml::xml_node<xmlUsedCharType>* heightsNode = doc.allocate_node(rapidxml::node_element);
    heightsNode->name("Heights");
    unsigned int scale = pow(16.0, bitDepth)-1;
    std::ostringstream data;
    data.setf(std::ios::hex, std::ios::basefield);
    for(unsigned int i = 0; i < width * length; i ++) {
        data.fill('0');
        data.width(bitDepth);
        data << (unsigned int)(scale*heights[i]);
    }
    heightsNode->value(doc.allocate_string(data.str().c_str()));
    node->append_node(heightsNode);
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
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