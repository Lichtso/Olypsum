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
    physicsWorld = NULL;
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
}

WorldManager::~WorldManager() {
    clearAll();
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
}

void WorldManager::clearAll() {
    objectManager.clear();
    clearPhysics();
}

static void calculatePhysicsTick(btDynamicsWorld* world, btScalar timeStep) {
    printf("%d\n", worldManager.physicsWorld->getDispatcher()->getNumManifolds());
    objectManager.physicsTick();
}

void WorldManager::clearPhysics() {
    if(physicsWorld == NULL) return;
    for(unsigned char i = 0; i < 6; i ++) {
        delete worldWallBodys[i]->getMotionState();
        delete worldWallBodys[i];
    }
    for(auto iterator: sharedCollisionShapes)
        delete iterator.second;
    sharedCollisionShapes.clear();
    delete physicsWorld;
    delete broadphase;
    physicsWorld = NULL;
}

void WorldManager::loadLevel() {
    btVector3 worldSize(10, 10, 10);
    if(physicsWorld != NULL) {
        delete physicsWorld;
        delete broadphase;
    }
    broadphase = new btDbvtBroadphase();
    //broadphase = new btAxisSweep3(-worldSize, worldSize);
    physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    physicsWorld->setGravity(btVector3(0, -9.81, 0));
    physicsWorld->setInternalTickCallback(calculatePhysicsTick);
    
    if(!sharedCollisionShapes["worldWall"])
        sharedCollisionShapes["worldWall"] = new btStaticPlaneShape(btVector3(0, 1, 0), 0);
    btDefaultMotionState* wallMotionState[6];
    wallMotionState[0] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, -M_PI_2), btVector3(-worldSize.x(), 0, 0)));
    wallMotionState[1] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI_2), btVector3(worldSize.x(), 0, 0)));
    wallMotionState[2] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(0, -worldSize.y(), 0)));
    wallMotionState[3] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI), btVector3(0, worldSize.y(), 0)));
    wallMotionState[4] = new btDefaultMotionState(btTransform(btQuaternion(0, M_PI_2, 0), btVector3(0, 0, -worldSize.z())));
    wallMotionState[5] = new btDefaultMotionState(btTransform(btQuaternion(0, -M_PI_2, 0), btVector3(0, 0, worldSize.z())));
    for(unsigned char i = 0; i < 6; i ++) {
        worldWallBodys[i] = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, wallMotionState[i], sharedCollisionShapes["worldWall"], btVector3(0, 0, 0)));
        physicsWorld->addRigidBody(worldWallBodys[i], 0, CollisionMask_Object);
    }
    
    btDefaultMotionState* MS = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(0, 0, 0)));
    sharedCollisionShapes["objectShape"] = new btBoxShape(btVector3(1, 1, 0.2));
    btRigidBody::btRigidBodyConstructionInfo cI(0, MS, sharedCollisionShapes["objectShape"], btVector3(0, 0, 0));
    new RigidObject(fileManager.getPackage("Default")->getResource<Model>("man.dae"), cI);
    
    gameStatus = localGame;
    setMenu(inGameMenu);
}

void WorldManager::saveLevel() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", false);
    char mapStr[8];
    sprintf(mapStr, "%d", levelId);
    doc.first_node("Status")->first_node("Level")->first_attribute("value")->value(mapStr);
    writeXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", true);
}

void WorldManager::gameTick() {
    physicsWorld->stepSimulation(animationFactor, 4, 1.0/60.0);
}

void WorldManager::leaveGame() {
    levelId = -1;
    gameName = "";
    gameStatus = noGame;
    clearAll();
    fileManager.clear();
    fileManager.loadPackage("Default");
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
    gamePackage = fileManager.loadPackage(statusNode->first_node("Package")->first_attribute("value")->value());
    sscanf(statusNode->first_node("Level")->first_attribute("value")->value(), "%d", &levelId);
    gameName = name;
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
    addXMLNode(doc, statusNode, "Level", "0");
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