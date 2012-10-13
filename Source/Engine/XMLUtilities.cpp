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