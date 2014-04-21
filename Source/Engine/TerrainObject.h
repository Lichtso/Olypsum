//
//  TerrainObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef HeightMap_h
#define HeightMap_h

#include "VisualObject.h"

//! A MatterObject used to describe a terrain
/*!
 Used to make a terrain.
 This class manages its own collision shape and drawing
 */
class TerrainObject : public MatterObject {
    VertexArrayObject vao; //!< VertexArrayObject used for OpenGL
    //! Internally used by updateModel()
    btVector3 getVertexAt(float* vertices, unsigned int x, unsigned int y);
    //! Updates the OpenGL buffers with the data from heights
    public:
    btVector3 textureScale; //!< Used to scale the diffuse and specular texture in OpenGL
    FileResourcePtr<Texture> diffuse, //!< The diffuse texture
                             effectMap; //!< The specular texture
    unsigned int width, //!< How many samples are there in X direction
                 length,  //!< How many samples are there in Z direction
                 bitDepth; //!< How many bits to use per Y-sample to store the terrain
    float* heights; //!< Pointer to the Y-samples
    TerrainObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~TerrainObject();
    void draw();
    void updateModel();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif