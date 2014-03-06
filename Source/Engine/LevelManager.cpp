//
//  LevelManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "AppMain.h"

//! @cond
struct Compound {
    typedef std::pair<std::string, btTransform> Element;
    std::unordered_set<Element*> elements;
    std::string name;
};
//! @endcond

LevelManager::LevelManager() {
    
}

btCollisionShape* LevelManager::getCollisionShape(const std::string& name) {
    btCollisionShape* shape = levelManager.sharedCollisionShapes[name];
    if(shape) return shape;
    log(error_log, std::string("Couldn't find collision shape with id ")+name+'.');
    return NULL;
}

const char* LevelManager::getCollisionShapeName(btCollisionShape* shape) {
    foreach_e(sharedCollisionShapes, iterator)
        if(iterator->second == shape)
            return iterator->first.c_str();
    return "";
}

void LevelManager::clear() {
    if(scriptManager)
        levelManager.mainScript->callFunction("onleave", false, 0, NULL);
    saveGameName = levelContainer = "";
    levelPackage = NULL;
    mainScript = NULL;
    gameStatus = noGame;
    for(auto iterator: sharedCollisionShapes)
        delete iterator.second;
    sharedCollisionShapes.clear();
    objectManager.clear();
    fileManager.clear();
}

bool LevelManager::loadGame(FilePackage* package, const std::string& name, const std::string& container) {
    levelPackage = package;
    saveGameName = name;
    levelContainer = container;
    fileManager.clear();
    
    //Load CollisionShapes
    std::unordered_set<Compound*> compounds;
    
    for(auto packageIterator : fileManager.filePackages) {
        rapidxml::xml_document<xmlUsedCharType> doc;
        std::unique_ptr<char[]> fileData = readXmlFile(doc, packageIterator.second->path+"/CollisionShapes.xml", false);
        if(!fileData) continue;
        
        rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("CollisionShapes")->first_node();
        while(node) {
            const char* name = node->first_attribute("id")->value();
            if(sharedCollisionShapes.find(name) != sharedCollisionShapes.end()) {
                log(error_log, std::string("Tried to overwrite collision shape by id: ")+name+".");
                menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
                return false;
            }
            
            if(strcmp(node->name(), "Cylinder") == 0 || strcmp(node->name(), "Box") == 0) {
                XMLValueArray<float> vecData;
                vecData.readString(node->first_attribute("size")->value(), "%f");
                if(strcmp(node->name(), "Cylinder") == 0) {
                    char* direction = node->first_attribute("direction")->value();
                    if(strcmp(direction, "x") == 0)
                        sharedCollisionShapes[name] = new btCylinderShapeX(vecData.getVector3());
                    else if(strcmp(direction, "y") == 0)
                        sharedCollisionShapes[name] = new btCylinderShape(vecData.getVector3());
                    else if(strcmp(direction, "z") == 0)
                        sharedCollisionShapes[name] = new btCylinderShapeZ(vecData.getVector3());
                    else{
                        log(error_log, std::string("Found collision shape (")+name+") with an unknown direction: "+direction+'.');
                        menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
                        return false;
                    }
                }else
                    sharedCollisionShapes[name] = new btBoxShape(vecData.getVector3());
            }else if(strcmp(node->name(), "Sphere") == 0) {
                float radius;
                sscanf(node->first_attribute("radius")->value(), "%f", &radius);
                sharedCollisionShapes[name] = new btSphereShape(radius);
            }else if(strcmp(node->name(), "Capsule") == 0 || strcmp(node->name(), "Cone") == 0) {
                bool isCapsule = (strcmp(node->name(), "Capsule") == 0);
                float radius, length;
                sscanf(node->first_attribute("radius")->value(), "%f", &radius);
                sscanf(node->first_attribute("length")->value(), "%f", &length);
                char* direction = node->first_attribute("direction")->value();
                if(strcmp(direction, "x") == 0) {
                    if(isCapsule)
                        sharedCollisionShapes[name] = new btCapsuleShapeX(radius, length);
                    else
                        sharedCollisionShapes[name] = new btConeShapeX(radius, length);
                }else if(strcmp(direction, "y") == 0) {
                    if(isCapsule)
                        sharedCollisionShapes[name] = new btCapsuleShape(radius, length);
                    else
                        sharedCollisionShapes[name] = new btConeShape(radius, length);
                }else if(strcmp(direction, "z") == 0) {
                    if(isCapsule)
                        sharedCollisionShapes[name] = new btCapsuleShapeZ(radius, length);
                    else
                        sharedCollisionShapes[name] = new btConeShapeZ(radius, length);
                }else{
                    log(error_log, std::string("Found collision shape (")+name+") with an unknown direction: "+direction+'.');
                    menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
                    return false;
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
                    XMLValueArray<float> vecData;
                    vecData.readString(childNode->first_attribute("position")->value(), "%f");
                    positions[count] = vecData.getVector3();
                    sscanf(childNode->first_attribute("radius")->value(), "%f", &radi[count]);
                    count ++;
                    childNode = childNode->next_sibling("Sphere");
                }
                sharedCollisionShapes[name] = new btMultiSphereShape(positions.get(), radi.get(), count);
            }else if(strcmp(node->name(), "Compound") == 0) {
                Compound* compound = new Compound();
                rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("Child");
                while(childNode) {
                    compound->elements.insert(new Compound::Element(childNode->first_attribute("collisionShape")->value(), readTransformationXML(childNode)));
                    childNode = childNode->next_sibling("Child");
                }
                compound->name = name;
                compounds.insert(compound);
            }else if(strcmp(node->name(), "VertexCloud") == 0) {
                XMLValueArray<btScalar> vertices;
                sscanf(node->first_attribute("vertexCount")->value(), "%d", &vertices.count);
                vertices.readString(node, vertices.count*3, "%f");
                vertices.count /= 3;
                sharedCollisionShapes[name] = new btConvexHullShape(vertices.data, vertices.count, sizeof(btScalar)*3);
                if(vertices.count > 100)
                    log(warning_log, std::string("Found vertex cloud shape (")+name+") with more than 100 vertices.");
            }else if(strcmp(node->name(), "StaticPlane") == 0) {
                btScalar distance;
                sscanf(node->first_attribute("distance")->value(), "%f", &distance);
                XMLValueArray<btScalar> vec;
                vec.readString(node->value(), "%f");
                sharedCollisionShapes[name] = new btStaticPlaneShape(btVector3(vec.data[0], vec.data[1], vec.data[2]), distance);
            }else{
                log(error_log, std::string("Found collision shape (")+name+") shape with an unknown type: "+node->name()+'.');
                menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
                return false;
            }
            node = node->next_sibling();
        }
    }
    
    foreach_e(compounds, compound) {
        btCompoundShape* compoundShape = new btCompoundShape();
        foreach_e((*compound)->elements, compoundElement) {
            btCollisionShape* childShape = getCollisionShape((*compoundElement)->first);
            if(!childShape) {
                log(error_log, std::string("Found compound collision shape (")+(*compoundElement)->first+") with an invalid child.");
                menu.setModalView("error", fileManager.localizeString("packageError_Corrupted"), nullptr);
                return false;
            }
            compoundShape->addChildShape((*compoundElement)->second, childShape);
            delete *compoundElement;
        }
        sharedCollisionShapes[(*compound)->name] = compoundShape;
        delete *compound;
    }
    
    mainFBO.init();
    scriptManager.reset(new ScriptManager());
    mainScript = fileManager.getResourceByPath<ScriptFile>(package, "Main");
    LevelLoader levelLoader;
    return levelLoader.loadLevel();
}

bool LevelManager::newGame(FilePackage* package, const std::string& name) {
    if(!createDir(supportPath+"Saves/"+name+'/'))
        return false;
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* rootNode = doc.allocate_node(rapidxml::node_element);
    rootNode->name("Status");
    doc.append_node(rootNode);
    addXMLNode(doc, rootNode, "EngineVersion", VERSION);
    addXMLNode(doc, rootNode, "Package", package->name.c_str());
    addXMLNode(doc, rootNode, "Level", package->initialContainer.c_str());
    writeXmlFile(doc, supportPath+"Saves/"+name+"/Status.xml", true);
    
    return loadGame(package, name, package->initialContainer);
}

LevelManager levelManager;