//
//  Links.h
//  Olypsum
//
//  Created by Alexander Meißner on 06.01.13.
//
//

#include "Object.h"

#ifndef Links_h
#define Links_h

//! A structure that contains all the information needed to construct or save a BaseLink
class LinkInitializer {
    public:
    BaseObject* object[2]; //!< The objects to be linked together
    int index[2];  //!< The index of each object (only needed to save the link)
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
    BaseLink() { };
    virtual ~BaseLink() { };
    public:
    BaseObject *a, *b; //!< The linkes BaseObjects
    /*! Constructs a new LinkObject
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     */
    BaseLink(LinkInitializer& initializer);
    //! Initialize from rapidxml::xml_node
    BaseLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Is called by a parent BaseObject to its children to prepare the next graphics frame
    virtual void gameTick() { };
    /*! Gets the other BaseObject
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @return If a was passed then b and if b was passed then a
     */
    BaseObject* getOther(BaseObject* object) {
        return (object == b) ? a : b;
    }
    //! Used to remove a LinkObject correctly
    virtual void removeClean();
    /*! Used to delete a LinkObject
     @param object One of the two BaseObject passed in the constructor. Either a or b
     */
    virtual void removeFast(BaseObject* object);
    //! Reads the LinkInitializer from rapidxml::xml_node
    static LinkInitializer readInitializer(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Initialize from LinkInitializer. Returns success
    bool init(LinkInitializer& initializer);
    //! Writes its self to rapidxml::xml_node and returns it
    virtual rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a btTypedConstraint
class PhysicLink : public BaseLink {
    ~PhysicLink();
    public:
    btTypedConstraint* constraint;
    void gameTick();
    /*! Constructs a new PhysicLink
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     @param constraint The bullet physics constraint to be attached
     */
    PhysicLink(LinkInitializer& initializer, btTypedConstraint* constraint);
    PhysicLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    void removeClean();
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a parent child relationship
class TransformLink : public BaseLink {
    public:
    btTransform transform; //!< Applied from parent to child
    void gameTick();
    /*! Constructs a new HierarchicalLink
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     */
    TransformLink(LinkInitializer& initializer);
    TransformLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    /*! Used to remove a HierarchicalLink correctly.
     This will call BaseObject::remove() on the child if and only if the parameter a is the parent.
     @param a One of the two BaseObject passed in the constructor. Either parent or child
     @param iteratorInA the iterator of this LinkObject in the BaseObject::links map of the first parameter
     
     @warning This method calls remove() on the child object but only if it is called from the parent
     */
    void removeClean();
    void removeFast(BaseObject* a);
    bool init(LinkInitializer& initializer);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

#endif