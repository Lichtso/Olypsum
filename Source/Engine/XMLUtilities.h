//
//  XMLUtilities.h
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "rapidxml.hpp"
#include "rapidxml_print.hpp"
#include "Profiler.h"

#ifndef XMLUtilities_h
#define XMLUtilities_h

#define xmlUsedCharType char

/*! Parses the content of a file as XML
 @param doc A reference to the rapidxml::xml_document which will be used to store the content
 @param filePath The file to be parsed
 @param logs Enables console logs if a error occurs
 @return A string with the raw content of the file to be deleted by the receiver
 */
std::unique_ptr<char[]> readXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& filePath, bool logs);

/*! Adds a rapidxml::xml_node with a "value" attribute to a rapidxml::xml_document
 @param doc A reference to the rapidxml::xml_document
 @param parent The parent node to be used to attach the new child
 @param name The name of the new child node
 @param name The value of the "value" attribute
 */
void addXMLNode(rapidxml::xml_document<xmlUsedCharType>& doc, rapidxml::xml_node<xmlUsedCharType>* parent, const char* name, const char* value);

/*! Writes a rapidxml::xml_document to a file
 @param doc A reference to the rapidxml::xml_document
 @param filePath The file to be written
 @param logs Enables console logs if a error occurs
 @return Success
 */
bool writeXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const std::string& filePath, bool logs);

//! A utility used to parse XML and string value arrays
template <class T>
class XMLValueArray {
    public:
    T* data; //!< The content of this array
    unsigned int count, //!< The size of this array
                 stride; //!< The stried used to interleave with other XMLValueArrays (ignored in parsing)
    XMLValueArray() {
        data = NULL;
        count = 0;
    }
    ~XMLValueArray() {
        if(data)
            delete [] data;
    }
    //! Deletes the content
    void clear() {
        if(!data) return;
        delete [] data;
        data = NULL;
    }
    //! Convertes data to a btVector3 and returns it
    btVector3 getVector3() {
        return btVector3(data[0], data[1], data[2]);
    }
    //! Convertes data to a btQuaternion and returns it
    btQuaternion getQuaternion() {
        return btQuaternion(data[0], data[1], data[2], data[3]);
    }
    /*! Parses a string of values separated by ' '
     @param dataStr The string to be parsed
     @param readType The format like in scanf()
     */
    void readString(char* dataStr, const char* readType) {
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
    }
    /*! Parses a rapidxml::xml_node of a COLLADA file for example
     @param node The rapidxml::xml_node to be parsed
     @param readType The format like in scanf()
     @return Success
     */
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
    /*! Parses a rapidxml::xml_node of a COLLADA file for example
     @param node The rapidxml::xml_node to be parsed
     @param expectedCount Set another count to be expected as size of the array (used for error logs)
     @param readType The format like in scanf()
     @return Success
     */
    bool readString(rapidxml::xml_node<xmlUsedCharType>* node, unsigned int expectedCount, const char* readType) {
        if(!node) {
            log(error_log, "XMLValueArray readString(): Node is undefined.");
            return false;
        }
        readString(node->value(), readType);
        if(count != expectedCount) {
            log(error_log, "XMLValueArray readString(): Expected count doesn't match the elements count.");
            return false;
        }
        return true;
    }
};

//! Parses a rapidxml::xml_node and convertes its content into a btTransform
btTransform readTransformationXML(rapidxml::xml_node<xmlUsedCharType>* node);

//! Writes a btTransform and returns it as rapidxml::xml_node
rapidxml::xml_node<xmlUsedCharType>* writeTransformationXML(rapidxml::xml_document<xmlUsedCharType> &doc, btTransform& transform);

#endif