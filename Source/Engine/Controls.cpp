//
//  Controls.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.01.13.
//
//

#include "AppMain.h"

ControlsMangager::ControlsMangager() :grabbedObject(NULL) {
    
}

void ControlsMangager::handleMouseDown(int mouseX, int mouseY, SDL_Event& event) {
    Ray3 ray = mainCam->getRayAt(2.0*mouseX/screenSize[0]-1.0, 2.0*mouseY/screenSize[1]-1.0);
    btCollisionWorld::ClosestRayResultCallback rayCallback(ray.origin, ray.direction*100.0);
    rayCallback.m_collisionFilterGroup = 0xFFFF;
    rayCallback.m_collisionFilterMask = CollisionMask_Object;
    objectManager.physicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
    
    if(rayCallback.hasHit()) {
        grabbedObject = static_cast<PhysicObject*>(rayCallback.m_collisionObject->getUserPointer());
        relGrabbPos = mainCam->getTransformation().inverse()(grabbedObject->getTransformation().getOrigin());
        relGrabbDist = (relGrabbPos.length()-mainCam->near)*0.6;
    }
}

void ControlsMangager::handleMouseUp(int mouseX, int mouseY, SDL_Event& event) {
    grabbedObject = NULL;
}

void ControlsMangager::handleMouseMove(int mouseX, int mouseY, SDL_Event& event) {
    if(keyState[SDLK_LMETA]) {
        btQuaternion rot;
        rot.setEuler(-((float)event.button.x/screenSize[0]-0.5)*M_PI*2.0, -((float)event.button.y/screenSize[1]-0.5)*M_PI*2.0, 0);
        btTransform camMat;
        camMat.setIdentity();
        camMat.setRotation(rot);
        camMat.setOrigin(camMat.getBasis()*btVector3(0, 0, 3));
        mainCam->setTransformation(camMat);
        mainCam->gameTick();
    }
    
    if(grabbedObject) {
        Ray3 ray = mainCam->getRelativeRayAt(2.0*mouseX/screenSize[0]-1.0, 2.0*mouseY/screenSize[1]-1.0);
        relGrabbPos = ray.origin+ray.direction*relGrabbDist;
    }
}

void ControlsMangager::handleMouseWheel(int mouseX, int mouseY, float delta) {
    relGrabbDist += delta*0.5;
    Ray3 ray = mainCam->getRelativeRayAt(2.0*mouseX/screenSize[0]-1.0, 2.0*mouseY/screenSize[1]-1.0);
    relGrabbPos = ray.origin+ray.direction*relGrabbDist;
}

void ControlsMangager::gameTick() {
    if(grabbedObject) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(!body) return;
        btVector3 velocity = mainCam->getTransformation()(relGrabbPos)-grabbedObject->getTransformation().getOrigin();
        float speed = velocity.length();
        velocity *= 1.0/speed;
        speed = fminf(speed, 5.0F);
        
        body->activate();
        body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
        body->setLinearVelocity(velocity*speed);
    }
}

ControlsMangager* controlsMangager = NULL;