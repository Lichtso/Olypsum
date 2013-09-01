//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Model_h
#define Model_h

#include "CamObject.h"

class RigidObject;

//! A Mesh of a Model
class Mesh {
    public:
    VertexArrayObject vao; //!< VertexArrayObject used for OpenGL
    //! Surface material of a Mesh used for OpenGL
    struct Material {
        float reflectivity = 0.0, //!< Reflectivity <0.0 : Plane Mirror, ==0.0 : No Mirror, >0.0 : Environment Mirror
              refraction = 0.0; //!< Refraction <=0.0 Non transparent, 0.0-1.0 : transparent, >1.0 : water
        FileResourcePtr<Texture> diffuse, //!< The diffuse texture
                                 effectMap, //!< The specular texture (optional)
                                 heightMap; //!< The highmap (optional)
    } material;
    /*! Used by the engine to render this Mesh
     @param object The parent RigidObject which is used to render the Model of this Mesh
     */
    void draw(RigidObject* object);
};

//! A Skeleton of a Model
/*!
 This class is used internally to hold the hierarchical structure of a Skeleton found in the COLLADA-file
 
 @warning Don't use it directly
 */
class Skeleton {
    public:
    Bone* rootBone;
    std::map<std::string, Bone*> bones;
};

//! A Model used for graphics
/*!
 A FilePackageResource which can be loaded form a COLLADA-file and can be rendered on screen.
 */
class Model : public FileResource {
    public:
    std::vector<Mesh*> meshes; //!< All meshes found in the COLLADA-file
    Skeleton* skeleton; //!< A skeleton if there is one in the COLLADA-file
    Model();
    ~Model();
    /*! Used by the engine to load a COLLADA-file
     @param filePackage The parent FilePackage
     @param name The file name
     @see FilePackage::getResource()
     */
    FileResourcePtr<FileResource> load(FilePackage* filePackage, const std::string& name);
    /*! Used by the engine to render the entire model
     @param object The parent ModelObject which is used to render this Model
     */
    void draw(RigidObject* object);
};

#endif