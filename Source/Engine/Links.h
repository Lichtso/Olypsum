//
//  Links.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//  Copyright (c) 2014 Gamefortec. All rights reserved.
//

#ifndef Links_h
#define Links_h

#include "Object.h"

//! A structure that contains all the information needed to construct or save a BaseLink
class LinkInitializer {
    public:
    BaseObject* object[2]; //!< The objects to be linked together
    int index[2];  //!< The index of each object (only needed to save the link)
    //!< Swaps the first and the second objects and their indecies
    void swap() {
        std::swap(object[0], object[1]);
        std::swap(index[0], index[1]);
    }
};

//! Links basic class
/*!
 This is the basic class for all LinkObjects.
 */
class BaseLink : public BaseClass {
    protected:
    virtual ~BaseLink() { };
    public:
    BaseObject *a, *b; //!< The linkes BaseObjects
    void removeClean();
    void removeFast();
    //! Is called by a parent BaseObject to its children to prepare the next graphics frame
    virtual void gameTick() { };
    /*! Gets the other BaseObject
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @return If a was passed then b and if b was passed then a
     */
    BaseObject* getOther(BaseObject* object) {
        return (object == b) ? a : b;
    }
    //! Returns true if a connection between the objects is allowed
    virtual bool isAttachingIsValid(LinkInitializer& initializer);
    /*! Initialize from LinkInitializer. Returns success
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     */
    bool init(LinkInitializer& initializer);
    //! Initialize from rapidxml::xml_node. Returns success
    virtual bool init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Writes its self to rapidxml::xml_node and returns it
    virtual rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink for skeletal animation
class BoneLink : public BaseLink {
    public:
    Bone* bone; //!< Bone which applies the transformation
    //! Gets a bone by the name in the model of object a
    Bone* getBoneByName(const std::string& name);
    void gameTick();
    bool isAttachingIsValid(LinkInitializer& initializer);
    bool init(LinkInitializer& initializer, Bone* bone);
    bool init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a parent manipulating the transformation of a child
class TransformLink : public BaseLink {
    public:
    std::vector<btTransform> transformations; //!< Transformation stack applied from parent to child
    void gameTick();
    bool isAttachingIsValid(LinkInitializer& initializer);
    bool init(LinkInitializer& initializer, const std::vector<btTransform>& transformations);
    bool init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a btTypedConstraint
class PhysicLink : public BaseLink {
    ~PhysicLink();
    public:
    btTypedConstraint* constraint;
    //! Enables/Disables collisions between the linked objects
    void setCollisionDisabled(bool collisionDisabled);
    //! Returns if collisions between the linked objects are disabled
    bool isCollisionDisabled();
    void gameTick();
    void removeClean();
    bool init(LinkInitializer& initializer, btTypedConstraint* constraint);
    bool init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

#endif