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
        delete sharedCollisionShape->second;
        worldManager.sharedCollisionShapes.erase(sharedCollisionShape);
    }
}

btCollisionShape* LevelLoader::getCollisionShape(std::string name) {
    btCollisionShape* shape = worldManager.sharedCollisionShapes[name];
    if(shape) {
        auto iterator = collisionShapeNodes.find(name);
        if(iterator == collisionShapeNodes.end())
            collisionShapeNodes.erase(iterator);
        return shape;
    }
    rapidxml::xml_node<xmlUsedCharType>* node = collisionShapeNodes[name];
    if(!node) return NULL;
    char* type = node->first_attribute("type")->value();
    if(strcmp(type, "Cylinder") == 0 || strcmp(type, "Box") == 0) {
        Vector3 size;
        sscanf(node->first_attribute("width")->value(), "%f", &size.x);
        sscanf(node->first_attribute("height")->value(), "%f", &size.y);
        sscanf(node->first_attribute("length")->value(), "%f", &size.z);
        if(strcmp(type, "Cylinder") == 0)
            shape = new btCylinderShape(size.getBTVector());
        else
            shape = new btBoxShape(size.getBTVector());
    }else if(strcmp(type, "Sphere") == 0) {
        float radius;
        sscanf(node->first_attribute("radius")->value(), "%f", &radius);
        shape = new btSphereShape(radius);
    }else if(strcmp(type, "Capsule") == 0 || strcmp(type, "Cone") == 0) {
        bool isCapsule = (strcmp(type, "Capsule") == 0);
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
            log(error_log, "Found collision shape with an unknown direction.");
            return NULL;
        }
    }else if(strcmp(type, "SphereCompound") == 0) {
        unsigned int count = 0;
        rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("sphere");
        while(childNode) {
            count ++;
            childNode = childNode->next_sibling("sphere");
        }
        std::unique_ptr<btVector3[]> positions(new btVector3[count]);
        std::unique_ptr<btScalar[]> radi(new btScalar[count]);
        count = 0;
        childNode = node->first_node("sphere");
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
            childNode = childNode->next_sibling("sphere");
        }
        shape = new btMultiSphereShape(positions.get(), radi.get(), count);
    }else if(strcmp(type, "Compound") == 0) {
        btCompoundShape* compoundShape = new btCompoundShape();
        rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("child");
        while(childNode) {
            btCollisionShape* childShape = getCollisionShape(childNode->first_attribute("id")->value());
            if(!childShape) {
                log(error_log, "Found compound collision shape with an invalid child.");
                return NULL;
            }
            rapidxml::xml_node<xmlUsedCharType>* transformationNode = node->first_node("transformation");
            if(!transformationNode) {
                log(error_log, "Found compound collision shape child with no transformation node.");
                return NULL;
            }
            Matrix4 transformation;
            transformation.readTransform(transformationNode);
            compoundShape->addChildShape(transformation.getBTMatrix(), childShape);
            childNode = childNode->next_sibling("child");
        }
        shape = compoundShape;
    }else if(strcmp(type, "VertexCloud") == 0) {
        XMLValueArray<float> vertices;
        sscanf(node->first_attribute("vertex")->value(), "%d", &vertices.count);
        vertices.readString(node, vertices.count*3, "%f");
        vertices.count /= 3;
        shape = new btConvexHullShape(vertices.data, vertices.count, sizeof(float)*3);
        if(vertices.count > 100)
            log(warning_log, "Found vertex cloud shape with more than 100 vertices.");
    }else{
        log(error_log, "Found collision shape with an unknown type.");
        return NULL;
    }
    worldManager.sharedCollisionShapes[name] = shape;
    collisionShapeNodes.erase(name);
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
    rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("CollisionShape");
    while(node) {
        collisionShapeNodes[node->first_attribute("id")->value()] = node;
        node = node->next_sibling("CollisionShape");
    }
    
    //
    
    return true;
}