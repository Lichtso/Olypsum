//
//  LevelLoader.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#import "WorldManager.h"

LevelLoader::LevelLoader() {
    
}

LevelLoader::~LevelLoader() {
    //Clean up unused shared collision shapes
    decltype(collisionShapeNodes)::iterator iterator;
    for(iterator = collisionShapeNodes.begin(); iterator != collisionShapeNodes.end(); iterator ++) {
        auto sharedCollisionShape = worldManager.sharedCollisionShapes.find(iterator->first);
        if(sharedCollisionShape == worldManager.sharedCollisionShapes.end()) continue;
        delete sharedCollisionShape->second;
        worldManager.sharedCollisionShapes.erase(sharedCollisionShape);
    }
}

void LevelLoader::deleteCollisionShapeNode(std::string name) {
    auto iterator = collisionShapeNodes.find(name);
    if(iterator != collisionShapeNodes.end()) return;
    rapidxml::xml_node<xmlUsedCharType>* childNode = iterator->second->first_node("Child");
    while(childNode) {
        deleteCollisionShapeNode(childNode->first_attribute("collisionShape")->value());
        childNode = childNode->next_sibling("Child");
    }
    collisionShapeNodes.erase(iterator);
}

btCollisionShape* LevelLoader::getCollisionShape(std::string name) {
    btCollisionShape* shape = worldManager.sharedCollisionShapes[name];
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
        Vector3 size;
        sscanf(node->first_attribute("width")->value(), "%f", &size.x);
        sscanf(node->first_attribute("height")->value(), "%f", &size.y);
        sscanf(node->first_attribute("length")->value(), "%f", &size.z);
        if(strcmp(node->name(), "Cylinder") == 0)
            shape = new btCylinderShape(size.getBTVector());
        else
            shape = new btBoxShape(size.getBTVector());
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
            Matrix4 transformation;
            transformation.readTransform(childNode);
            compoundShape->addChildShape(transformation.getBTMatrix(), childShape);
            childNode = childNode->next_sibling("Child");
        }
        shape = compoundShape;
    }else if(strcmp(node->name(), "VertexCloud") == 0) {
        XMLValueArray<float> vertices;
        sscanf(node->first_attribute("vertexCount")->value(), "%d", &vertices.count);
        vertices.readString(node, vertices.count*3, "%f");
        vertices.count /= 3;
        shape = new btConvexHullShape(vertices.data, vertices.count, sizeof(float)*3);
        if(vertices.count > 100)
            log(warning_log, std::string("Found vertex cloud shape (")+name+") with more than 100 vertices.");
    }else{
        log(error_log, std::string("Found collision shape (")+name+") shape with an unknown type: "+node->name()+'.');
        return NULL;
    }
    worldManager.sharedCollisionShapes[name] = shape;
    collisionShapeNodes.erase(nodeIterator);
    return shape;
}

bool LevelLoader::loadLevel() {
    //Load CollisionShape index
    rapidxml::xml_document<xmlUsedCharType> doc;
    collisionShapesData = readXmlFile(doc, worldManager.gamePackage->path+'/'+"CollisionShapes.xml", false);
    if(!collisionShapesData) {
        log(error_log, "Could not load package, because CollisionShapes.xml is missing.");
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("CollisionShapes")->first_node();
    while(node) {
        collisionShapeNodes[node->first_attribute("id")->value()] = node;
        node = node->next_sibling();
    }
    
    //
    
    return true;
}