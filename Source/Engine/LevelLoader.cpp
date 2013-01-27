//
//  LevelLoader.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#include "Controls.h"

LevelLoader::LevelLoader() :transformation(btTransform::getIdentity()) {
    
}

LevelLoader::~LevelLoader() {
    //Clean up unused shared collision shapes
    decltype(collisionShapeNodes)::iterator iterator;
    for(iterator = collisionShapeNodes.begin(); iterator != collisionShapeNodes.end(); iterator ++) {
        auto sharedCollisionShape = levelManager.sharedCollisionShapes.find(iterator->first);
        if(sharedCollisionShape == levelManager.sharedCollisionShapes.end()) continue;
        delete sharedCollisionShape->second;
        levelManager.sharedCollisionShapes.erase(sharedCollisionShape);
    }
}

void LevelLoader::deleteCollisionShapeNode(std::string name) {
    auto iterator = collisionShapeNodes.find(name);
    if(iterator == collisionShapeNodes.end()) return;
    rapidxml::xml_node<xmlUsedCharType>* childNode = iterator->second->first_node("Child");
    while(childNode) {
        deleteCollisionShapeNode(childNode->first_attribute("collisionShape")->value());
        childNode = childNode->next_sibling("Child");
    }
    collisionShapeNodes.erase(iterator);
}

btCollisionShape* LevelLoader::getCollisionShape(std::string name) {
    btCollisionShape* shape = levelManager.sharedCollisionShapes[name];
    if(shape) {
        deleteCollisionShapeNode(name);
        return shape;
    }
    auto nodeIterator = collisionShapeNodes.find(name);
    if(nodeIterator == collisionShapeNodes.end()) {
        log(error_log, std::string("Couldn't find collision shape with id ")+name+'.');
        return NULL;
    }
    rapidxml::xml_node<xmlUsedCharType>* node = nodeIterator->second;
    if(strcmp(node->name(), "Cylinder") == 0 || strcmp(node->name(), "Box") == 0) {
        btScalar x, y, z;
        sscanf(node->first_attribute("width")->value(), "%f", &x);
        sscanf(node->first_attribute("height")->value(), "%f", &y);
        sscanf(node->first_attribute("length")->value(), "%f", &z);
        if(strcmp(node->name(), "Cylinder") == 0)
            shape = new btCylinderShape(btVector3(x, y, z));
        else
            shape = new btBoxShape(btVector3(x, y, z));
    }else if(strcmp(node->name(), "Sphere") == 0) {
        float radius;
        sscanf(node->first_attribute("radius")->value(), "%f", &radius);
        shape = new btSphereShape(radius);
    }else if(strcmp(node->name(), "Capsule") == 0 || strcmp(node->name(), "Cone") == 0) {
        bool isCapsule = (strcmp(node->name(), "Capsule") == 0);
        float radius, length;
        sscanf(node->first_attribute("radius")->value(), "%f", &radius);
        sscanf(node->first_attribute("length")->value(), "%f", &length);
        char* direction = node->first_attribute("direction")->value();
        if(strcmp(direction, "x") == 0) {
            if(isCapsule)
                shape = new btCapsuleShapeX(radius, length);
            else
                shape = new btConeShapeX(radius, length);
        }else if(strcmp(direction, "y") == 0) {
            if(isCapsule)
                shape = new btCapsuleShape(radius, length);
            else
                shape = new btConeShape(radius, length);
        }else if(strcmp(direction, "z") == 0) {
            if(isCapsule)
                shape = new btCapsuleShapeZ(radius, length);
            else
                shape = new btConeShapeZ(radius, length);
        }else{
            log(error_log, std::string("Found collision shape (")+name+") with an unknown direction: "+direction+'.');
            return NULL;
        }
    }else if(strcmp(node->name(), "SphereCompound") == 0) {
        unsigned int count = 0;
        rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("Sphere");
        while(childNode) {
            count ++;
            childNode = childNode->next_sibling("Sphere");
        }
        std::unique_ptr<btVector3[]> positions(new btVector3[count]);
        std::unique_ptr<btScalar[]> radi(new btScalar[count]);
        count = 0;
        childNode = node->first_node("Sphere");
        while(childNode) {
            float aux;
            sscanf(childNode->first_attribute("x")->value(), "%f", &aux);
            positions[count].setX(aux);
            sscanf(childNode->first_attribute("y")->value(), "%f", &aux);
            positions[count].setY(aux);
            sscanf(childNode->first_attribute("z")->value(), "%f", &aux);
            positions[count].setZ(aux);
            sscanf(childNode->first_attribute("radius")->value(), "%f", &radi[count]);
            count ++;
            childNode = childNode->next_sibling("Sphere");
        }
        shape = new btMultiSphereShape(positions.get(), radi.get(), count);
    }else if(strcmp(node->name(), "Compound") == 0) {
        btCompoundShape* compoundShape = new btCompoundShape();
        rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("Child");
        while(childNode) {
            btCollisionShape* childShape = getCollisionShape(childNode->first_attribute("collisionShape")->value());
            if(!childShape) {
                log(error_log, std::string("Found compound collision shape (")+name+") with an invalid child.");
                return NULL;
            }
            btTransform transformation = readTransformationXML(childNode);
            compoundShape->addChildShape(transformation, childShape);
            childNode = childNode->next_sibling("Child");
        }
        shape = compoundShape;
    }else if(strcmp(node->name(), "VertexCloud") == 0) {
        XMLValueArray<btScalar> vertices;
        sscanf(node->first_attribute("vertexCount")->value(), "%d", &vertices.count);
        vertices.readString(node, vertices.count*3, "%f");
        vertices.count /= 3;
        shape = new btConvexHullShape(vertices.data, vertices.count, sizeof(btScalar)*3);
        if(vertices.count > 100)
            log(warning_log, std::string("Found vertex cloud shape (")+name+") with more than 100 vertices.");
    }else if(strcmp(node->name(), "StaticPlane") == 0) {
        btScalar distance;
        sscanf(node->first_attribute("distance")->value(), "%f", &distance);
        XMLValueArray<btScalar> vec;
        vec.readString(node->value(), "%f");
        shape = new btStaticPlaneShape(btVector3(vec.data[0], vec.data[1], vec.data[2]), distance);
    }else{
        log(error_log, std::string("Found collision shape (")+name+") shape with an unknown type: "+node->name()+'.');
        return NULL;
    }
    levelManager.sharedCollisionShapes[name] = shape;
    collisionShapeNodes.erase(nodeIterator);
    return shape;
}

BaseObject* LevelLoader::getObjectLinking(const char* id) {
    unsigned int offset;
    sscanf(id, "%d", &offset);
    offset += objectLinkingOffset;
    if(offset < objectLinkingScope || offset >= objectLinkingIndex.size()) {
        log(error_log, std::string("Object linking offset out of scope: ")+id+'.');
        return NULL;
    }
    return objectLinkingIndex[offset];
}

void LevelLoader::pushObject(BaseObject* object) {
    objectLinkingIndex.push_back(object);
}

bool LevelLoader::loadContainer(std::string name) {
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> rawData = readXmlFile(doc, gameDataDir+"Saves/"+levelManager.saveGameName+"/Containers/"+name+".xml", false);
    if(!rawData)
        rawData = readXmlFile(doc, levelManager.levelPackage->path+"/Containers/"+name+".xml", false);
    if(!rawData) {
        log(error_log, std::string("Could not find container by name: ")+name+'.');
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* containerNode = doc.first_node("Container");
    if(!containerNode) {
        log(error_log, std::string("Could not find \"Container\"-node in container: ")+name+'.');
        return false;
    }
    //Push the container on the stack and avoid self recursion
    if(containerStack.find(name) != containerStack.end()) {
        log(error_log, std::string("Container contains its self: ")+name+'.');
        return false;
    }
    containerStack.insert(name);
    //Prepare object linking
    unsigned int objectLinkingScopePrev = objectLinkingIndex.size();
    //Check for Level-node
    rapidxml::xml_node<xmlUsedCharType>* node = containerNode->first_node();
    if(strcmp(node->name(), "Level") == 0) {
        if(containerStack.size() > 1) {
            log(error_log, "Found a \"Level\"-node in a child container.");
            return false;
        }
        rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("gravity");
        if(!attribute) {
            log(error_log, "Tried to construct Level without \"gravity\"-attribute.");
            return false;
        }
        XMLValueArray<float> vecData;
        vecData.readString(attribute->value(), "%f");
        objectManager.physicsWorld->setGravity(vecData.getVector3());
        node = node->next_sibling();
    }else if(containerStack.size() == 1) {
        log(error_log, "Root container does not begin with a \"Level\"-node.");
        return false;
    }
    //Load containers
    node = containerNode->first_node("Container");
    while(node) {
        rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("src");
        if(!attribute) {
            log(error_log, "Tried to construct Container without \"src\"-attribute.");
            return false;
        }
        btTransform parentTransform = transformation;
        transformation *= readTransformationXML(node);
        if(!loadContainer(attribute->value())) return false;
        transformation = parentTransform;
        node = node->next_sibling("Container");
    }
    //Prepare object linking
    objectLinkingScope = objectLinkingScopePrev;
    objectLinkingOffset = objectLinkingIndex.size();
    //Load objects
    node = containerNode->first_node("Objects");
    if(node) {
        node = node->first_node();
        while(node) {
            if(strcmp(node->name(), "RigidObject") == 0) {
                new RigidObject(node, this);
            }else if(strcmp(node->name(), "LightObject") == 0) {
                rapidxml::xml_attribute<xmlUsedCharType>* attribute = node->first_attribute("type");
                if(!attribute) {
                    log(error_log, "Tried to construct LightObject without \"type\"-attribute.");
                    return false;
                }
                if(strcmp(attribute->value(), "positional") == 0)
                    new PositionalLight(node, this);
                else if(strcmp(attribute->value(), "spot") == 0)
                    new SpotLight(node, this);
                else if(strcmp(attribute->value(), "directional") == 0)
                    new DirectionalLight(node, this);
            }else if(strcmp(node->name(), "ParticlesObject") == 0) {
                new ParticlesObject(node, this);
            }else if(strcmp(node->name(), "SoundSourceObject") == 0) {
                new SoundSourceObject(node, this);
            }else if(strcmp(node->name(), "HeightfieldTerrain") == 0) {
                new HeightfieldTerrain(node, this);
            }else if(strcmp(node->name(), "WaterObject") == 0) {
                new WaterObject(node, this);
            }else if(strcmp(node->name(), "Cam") == 0) {
                new Cam(node, this);
            }else{
                log(error_log, std::string("Tried to construct invalid Object: ")+node->name()+'.');
                return false;
            }
            node = node->next_sibling();
        }
    }
    //Load links
    node = containerNode->first_node("Links");
    if(node) {
        node = node->first_node();
        while(node) {
            if(strcmp(node->name(), "BaseLink") == 0) {
                new BaseLink(node, this);
            }else if(strcmp(node->name(), "PhysicLink") == 0) {
                new PhysicLink(node, this);
            }else if(strcmp(node->name(), "TransformLink") == 0) {
                new TransformLink(node, this);
            }else{
                log(error_log, std::string("Tried to construct invalid Link: ")+node->name()+'.');
                return false;
            }
            node = node->next_sibling();
        }
    }
    //Pop the container from the stack
    containerStack.erase(name);
    return true;
}

bool LevelLoader::loadLevel() {
    controlsMangager = new ControlsMangager();
    std::unique_ptr<ControlsMangager> newControlsMangager(controlsMangager);
    
    //Load CollisionShape index
    rapidxml::xml_document<xmlUsedCharType> doc;
    collisionShapesData = readXmlFile(doc, levelManager.levelPackage->path+'/'+"CollisionShapes.xml", false);
    if(!collisionShapesData) {
        log(error_log, "Could not load package, because CollisionShapes.xml is missing.");
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("CollisionShapes")->first_node();
    while(node) {
        collisionShapeNodes[node->first_attribute("id")->value()] = node;
        node = node->next_sibling();
    }
    
    //Load root conatiner
    objectManager.clear();
    objectManager.initPhysics();
    if(!loadContainer(levelManager.levelId)) return false;
    
    levelManager.gameStatus = localGame;
    setMenu(inGameMenu);
    newControlsMangager.release();
    return true;
}