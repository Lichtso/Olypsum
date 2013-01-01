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

class Skeleton;
class BaseLink;
class LevelLoader;
class LevelSaver;

//! Objects basic class
/*!
 This is the basic class for all Objects.
 
 @warning Don't use it directly
 */
class BaseObject {
    protected:
    BaseObject() { };
    public:
    virtual ~BaseObject() { };
    std::map<std::string, BaseLink*> links; //!< A map of LinkObject and names to connect BaseObject to others
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
    //! Used to remove a BaseObject from all lists correctly
    virtual void remove();
    //! Initialize from rapidxml::xml_node
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Reads the transformation from a rapidxml::xml_node
    static btTransform readTransformtion(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Writes its self to rapidxml::xml_node and returns it
    virtual rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
    //! Returns the object at the end of the path (seperated by '/')
    BaseObject* findObjectByPath(std::string path);
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
    SimpleObject(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader) {
        BaseObject::init(node, levelLoader);
    }
    public:
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

//! SimpleObject used for the bones in ModelObject
class BoneObject : public SimpleObject {
    public:
    Bone* bone;
    BoneObject(Bone* boneB) : bone(boneB) { }
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
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
    //! Getter method for the physics body, child classes will upcast their bodies
    btCollisionObject* getBody() {
        return body;
    }
    //! Initialize from rapidxml::xml_node as btCollisionObject
    void init(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Reads the collision shape from a rapidxml::xml_node named "PhysicsBody"
    btCollisionShape* readCollisionShape(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LevelSaver* levelSaver);
};



//! A structure that contains all the information needed to construct or save a BaseLink
class LinkInitializer {
    public:
    BaseObject* object[2]; //!< The objects to be linked together
    std::string name[2]; //!< The name of the link in each object
    int index[2];  //!< The index of each object (only needed to save the link)
    void swap() {
        std::swap(object[0], object[1]);
        std::swap(name[0], name[1]);
    }
};

//! Links basic class
/*!
 This is the basic class for all LinkObjects.
 */
class BaseLink {
    BaseObject* fusion; //!< A pointer which combines the pointers of BaseObject a and b via xor
    protected:
    BaseLink() { };
    virtual ~BaseLink() { }
    public:
    /*! Constructs a new LinkObject
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     */
    BaseLink(LinkInitializer& initializer);
    //! Initialize from rapidxml::xml_node
    BaseLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Is called by a parent BaseObject to its children to prepare the next graphics frame
    virtual void gameTickFrom(BaseObject* parent) { };
    /*! Gets the other BaseObject
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @return If a was passed then b and if b was passed then a
     */
    BaseObject* getOther(BaseObject* a) {
        return reinterpret_cast<BaseObject*>(reinterpret_cast<unsigned long>(a) ^ reinterpret_cast<unsigned long>(fusion));
    }
    /*! Used to remove a LinkObject correctly
     @param a One of the two BaseObject passed in the constructor. Either a or b
     @param iteratorInA the iterator of this LinkObject in the BaseObject::links map of the first parameter
     */
    virtual void remove(BaseObject* a, const std::map<std::string, BaseLink*>::iterator& iteratorInA);
    //! Reads the LinkInitializer from rapidxml::xml_node
    static LinkInitializer readInitializer(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    //! Initialize from LinkInitializer
    void init(LinkInitializer& initializer);
    //! Writes its self to rapidxml::xml_node and returns it
    virtual rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a btTypedConstraint
class PhysicLink : public BaseLink {
    protected:
    ~PhysicLink();
    public:
    btTypedConstraint* constraint;
    /*! Constructs a new PhysicLink
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     @param constraint The bullet physics constraint to be attached
     */
    PhysicLink(LinkInitializer& initializer, btTypedConstraint* constraint);
    PhysicLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

//! A BaseLink with a parent child relationship
class TransformLink : public BaseLink {
    public:
    //! This is the base for all entries of a TransformLink
    /*!
     @warning Don't use it directly
     */
    class BaseEntry {
        protected:
        BaseEntry() { }
        public:
        /*! This calculates a matrix which is multiplied with the others in TransformLink::transforms
         @return A btTransform to be used to transform the child object of this link
         */
        virtual btTransform gameTick() {
            return btTransform::getIdentity();
        }
    };
    //! A TransformLink::BaseEntry which transforms with a matrix
    class TransformEntry : public BaseEntry {
        public:
        btTransform matrix; //!< The direct transformation matrix
        TransformEntry(btTransform matrixB) : matrix(matrixB) { }
        btTransform gameTick() {
            return matrix;
        }
    };
    //! A TransformLink::BaseEntry which transforms with animation frames
    class AnimationEntry : public BaseEntry {
        public:
        //! This is key frame used for animation
        class Frame {
            public:
            btQuaternion rotation;
            btVector3 position;
            float accBegin, //!< The acceleration at t = 0
                  accEnd, //!< The acceleration at t = duration
                  duration; //!< The duration of this frame in seconds
            Frame(float accBegin, float accEnd, float duration, btQuaternion rotation, btVector3 position);
            /*! Internaly used by the engine to get the transformation if only one frame is available
             @return The transformation at t = 0
             */
            btTransform getTransform();
            /*! Internaly used by the engine to interpolate with a cubic hermite spline
             @param next The next TransformFrame to interpolate to
             @param t The time in seconds
             @return The interpolated result
             */
            btTransform interpolateTo(Frame* next, float t);
        };
        
        float animationTime; //!< How many seconds is the current frame old
        std::vector<Frame*> frames; //!< The frames in this animation
        ~AnimationEntry();
        btTransform gameTick();
    };
    std::vector<BaseEntry*> transforms; //!< A array with all the transformations to be applied
    virtual void gameTickFrom(BaseObject* parent);
    /*! Constructs a new HierarchicalLink
     @param initializer A LinkInitializer which contains the objects and names to be linked together
     */
    TransformLink(LinkInitializer& initializer);
    TransformLink(rapidxml::xml_node<xmlUsedCharType>* node, LevelLoader* levelLoader);
    ~TransformLink();
    /*! Used to remove a HierarchicalLink correctly.
     This will call BaseObject::remove() on the child if and only if the parameter a is the parent.
     @param a One of the two BaseObject passed in the constructor. Either parent or child
     @param iteratorInA the iterator of this LinkObject in the BaseObject::links map of the first parameter
     
     @warning This method calls remove() on the child object but only if it is called from the parent
     */
    void remove(BaseObject* a, const std::map<std::string, BaseLink*>::iterator& iteratorInA);
    void init(LinkInitializer& initializer);
    rapidxml::xml_node<xmlUsedCharType>* write(rapidxml::xml_document<xmlUsedCharType>& doc, LinkInitializer* linkSaver);
};

#endif