//
//  WorldManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#import "Menu.h"
#import "WorldManager.h"

WorldManager::WorldManager() {
    
}

WorldManager::~WorldManager() {
    
}

void WorldManager::loadLevel() {
    objectManager.initPhysics(btVector3(10, 10, 10));
    
    btVector3 inertia;
    objectManager.sharedCollisionShapes["objectShape"] = new btBoxShape(btVector3(1, 1, 0.2));
    objectManager.sharedCollisionShapes["objectShape"]->calculateLocalInertia(1.0, inertia);
    simpleMotionState* MS = new simpleMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(0, 0, 0)));
    btRigidBody::btRigidBodyConstructionInfo cI(0, MS, objectManager.sharedCollisionShapes["objectShape"], btVector3(0, 0, 0));
    RigidObject* a = new RigidObject(fileManager.getPackage("Default")->getResource<Model>("man.dae"), cI);
    
    /*MS = new simpleMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(1.1, 4, 0)));
    btRigidBody::btRigidBodyConstructionInfo cI2(0, MS, sharedCollisionShapes["objectShape"], inertia);
    RigidObject* b = new RigidObject(fileManager.getPackage("Default")->getResource<Model>("man.dae"), cI2);*/
    
    /*btHingeConstraint* constraint = new btHingeConstraint(*a->getBody(), *b->getBody(),
                                                          btVector3(0, -2, 0), btVector3(0, 1, 0),
                                                          btVector3(0, 0, 1), btVector3(1, 0, 0));
    new PhysicLink(a, b, "b", "a", constraint);*/
    TransformLink* link = (TransformLink*)a->links["Back"];
    link = (TransformLink*)link->getOther(a)->links["Hip"];
    auto animationR = new TransformLink::AnimationEntry();
    animationR->frames.push_back(new TransformLink::AnimationEntry::Frame(0, 0, 4, btQuaternion(0, 0, 0), btVector3(0, 0, 0)));
    animationR->frames.push_back(new TransformLink::AnimationEntry::Frame(1, 1, 2, btQuaternion(M_PI_2, 0, 0), btVector3(2, 0, 0)));
    animationR->frames.push_back(new TransformLink::AnimationEntry::Frame(1, 1, 3, btQuaternion(0, M_PI_2, 0), btVector3(0, 2, 0)));
    link->transforms.push_back(animationR);
    
    SpotLight* light = new SpotLight();
    light->setBounds(30.0/180.0*M_PI, 5.0);
    light->setTransformation(btTransform(btQuaternion(0.8, 0.2, 0), btVector3(2, 1, 2)));
    
    gameStatus = localGame;
    setMenu(inGameMenu);
}

void WorldManager::saveLevel() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+levelPackageName+'/'+"Status.xml", false);
    doc.first_node("Status")->first_node("Level")->first_attribute("value")->value(levelId.c_str());
    writeXmlFile(doc, gameDataDir+"Saves/"+levelPackageName+'/'+"Status.xml", true);
}

void WorldManager::leaveGame() {
    levelId = "";
    levelPackageName = "";
    levelPackage = NULL;
    gameStatus = noGame;
    objectManager.clear();
    fileManager.clear();
    fileManager.getPackage("Default");
    setMenu(mainMenu);
}

bool WorldManager::loadGame(std::string name) {
    std::string path = gameDataDir+"Saves/"+name+'/';
    if(!checkDir(path)) {
        log(error_log, "Could not find a saved game by this name.");
        return false;
    }
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", false);
    if(!fileData) {
        log(error_log, "Could not load saved game, because Status.xml is missing.");
        return false;
    }
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.first_node("Status");
    if(strcmp(statusNode->first_node("Version")->first_attribute("value")->value(), VERSION) != 0) {
        log(error_log, "Could not load saved game, because its version is outdated.");
        return false;
    }
    levelPackageName = name;
    levelPackage = fileManager.getPackage(statusNode->first_node("Package")->first_attribute("value")->value());
    levelId = statusNode->first_node("Level")->first_attribute("value")->value();
    loadLevel();
    return true;
}

bool WorldManager::newGame(std::string packageName, std::string name) {
    if(!createDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not create new game, because the name already exists.");
        return false;
    }
    
    rapidxml::xml_document<xmlUsedCharType> doc;
    rapidxml::xml_node<xmlUsedCharType>* statusNode = doc.allocate_node(rapidxml::node_element);
    statusNode->name("Status");
    doc.append_node(statusNode);
    addXMLNode(doc, statusNode, "Version", VERSION);
    addXMLNode(doc, statusNode, "Package", packageName.c_str());
    addXMLNode(doc, statusNode, "Level", "start");
    writeXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", true);
    return loadGame(name);
}

bool WorldManager::removeGame(std::string name) {
    if(!removeDir(gameDataDir+"Saves/"+name+'/')) {
        log(error_log, "Could not remove a saved game by this name.");
        return false;
    }
    return true;
}

WorldManager worldManager;