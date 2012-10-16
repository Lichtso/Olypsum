//
//  XMLUtilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import <fstream>
#import "rapidxml.hpp"
#import "rapidxml_print.hpp"
#import "LinearMaths.h"

#ifndef XMLUtilities_h
#define XMLUtilities_h

#define xmlUsedCharType char

std::unique_ptr<char[]> readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, bool logs);

void addXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>* nodes, const char* name, const char* value);

bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, std::string filePath, bool logs);

template <class T>
class XMLValueArray {
    public:
    T* data;
    unsigned int count, stride;
    XMLValueArray() {
        data = NULL;
        count = 0;
    }
    ~XMLValueArray() {
        if(data)
            delete [] data;
    }
    void clear() {
        if(!data) return;
        delete [] data;
        data = NULL;
    }
    bool readString(char* dataStr, const char* readType) {
        char* dataEnd = dataStr+strlen(dataStr);
        if(!data) {
            count = 1;
            for(char* pos = dataStr; pos < dataEnd-1; pos ++)
                if(*pos == ' ') count ++;
            data = new T[count];
        }
        count = 0;
        char *lastPos = dataStr;
        while(dataStr && lastPos < dataEnd) {
            dataStr = strchr(lastPos, ' ');
            if(dataStr) *dataStr = 0;
            sscanf(lastPos, readType, &data[count ++]);
            lastPos = dataStr+1;
        }
        return true;
    }
    bool readString(rapidxml::xml_node<xmlUsedCharType>* node, const char* readType) {
        if(!node) {
            log(error_log, "XMLValueArray readString(): Node is undefined.");
            return false;
        }
        rapidxml::xml_attribute<xmlUsedCharType>* countAttribute = node->first_attribute("count");
        if(!countAttribute) {
            log(error_log, "XMLValueArray readString(): Couldn't find count attribute.");
            return false;
        }
        unsigned int expectedCount;
        sscanf(countAttribute->value(), "%d", &expectedCount);
        readString(node->value(), readType);
        if(count != expectedCount) {
            log(error_log, "XMLValueArray readString(): Count attribute doesn't match the elements count.");
            return false;
        }
        return true;
    }
    bool readString(rapidxml::xml_node<xmlUsedCharType>* node, unsigned int expectedCount, const char* readType) {
        if(!node) {
            log(error_log, "XMLValueArray readString(): Node is undefined.");
            return false;
        }
        readString(node->value(), readType);
        if(count != expectedCount) {
            log(error_log, "XMLValueArray readString(): Count attribute doesn't match the elements count.");
            return false;
        }
        return true;
    }
};

btTransform readTransformationXML(rapidxml::xml_node<xmlUsedCharType>* node);

#endif