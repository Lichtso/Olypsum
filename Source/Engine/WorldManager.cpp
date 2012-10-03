//
//  WorldManager.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 26.09.12.
//
//

#include "WorldManager.h"

WorldManager::WorldManager() {
    dynamicsWorld = NULL;
}

WorldManager::~WorldManager() {
    clearAll();
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
    if(dynamicsWorld == NULL) return;
    for(unsigned char i = 0; i < 6; i ++) {
        delete worldWallBodys[i]->getMotionState();
        delete worldWallBodys[i];
    }
    delete worldWallShape;
    delete dynamicsWorld;
    delete solver;
    delete dispatcher;
    delete collisionConfiguration;
    delete broadphase;
    dynamicsWorld = NULL;
}

void WorldManager::loadLevel() {
    Vector3 worldSize = Vector3(500, 500, 500);
    broadphase = new btAxisSweep3(worldSize.getBTVector(), (worldSize*-1.0).getBTVector());
    collisionConfiguration = new btDefaultCollisionConfiguration();
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    solver = new btSequentialImpulseConstraintSolver();
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
    dynamicsWorld->setGravity(btVector3(0, -98.1, 0));
    dynamicsWorld->setInternalTickCallback(calculatePhysicsTick);
    
    worldWallShape = new btStaticPlaneShape(btVector3(1, 0, 0), 0);
    Matrix4 transform;
    btDefaultMotionState* wallMotionState[6];
    wallMotionState[0] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0), btVector3(-worldSize.x, 0, 0)));
    wallMotionState[1] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI), btVector3( worldSize.x, 0, 0)));
    wallMotionState[2] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, -M_PI_2), btVector3(0, -worldSize.y, 0)));
    wallMotionState[3] = new btDefaultMotionState(btTransform(btQuaternion(0, 0, M_PI_2), btVector3(0,  worldSize.y, 0)));
    wallMotionState[4] = new btDefaultMotionState(btTransform(btQuaternion(-M_PI_2, 0, 0), btVector3(0, 0, -worldSize.z)));
    wallMotionState[5] = new btDefaultMotionState(btTransform(btQuaternion(M_PI_2, 0, 0), btVector3(0, 0,  worldSize.z)));
    for(unsigned char i = 0; i < 6; i ++) {
        worldWallBodys[i] = new btRigidBody(btRigidBody::btRigidBodyConstructionInfo(0, wallMotionState[i], worldWallShape, btVector3(0, 0, 0)));
        dynamicsWorld->addRigidBody(worldWallBodys[i]);
    }
    
    gameStatus = localGame;
    setMenu(inGameMenu);
}

void WorldManager::saveLevel() {
    unsigned int fileSize;
    rapidxml::xml_document<xmlUsedCharType> doc;
    char* fileData = readXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml", fileSize, false);
    char mapStr[8];
    sprintf(mapStr, "%d", levelId);
    doc.first_node("level")->first_attribute("value")->value(mapStr);
    writeXmlFile(doc, gameDataDir+"Saves/"+gameName+'/'+"Status.xml");
    delete [] fileData;
}

void WorldManager::calculate() {
    dynamicsWorld->stepSimulation(animationFactor, 4, 1.0/60.0);
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
    
    unsigned int fileSize;
    rapidxml::xml_document<xmlUsedCharType> doc;
    char* fileData = readXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml", fileSize, false);
    if(!fileData || strcmp(doc.first_node("version")->first_attribute("value")->value(), VERSION) != 0) {
        log(error_log, "Could not load saved game, because its version is outdated.");
        delete [] fileData;
        return false;
    }
    fileManager.loadPackage(doc.first_node("package")->first_attribute("value")->value());
    sscanf(doc.first_node("level")->first_attribute("value")->value(), "%d", &levelId);
    delete [] fileData;
    
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
    writeXmlFile(doc, gameDataDir+"Saves/"+name+'/'+"Status.xml");
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