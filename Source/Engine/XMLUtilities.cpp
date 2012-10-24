//
//  XMLUtilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import "XMLUtilities.h"

std::unique_ptr<char[]> readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, bool logs) {
    std::ifstream file;
    file.open(filePath.c_str(), std::ios::ate);
    if(!file.is_open()) {
        if(!logs) return NULL;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be found.");
        return NULL;
    }
    unsigned int fileSize = file.tellg();
    std::unique_ptr<char[]> data(new char[fileSize+1]);
    file.seekg(0, std::ios::beg);
    file.read(data.get(), fileSize);
    file.close();
    data[fileSize] = 0;
    doc.parse<0>(data.get());
    return data;
}

void addXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>* nodes, const char* name, const char* value) {
    rapidxml::xml_node<xmlUsedCharType>* node = doc.allocate_node(rapidxml::node_element);
    rapidxml::xml_attribute<xmlUsedCharType>* attribute = doc.allocate_attribute();
    node->name(name);
    attribute->name("value");
    attribute->value(value);
    node->append_attribute(attribute);
    nodes->append_node(node);
}

bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, bool logs) {
    std::ofstream file;
    file.open(filePath.c_str(), std::ios_base::trunc);
    if(!file.is_open()) {
        if(!logs) return NULL;
        log(error_log, std::string("The file ")+filePath.c_str()+" couldn't be found.");
        return NULL;
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
        if(strcmp(node->name(), "matrix") == 0) {
            XMLValueArray<btScalar> matrixData;
            matrixData.readString(node->value(), "%f");
            btTransform mat;
            mat.setBasis(btMatrix3x3(matrixData.data[0], matrixData.data[1], matrixData.data[2],
                                     matrixData.data[4], matrixData.data[5], matrixData.data[6],
                                     matrixData.data[8], matrixData.data[9], matrixData.data[10]));
            mat.setOrigin(btVector3(matrixData.data[3], matrixData.data[7], matrixData.data[11]));
            transform = transform * mat;
        }else if(strcmp(node->name(), "translate") == 0) {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            mat.setOrigin(btVector3(vectorData.data[0], vectorData.data[1], vectorData.data[2]));
            transform = transform * mat;
        }else if(strcmp(node->name(), "rotate") == 0) {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            mat.setRotation(btQuaternion(vectorData.data[0], vectorData.data[1], vectorData.data[2], vectorData.data[3]/180.0*M_PI));
            transform = transform * mat;
        }else if(strcmp(node->name(), "scale") == 0) {
            XMLValueArray<float> vectorData;
            vectorData.readString(node->value(), "%f");
            btTransform mat;
            mat.setIdentity();
            //mat.setBasis(mat.getBasis().scaled(btVector3(vectorData.data[0], vectorData.data[1], vectorData.data[2])));
            mat.setBasis(btMatrix3x3(btVector3(vectorData.data[0], 0, 0),
                                     btVector3(0, vectorData.data[1], 0),
                                     btVector3(0, 0, vectorData.data[2])));
            transform = transform * mat;
        }
        node = node->next_sibling();
    }
    return transform;
}