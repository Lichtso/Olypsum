//
//  Model.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.04.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#import "Audio.h"

#ifndef Model_h
#define Model_h

enum ObjectInstanceType {
    ObjectInstance_Base = 0,
    ObjectInstance_Normal = 1,
    ObjectInstance_Animated = 2,
    ObjectInstance_NPC = 3,
    ObjectInstance_Zone = 4,
    ObjectInstance_Water = 5,
    ObjectInstance_ModelOnly = 6
};

class ObjectBase {
    public:
    ObjectInstanceType type;
    virtual void calculate();
    virtual float getDiscardDensity();
    virtual Matrix4 getTransformation();
    virtual void draw();
};

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

class SkeletonPose {
    void calculateBonePose(Bone* bone, Bone* parentBone);
    public:
    Skeleton* skeleton;
    Matrix4* mats;
    std::map<std::string, Matrix4> bonePoses;
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
    Texture *diffuse, *effectMap, *heightMap;
    Mesh();
    ~Mesh();
    void draw(ObjectBase* object);
};

class Model {
    public:
    unsigned int useCounter;
    std::vector<Mesh*> meshes;
    Skeleton* skeleton;
    Model();
    ~Model();
    bool loadCollada(FilePackage* filePackage, const char* filePath);
    void draw(ObjectBase* object);
};

#endif