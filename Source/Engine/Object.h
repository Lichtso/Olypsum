//
//  Object.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#ifndef Object_h
#define Object_h

#include "FBO.h"

class ScriptFile;
class Skeleton;
class BaseLink;
class LevelLoader;
class LevelSaver;

//! Objects and Links basic class
/*!
 This is the basic class for all Objects and Links.
 
 @warning Don't use it directly
 */
class BaseClass {
    protected:
    BaseClass() :scriptFile(NULL) { }
    virtual ~BaseClass();
    public:
    ScriptFile* scriptFile; //!< The script file to be called on events
    v8::Persistent<v8::Object> scriptInstance; //!< The script representation
    //! Used to remove a BaseClass correctly
    virtual void removeClean() { };
    //! Used to delete a LinkObject quickly
    virtual void removeFast() { };
    //! Initialize the "Script" rapidxml::xml_node
    void initScriptNode(FilePackage* filePackage, rapidxml::xml_node<xmlUsedCharType>* node);
};

//! Objects basic class
/*!
 This is the basic class for all Objects.
 
 @warning Don't use it directly
 */
class BaseObject : public BaseClass {
    public:
    std::set<BaseLink*> links; //!< A map of LinkObject and names to connect BaseObject to others
    void removeClean();
    void removeFast();
    /*! Used to update the transfomation of this object
     @param transformation The new transformation
     */
    virtual void setTransformation(const btTransform& transformation) = 0;
    /*! Used to get the transfomation of this object
     @return The current transformation
     */
    virtual btTransform getTransformation() = 0;
    /*! Called by the engine to prepare the next graphics frame
     @return Returns false when it deleted it's self in this tick
     */
    virtual bool gameTick();
    //! Initialize from rapidxml::xml_node
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Reads the transformation from a rapidxml::xml_node
    static btTransform readTransformtion(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Writes its self to rapidxml::xml_node and returns it
    virtual rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
    /*! Returns the iterator of a BaseLink
     @param linked BaseObject* to search for
     @return BaseLink between this and linked or links.end()
    */
    std::set<BaseLink*>::iterator findLinkTo(BaseObject* linked);
};

//! BaseObject without physics-body, only transformation
/*!
 This is the basic class for all Objects without a physics-body.
 
 @warning Don't use it directly
 */
class SimpleObject : public BaseObject {
    protected:
    btTransform transformation; //!< The transformation in the world
    SimpleObject() { };
    public:
    SimpleObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    void setTransformation(const btTransform& t) {
        transformation = t;
    }
    btTransform getTransformation() {
        return transformation;
    }
};

//! A Bone of a Skeleton
/*!
 This class is used internally to hold the hierarchical structure of a Skeleton
 
 @warning Don't use it directly
 */
struct Bone {
    btTransform relativeMat, absoluteInv;
    unsigned int jointIndex; //!< The index of this Bone in the matrix array used for OpenGL
    std::string name; //!< A human readable name of this Bone found in the COLLADA-file
    std::vector<Bone*> children; //!< A array of all children of this Bone
};

//! BaseObject with physics-body
/*!
 This is the basic class for all Objects with a physics-body.
 
 @warning Don't use it directly
 */
class PhysicObject : public BaseObject {
    protected:
    btCollisionObject* body; //!< The physics-body
    PhysicObject() :body(NULL) { }
    public:
    PhysicObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    void removeFast();
    virtual void setTransformation(const btTransform& transformation) {
        body->setWorldTransform(transformation);
    }
    virtual btTransform getTransformation() {
        return body->getWorldTransform();
    }
    //! Activates all PhysicObjects that are touching this PhysicObject
    void updateTouchingObjects();
    //! Is called by the engine if a collision to another PhysicObject has been detected
    virtual void handleCollision(btPersistentManifold* contactManifold, PhysicObject* b);
    //! Getter method for the physics body, child classes will upcast their bodies
    btCollisionObject* getBody() {
        return body;
    }
    //! Reads the collision shape from a rapidxml::xml_node named "PhysicsBody"
    btCollisionShape* readCollisionShape(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};

#endif