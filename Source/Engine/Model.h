//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#import "GUIImage.h"

#ifndef Model_h
#define Model_h

struct Bone {
    Matrix4 staticMat;
    unsigned int jointIndex;
    std::string name;
    std::vector<Bone*> children;
};

struct Skeleton {
    Bone* rootBone;
    std::map<std::string, Bone*> bones;
};

class Mesh {
    public:
    GLuint vbo, ibo;
    unsigned int elementsCount;
    int postions, texcoords, normals, tangents, bitangents, weightJoints;
    Texture *diffuse, *normalMap, *effectMap;
    Mesh();
    ~Mesh();
    void draw();
};

class FilePackage;

struct BonePose {
    Matrix4 poseMat, dynamicMat;
};

class SkeletonPose {
    public:
    std::map<std::string, BonePose*> bonePoses;
    SkeletonPose(Skeleton* skeleton);
    ~SkeletonPose();
    void calculateBonePose(Bone* bone, Bone* parentBone);
    void calculateDisplayMatrix(Bone* bone, Bone* parentBone, Matrix4* mats);
};

class Model {
    public:
    unsigned int useCounter;
    std::vector<Mesh*> meshes;
    Skeleton* skeleton;
    Model();
    ~Model();
    bool loadCollada(FilePackage* filePackage, const char* filePath);
    void draw();
    void draw(SkeletonPose* skeletonPose);
};

#endif