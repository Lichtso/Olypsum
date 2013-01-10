//
//  HeightfieldTerrain.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//
//

#include "DisplayObject.h"

#ifndef HeightMap_h
#define HeightMap_h

//! A GraphicObject used to describe a Terrain
/*!
 Used to make a terrain.
 This class manages its own collision shape and drawing
 */
class HeightfieldTerrain : public GraphicObject {
    btVector3 textureScale; //!< Used to scale the diffuse and specular texture in OpenGL
    std::shared_ptr<Texture> diffuse, //!< The diffuse texture
                             effectMap; //!< The specular texture
    GLuint vbo, ibo; //!< VertexBufferObject and IndexBufferObject used for OpenGL
    unsigned int width, length;
    float* heights;
    //! Internally used by updateModel()
    btVector3 getVertexAt(float* vertices, unsigned int x, unsigned int y);
    //! Updates the OpenGL buffers with the data from heights
    void updateModel();
    public:
    HeightfieldTerrain(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~HeightfieldTerrain();
    void draw();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif