//
//  BasicObjects.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 14.10.12.
//
//

#import "WorldManager.h"

ObjectBase::ObjectBase() {
    objectManager.objects.push_back(this);
}

ObjectBase::~ObjectBase() {
    for(unsigned int i = 0; i < objectManager.objects.size(); i ++) {
        if(objectManager.objects[i] != this) continue;
        objectManager.objects.erase(objectManager.objects.begin()+i);
        return;
    }
}



PhysicObject::PhysicObject() {
    
}

void PhysicObject::physicsTick() {
    
}

void PhysicObject::handleCollision(btPersistentManifold* contactManifold, PhysicObject* b) {
    
}



GraphicObject::GraphicObject() {
    
}

bool GraphicObject::isInFrustum(btCollisionObject* body) {
    FrustumCullingCallback callback;
    worldManager.physicsWorld->contactPairTest(currentCam->frustumBody, body, callback);
    return callback.inView;
}



ModelObject::ModelObject(std::shared_ptr<Model> modelB) :model(modelB) {
    if(model->skeleton) {
        skeletonPose = new SkeletonPose(model->skeleton);
        skeletonPose->calculate();
    }
}

ModelObject::~ModelObject() {
    if(skeletonPose)
        delete skeletonPose;
}

void ModelObject::draw() {
    modelMat = getTransformation();
    model->draw(this);
}

void ModelObject::drawMesh(Mesh* mesh) {
    modelMat = getTransformation();
    mesh->draw(this);
}

void ModelObject::prepareShaderProgram(Mesh* mesh) {
    if(!lightManager.currentShadowLight) {
        char index = (mesh->heightMap) ? 1 : 0;
        if(skeletonPose) index |= 2;
        if(mesh->transparent && blendingQuality > 0) index |= 4;
        ShaderProgramNames lookup[] = {
            solidGeometrySP, solidBumpGeometrySP, skeletalGeometrySP, skeletalBumpGeometrySP,
            glassGeometrySP, glassBumpGeometrySP, glassSkeletalGeometrySP, glassSkeletalBumpGeometrySP
        };
        shaderPrograms[lookup[index]]->use();
    }
    
    currentShaderProgram->setUniformF("discardDensity", 1.0);
    if(skeletonPose)
        currentShaderProgram->setUniformMatrix4("jointMats", skeletonPose->mats, skeletonPose->skeleton->bones.size());
}



ObjectManager::~ObjectManager() {
    clear();
}

void ObjectManager::clear() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        delete objects[i];
    objects.clear();
}

void ObjectManager::gameTick() {
    for(unsigned int i = 0; i < objects.size(); i ++)
        objects[i]->gameTick();
}

void ObjectManager::physicsTick() {
    for(unsigned int i = 0; i < objects.size(); i ++) {
        PhysicObject* object = dynamic_cast<PhysicObject*>(objects[i]);
        if(!object) continue;
        object->physicsTick();
    }
    
    btDispatcher* dispatcher = worldManager.physicsWorld->getDispatcher();
    unsigned int numManifolds = dispatcher->getNumManifolds();
	for(unsigned int i = 0; i < numManifolds; i ++) {
		btPersistentManifold* contactManifold =  dispatcher->getManifoldByIndexInternal(i);
		void *objectA = static_cast<const btCollisionObject*>(contactManifold->getBody0())->getUserPointer(),
        *objectB = static_cast<const btCollisionObject*>(contactManifold->getBody1())->getUserPointer();
        
        PhysicObject *userObjectA = static_cast<PhysicObject*>(objectA),
        *userObjectB = static_cast<PhysicObject*>(objectB);
        
        if(userObjectA) userObjectA->handleCollision(contactManifold, userObjectB);
        if(userObjectB) userObjectB->handleCollision(contactManifold, userObjectA);
	}
}

void ObjectManager::draw() {
    for(unsigned int i = 0; i < objects.size(); i ++) {
        GraphicObject* object = dynamic_cast<GraphicObject*>(objects[i]);
        if(object)
            object->draw();
    }
}

ObjectManager objectManager;