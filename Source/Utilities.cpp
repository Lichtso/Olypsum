//
//  Utilities.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 08.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "Utilities.h"

bool parseXmlFile(rapidxml::xml_document<xmlUsedCharType>& doc, const char* filePath) {
    FILE* fp = fopen(filePath, "r");
    if(!fp) {
        printf("The file %s couldn't be found.", filePath);
        return false;
    }
    
    fseek(fp, 0, SEEK_END);
	long dataSize = ftell(fp);
	rewind(fp);
	char data[dataSize+1];
	fread(data, 1, dataSize, fp);
    fclose(fp);
    data[dataSize] = 0;
    
    doc.parse<0>(data);
    return true;
}