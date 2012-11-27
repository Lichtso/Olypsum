//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Object.h"

#ifndef Model_h
#define Model_h

class ModelObject;

//! A Mesh of a Model
class Mesh {
    public:
    GLuint vbo, ibo; //!< VertexBufferObject and IndexBufferObject used for OpenGL
    unsigned int elementsCount; //!< The count of triangles * 3 or the count of indecies in this Model
    bool transparent; //!< Is this Model transparent
    int postions, //!< The count of postions in this Model
        texcoords, //!< The count of texcoords in this Model
        normals, //!< The count of normals in this Model
        weightJoints; //!< The count of weightJoints in this Model
    std::shared_ptr<Texture> diffuse, //!< The diffuse texture
                             effectMap, //!< The specular texture (optional)
                             heightMap; //!< The highmap (optional)
    Mesh();
    ~Mesh();
    /*! Used by the engine to render this Mesh
     @param object The parent ModelObject which is used to render the Model of this Mesh
     */
    void draw(ModelObject* object);
};

//! A transparent Mesh of a Model
/*!
 A transparent Mesh of a Model which is accumulated by the engine to be rendered at the end of the graphics frame
 
 @see FBO::renderTransparentInDeferredBuffers()
 @warning Don't use it directly
 */
struct AccumulatedMesh {
    ModelObject* object;
    Mesh* mesh;
};

//! A Bone of a Skeleton
/*!
 This class is used internally to hold the hierarchical structure of a Skeleton
 
 @warning Don't use it directly
 */
struct Bone {
    btTransform relativeMat, absoluteMat, relativeInv, absoluteInv;
    unsigned int jointIndex; //!< The index of this Bone in the matrix array used for OpenGL
    std::string name; //!< A human readable name of this Bone found in the COLLADA-file
    std::vector<Bone*> children; //!< A array of all children of this Bone
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