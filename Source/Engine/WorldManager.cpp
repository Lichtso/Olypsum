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
    soundSourcesManager.clear();
    objectManager.clear();
    particleSystemManager.clear();
    decalManager.clear();
    lightManager.clear();
    clearPhysics();
}

static void calculatePhysicsTick(btDynamicsWorld* world, btScalar timeStep) {
    
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
    Vector3 worldSize = Vector3(50, 50, 50);
    if(physicsWorld != NULL) {
        delete physicsWorld;
        delete broadphase;
    }
    broadphase = new btAxisSweep3(worldSize.getBTVector(), (worldSize*-1.0).getBTVector());
    physicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    physicsWorld->setGravity(btVector3(0, -9.81, 0));
    physicsWorld->setInternalTickCallback(calculatePhysicsTick);
    
    if(!sharedCollisionShapes["worldWall"])
        sharedCollisionShapes["worldWall"] = new btStaticPlaneShape(btVector3(1, 0, 0), 0);
    Matrix4 transform;
    btDefaultMotionState* wallMotionState[6];
    wallMotionState[0] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(-worldSize.x, 0, 0)));
    wallMotionState[1] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI), btVector3( worldSize.x, 0, 0)));
    wallMotionState[2] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, -M_PI_2), btVector3(0, -worldSize.y, 0)));
    wallMotionState[3] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI_2), btVector3(0,  worldSize.y, 0)));
    wallMotionState[4] = new btDefaultMotionState(btTransform(btQuaternion(-M_PI_2, 0, 0), btVector3(0, 0, -worldSize.z)));
    wallMotionState[5] = new btDefaultMotionState(btTransform(btQuaternion(M_PI_2, 0, 0), btVector3(0, 0,  worldSize.z)));
    for(unsigned char i = 0; i < 6; i ++) {
        worldWallBodys[i] = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, wallMotionState[i], sharedCollisionShapes["worldWall"], btVector3(0, 0, 0)));
        physicsWorld->addRigidBody(worldWallBodys[i]);
    }
    
    AnimatedObject* object = new AnimatedObject(fileManager.getPackage("Default")->getModel("man.dae"));
    object->transformation.setIdentity();
    objectManager.objects.push_back(object);
    mainCam->camMat.setIdentity();
    //mainCam->camMat.rotateX(0.5);
    mainCam->camMat.translate(Vector3(0,1,2));
    mainCam->calculate();
    
    gameStatus = localGame;
    setMenu(inGameMenu);
}

void WorldManager::saveLevel() {
    rapidxml::xml_document<xmlUsedCharType> doc;
    std::unique_ptr<char[]> fileData = readXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", false);
    char mapStr[8];
    sprintf(mapStr, "%d", levelId);
    doc.first_node("level")->first_attribute("value")->value(mapStr);
    writeXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", true);
}

void WorldManager::calculate() {
    physicsWorld->stepSimulation(animationFactor, 4, 1.0/60.0);
    printf("%d\n", physicsWorld->getDispatcher()->getNumManifolds());
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
    if(strcmp(doc.first_node("version")->first_attribute("value")->value(), VERSION) != 0) {
        log(error_log, "Could not load saved game, because its version is outdated.");
        return false;
    }
    gamePackage = fileManager.loadPackage(doc.first_node("package")->first_attribute("value")->value());
    sscanf(doc.first_node("level")->first_attribute("value")->value(), "%d", &levelId);
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
    addXMLNode(doc, &doc, "version", VERSION);
    addXMLNode(doc, &doc, "package", packageName.c_str());
    addXMLNode(doc, &doc, "level", "0");
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