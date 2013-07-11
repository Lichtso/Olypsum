//
//  TerrainObject.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef HeightMap_h
#define HeightMap_h

#include "DisplayObject.h"

//! A GraphicObject used to describe a terrain
/*!
 Used to make a terrain.
 This class manages its own collision shape and drawing
 */
class TerrainObject : public GraphicObject {
    VertexArrayObject vao; //!< VertexArrayObject used for OpenGL
    //! Internally used by updateModel()
    btVector3 getVertexAt(float* vertices, unsigned int x, unsigned int y);
    //! Updates the OpenGL buffers with the data from heights
    public:
    btVector3 textureScale; //!< Used to scale the diffuse and specular texture in OpenGL
    FileResourcePtr<Texture> diffuse, //!< The diffuse texture
    effectMap; //!< The specular texture
    unsigned int width, length, bitDepth;
    float* heights;
    TerrainObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~TerrainObject();
    void newScriptInstance();
    void draw();
    void updateModel();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif