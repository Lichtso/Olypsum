//
//  BasicObjects.h
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "FBO.h"

#ifndef Object_h
#define Object_h

class BaseLink;

//! Objects basic class
/*!
 This is the basic class for all Objects.
 
 @warning Don't use it directly
 */
class BaseObject {
    protected:
    BaseObject() { };
    public:
    virtual ~BaseObject();
    std::map<std::string, BaseLink*> links; //!< A map of LinkObject and names to connect BaseObject to others
    /*! Used to update the transfomation of this object
     @param transformation The new transformation
     */
    virtual void setTransformation(const btTransform& transformation) = 0;
    /*! Used to get the transfomation of this object
     @return The current transformation
     */
    virtual btTransform getTransformation() = 0;
    //! Called by the engine to prepare the next graphics frame
    virtual void gameTick() { };
    //! Used to remove a BaseObject from all lists correctly
    virtual void remove() {
        delete this;
    };
};

//! BaseObject without physics-body, only transformation
/*!
 This is the basic class for all Objects without a physics-body.
 
 @warning Don't use it directly
 */
class SimpleObject : public BaseObject {
    protected:
    btTransform transformation; //!< The transformation in the world
    SimpleObject() { }
    public:
    virtual void setTransformation(const btTransform& t) {
        transformation = t;
    }
    virtual btTransform getTransformation() {
        return transformation;
    }
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
    PhysicObject(btCollisionShape* shape);
    public:
    ~PhysicObject();
    virtual void setTransformation(const btTransform& transformation) {
        body->setWorldTransform(transformation);
    }
    virtual btTransform getTransformation() {
        return body->getWorldTransform();
    }
    //! Is called by the engine to prepare the next physics frame
    virtual void physicsTick() { };
    //! Is called by the engine if a collision to another PhysicObject has been detected
    virtual void handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) { };
};


//! Objects basic class
/*!
 This is the basic class for all LinkObjects.
 */
class BaseLink {
    BaseObject* fusion; //!< A pointer which combines the pointers of BaseObject a and b via xor
    public:
    /*! Constructs a new LinkObject
     @param a A BaseObject to be connected
     @param b The other BaseObject to be connected
     @param nameInA The name this LinkObject shall get in the BaseObject::links map of a
     @param nameInB The name this LinkObject shall get in the BaseObject::links map of b
     */
    BaseLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB) {
        fusion = reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(b));
        a->links[nameInA] = this;
        b->links[nameInB] = this;
    }
    virtual ~BaseLink() {
        //if(fusion) log(error_log, "LinkObject being freed is not clear");
    }
    //! Is called by a parent BaseObject to its children to prepare the next graphics frame
    virtual void gameTickFrom(BaseObject* a) { };
    /*! Gets the other BaseObject
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @return If a was passed then b and if b was passed then a
     */
    BaseObject* getOther(BaseObject* a) {
        return reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(fusion));
    };
    /*! Used to remove a LinkObject correctly
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @param nameInA the name of this LinkObject in the BaseObject::links map of the first parameter
     */
    void remove(BaseObject* a, const std::string& nameInA) {
        a->links.erase(nameInA);
        BaseObject* b = getOther(a);
        for(auto iterator : b->links)
            if(iterator.second == this) {
                b->links.erase(iterator.first);
                break;
            }
        //fusion = NULL;
        delete this;
    };
};


class PhysicLink : public BaseLink {
    public:
    btTypedConstraint* constraint;
    /*! Constructs a new PhysicLink
     @param a A BaseObject to be connected
     @param b The other BaseObject to be connected
     @param nameInA The name this LinkObject shall get in the BaseObject::links map of a
     @param nameInB The name this LinkObject shall get in the BaseObject::links map of b
     @param constraint The bullet physics constraint to be attached
     */
    PhysicLink(BaseObject* a, BaseObject* b, std::string nameInA, std::string nameInB, btTypedConstraint* constraint);
    ~PhysicLink();
};


#endif