//
//  LevelLoader.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 13.10.12.
//
//

#include "AppMain.h"

LevelLoader::LevelLoader() :transformation(btTransform::getIdentity()) {
    
}

v8::Handle<v8::Array> LevelLoader::getResultsArray() {
    v8::HandleScope handleScope;
    unsigned int i = 0;
    v8::Handle<v8::Array> objects = v8::Array::New();
    for(auto hit : objectLinkingIndex)
        objects->Set(i ++, hit->scriptInstance);
    return handleScope.Close(objects);
}

BaseObject* LevelLoader::getObjectLinking(unsigned int index) {
    index += objectLinkingOffset;
    if(index < objectLinkingScope || index >= objectLinkingIndex.size()) {
        log(error_log, std::string("Object linking offset out of scope: "+stringOf(index)+'.'));
        return NULL;
    }
    return objectLinkingIndex[index];
}

void LevelLoader::pushObject(BaseObject* object) {
    objectLinkingIndex.push_back(object);
}

bool LevelLoader::loadContainer(std::string name, bool isLevelRoot) {
    v8::HandleScope handleScope;
    rapidxml::xml_document<xmlUsedCharType> doc;
    
    std::unique_ptr<char[]> rawData = readXmlFile(doc, gameDataDir+"Saves/"+levelManager.saveGameName+"/Containers/"+name+".xml", false);
    if(!rawData)
        rawData = readXmlFile(doc, levelManager.levelPackage->path+"/Containers/"+name+".xml", false);
    if(!rawData) {
        levelManager.showErrorModal(localization.localizeString("packageError_ContainerMissing")+'\n'+name);
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
    rapidxml::xml_node<xmlUsedCharType>* levelNode = containerNode->first_node("Level");
    if(levelNode) {
        if(!isLevelRoot) {
            log(error_log, "Found a \"Level\"-node in a child container.");
            return false;
        }
        rapidxml::xml_node<xmlUsedCharType>* parameterNode = levelNode->first_node("Gravity");
        if(!parameterNode) {
            log(error_log, "Tried to construct Level without \"Gravity\"-node.");
            return false;
        }
        XMLValueArray<float> vecData;
        vecData.readString(parameterNode->value(), "%f");
        objectManager.physicsWorld->setGravity(vecData.getVector3());
        parameterNode = levelNode->first_node("Ambient");
        if(parameterNode) {
            vecData.readString(parameterNode->value(), "%f");
            objectManager.sceneAmbient = vecData.getVector3();
        }
    }else if(isLevelRoot) {
        log(error_log, "Root container does not contain a \"Level\"-node.");
        return false;
    }
    
    //Load containers
    rapidxml::xml_attribute<xmlUsedCharType>* attribute;
    rapidxml::xml_node<xmlUsedCharType>* node = containerNode->first_node("Container");
    while(node) {
        attribute = node->first_attribute("src");
        if(!attribute) {
            log(error_log, "Tried to construct Container without \"src\"-attribute.");
            return false;
        }
        btTransform parentTransform = transformation;
        transformation *= readTransformationXML(node);
        if(!loadContainer(attribute->value(), false)) return false;
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
            BaseObject* object;
            if(strcmp(node->name(), "RigidObject") == 0) {
                object = new RigidObject(node, this);
            }else if(strcmp(node->name(), "LightObject") == 0) {
                attribute = node->first_attribute("type");
                if(!attribute) {
                    log(error_log, "Tried to construct LightObject without \"type\"-attribute.");
                    return false;
                }
                if(strcmp(attribute->value(), "positional") == 0)
                    object = new PositionalLight(node, this);
                else if(strcmp(attribute->value(), "spot") == 0)
                    object = new SpotLight(node, this);
                else if(strcmp(attribute->value(), "directional") == 0)
                    object = new DirectionalLight(node, this);
            }else if(strcmp(node->name(), "ParticlesObject") == 0) {
                object = new ParticlesObject(node, this);
            }else if(strcmp(node->name(), "SoundObject") == 0) {
                object = new SoundObject(node, this);
            }else if(strcmp(node->name(), "TerrainObject") == 0) {
                object = new TerrainObject(node, this);
            }else if(strcmp(node->name(), "CamObject") == 0) {
                object = new CamObject(node, this);
            }else{
                log(error_log, std::string("Tried to construct invalid Object: ")+node->name()+'.');
                return false;
            }
            
            object->newScriptInstance();
            rapidxml::xml_node<xmlUsedCharType>* scriptNode = node->first_node("Script");
            if(scriptNode) {
                attribute = scriptNode->first_attribute("src");
                if(attribute) {
                    FilePackage* filePackage;
                    std::string name;
                    if(fileManager.readResource(attribute->value(), filePackage, name)) {
                        object->scriptFile = scriptManager->getScriptFile(filePackage, name);
                        if(object->scriptFile)
                            object->scriptFile->callFunction("onload", true, { object->scriptInstance, scriptManager->readCdataXMLNode(node) });
                    }
                }else
                    log(error_log, "Tried to construct resource without \"src\"-attribute.");
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
    
    //Update status file
    if(levelNode) {
        v8::Handle<v8::Value> localData = scriptManager->readCdataXMLNode(levelNode);
        doc.clear();
        std::string statusFilePath = gameDataDir+"Saves/"+levelManager.saveGameName+"/Status.xml";
        std::unique_ptr<char[]> fileData = readXmlFile(doc, statusFilePath, true);
        doc.first_node("Status")->first_node("Level")->first_attribute("value")->value(name.c_str());
        v8::Handle<v8::Value> globalData = scriptManager->readCdataXMLNode(doc.first_node("Status"));
        if(!writeXmlFile(doc, statusFilePath, true))
            return false;
        levelManager.levelId = name;
        ScriptFile* script = scriptManager->getScriptFile(levelManager.levelPackage, MainScriptFileName);
        if(script) script->callFunction("onload", false, { localData, globalData });
        if(!mainCam) {
            log(error_log, "No CamObject was set as mainCam.");
            return false;
        }
    }
    return true;
}

bool LevelLoader::loadLevel(const std::string& levelPackage, const std::string& levelId) {
    objectManager.initGame(levelPackage);
    levelManager.gameStatus = localGame;
    
    //Load CollisionShapes
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> collisionShapesData = readXmlFile(doc, levelManager.levelPackage->path+'/'+"CollisionShapes.xml", false);
    if(!collisionShapesData) {
        levelManager.showErrorModal(localization.localizeString("packageError_FilesMissing"));
        return false;
    }
    
    rapidxml::xml_node<xmlUsedCharType>* node = doc.first_node("CollisionShapes")->first_node();
    while(node) {
        btCollisionShape* shape;
        const char* name = node->first_attribute("id")->value();
        if(strcmp(node->name(), "Cylinder") == 0 || strcmp(node->name(), "Box") == 0) {
            XMLValueArray<float> vecData;
            vecData.readString(node->first_attribute("size")->value(), "%f");
            
            if(strcmp(node->name(), "Cylinder") == 0)
                shape = new btCylinderShape(vecData.getVector3());
            else
                shape = new btBoxShape(vecData.getVector3());
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
            shape = new btMultiSphereShape(positions.get(), radi.get(), count);
        }else if(strcmp(node->name(), "Compound") == 0) {
            btCompoundShape* compoundShape = new btCompoundShape();
            rapidxml::xml_node<xmlUsedCharType>* childNode = node->first_node("Child");
            while(childNode) {
                btCollisionShape* childShape = levelManager.getCollisionShape(childNode->first_attribute("collisionShape")->value());
                if(!childShape) {
                    log(error_log, std::string("Found compound collision shape (")+name+") with an invalid child.");
                    return false;
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
            return false;
        }
        levelManager.sharedCollisionShapes[name] = shape;
        node = node->next_sibling();
    }
    
    //Load root conatiner
    if(!loadContainer(levelId, true)) {
        objectManager.clear();
        levelManager.gameStatus = noGame;
        return false;
    }
    
    //Update gameStatus and start the game
    objectManager.updateRendererSettings();
    menu.setPause(false);
    return true;
}