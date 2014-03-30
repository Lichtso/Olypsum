//
//  XMLUtilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "XMLUtilities.h"

std::unique_ptr<char[]> readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& filePath, bool logs) {
    std::unique_ptr<char[]> content = readFile(filePath, logs);
    if(!content) return NULL;
    doc.parse<0>(content.get());
    return content;
}

void addXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>* parent, const char* name, const char* value) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    node->name(name);
    attribute->name("value");
    attribute->value(value);
    node->append_attribute(attribute);
    parent->append_node(node);
}

bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& filePath, bool logs) {
    std::ofstream file;
    file.open(filePath.c_str(), std::ios_base::trunc);
    if(!file.is_open()) {
        if(!logs) return false;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be opened.");
        return false;
    }
    file << doc;
    file.close();
    return true;
}

btTransform readTransformationXML(rapidxml::xml_node<xmlUsedCharType>* node) {
    btTransform transform;
    transform.setIdentity();
    node = node->first_node();
    while(node) {
        std::string name(node->name());
        std::transform(name.begin(), name.end(), name.begin(), ::tolower);
        if(name == "matrix") {
            XMLValueArray<btScalar> matrixData;
            matrixData.readString(node->value(), "%f");
            btTransform mat;
            mat.setBasis(btMatrix3x3(matrixData.data[0], matrixData.data[1], matrixData.data[2],
                                     matrixData.data[4], matrixData.data[5], matrixData.data[6],
                                     matrixData.data[8], matrixData.data[9], matrixData.data[10]));
            mat.setOrigin(btVector3(matrixData.data[3], matrixData.data[7], matrixData.data[11]));
            transform = transform * mat;
        }else if(name == "translate") {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            mat.setOrigin(btVector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]));
            transform = transform * mat;
        }else if(name == "rotate") {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            mat.setRotation(btQuaternion(btVector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]), vectorData.data[3]/180.0*M_PI));
            transform = transform * mat;
        }else if(name == "scale") {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            mat.setBasis(mat.getBasis().scaled(btVector3(vectorData.data[0], vectorData.data[1], vectorData.data[2])));
            transform = transform * mat;
        }
        node = node->next_sibling();
    }
    return transform;
}

rapidxml::xml_node<xmlUsedCharType>* writeTransformationXML(rapidxml::xml_document<xmlUsedCharType> &doc, btTransform& transform) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    node->name("Matrix");
    btScalar values[16];
    transform.getBasis().transpose().getOpenGLSubMatrix(values);
    values[3] = transform.getOrigin().x();
    values[7] = transform.getOrigin().y();
    values[11] = transform.getOrigin().z();
    values[12] = values[13] = values[14] = btScalar(0.0);
    values[15] = btScalar(1.0);
    
    std::ostringstream ss;
    for(unsigned char i = 0; i < 16; i ++) {
        if(i > 0) ss << " ";
        ss << values[i];
    }
    node->value(doc.allocate_string(ss.str().c_str()));
    return node;
}

std::size_t hashXMLNode(rapidxml::xml_node<xmlUsedCharType>* node) {
    std::size_t hash = CityHash64(node->name());
    if(node->value())
        hash ^= CityHash64(node->value());
    
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute();
    while(attribute) {
        hash ^= CityHash64(attribute->name());
        if(attribute->value())
            hash ^= CityHash64(attribute->value());
        attribute = attribute->next_attribute();
    }
    
    node = node->first_node();
    while(node) {
        hash ^= hashXMLNode(node);
        node = node->next_sibling();
    }
    
    return hash;
}