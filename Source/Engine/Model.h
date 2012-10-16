//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "BasicObjects.h"

#ifndef Model_h
#define Model_h

struct Bone {
    btTransform staticMat;
    unsigned int jointIndex;
    std::string name;
    std::vector<Bone*> children;
};

struct Skeleton {
    Bone* rootBone;
    std::map<std::string, Bone*> bones;
};

class SkeletonPose {
    void calculateBonePose(Bone* bone, Bone* parentBone);
    public:
    Skeleton* skeleton;
    btTransform* mats;
    std::map<std::string, btTransform> bonePoses;
    SkeletonPose(Skeleton* skeleton);
    ~SkeletonPose();
    void calculate();
};

class Mesh {
    public:
    GLuint vbo, ibo;
    unsigned int elementsCount;
    bool transparent;
    int postions, texcoords, normals, weightJoints;
    std::shared_ptr<Texture> diffuse, effectMap, heightMap;
    Mesh();
    ~Mesh();
    void draw(ModelObject* object);
};

struct TransparentMesh {
    ModelObject* object;
    Mesh* mesh;
};

class Model : public FilePackageResource {
    public:
    std::vector<Mesh*> meshes;
    Skeleton* skeleton;
    Model();
    ~Model();
    std::shared_ptr<FilePackageResource> load(FilePackage* filePackageB, const std::string& name);
    void prepareShaderProgram(Mesh* mesh);
    void draw(ModelObject* object);
};

#endif