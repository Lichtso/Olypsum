//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "Object.h"

#ifndef Model_h
#define Model_h

class ModelObject;

//! A Mesh of a Model
class Mesh {
    public:
    VertexArrayObject vao; //!< VertexArrayObject used for OpenGL
    //! Surface material of a Mesh used for OpenGL
    struct Material {
        bool transparent; //!< Is this Model transparent
        btVector3 reflectorNormal; //!< (0, 0, 0) if this mesh does not reflect
        float reflectorDistance; //!< Distance of the reflection plane relative to the origin of the model
        std::shared_ptr<Texture> diffuse, //!< The diffuse texture
                                 effectMap, //!< The specular texture (optional)
                                 heightMap; //!< The highmap (optional)
    } material;
    Mesh();
    /*! Used by the engine to render this Mesh
     @param object The parent ModelObject which is used to render the Model of this Mesh
     */
    void draw(ModelObject* object);
};

//! A Skeleton of a Model
/*!
 This class is used internally to hold the hierarchical structure of a Skeleton found in the COLLADA-file
 
 @warning Don't use it directly
 */
struct Skeleton {
    Bone* rootBone;
    std::map<std::string, Bone*> bones;
};

//! A Model used for graphics
/*!
 A FilePackageResource which can be loaded form a COLLADA-file and can be rendered on screen.
 */
class Model : public FilePackageResource {
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
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackage, const std::string& name);
    /*! Used by the engine to render the entire model
     @param object The parent ModelObject which is used to render this Model
     */
    void draw(ModelObject* object);
};

#endif