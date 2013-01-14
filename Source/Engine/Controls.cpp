//
//  Controls.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.01.13.
//
//

#include "AppMain.h"

ControlsMangager::ControlsMangager() :grabbedObject(NULL), rotX(0), rotY(0) {
    
}

void ControlsMangager::handleMouseDown(int mouseX, int mouseY, SDL_Event& event) {
    btTransform transform = mainCam->getTransformation();
    btCollisionWorld::ClosestRayResultCallback rayCallback(transform.getOrigin(), transform.getOrigin()-transform.getBasis().getColumn(2)*100.0);
    rayCallback.m_collisionFilterGroup = 0xFFFF;
    rayCallback.m_collisionFilterMask = CollisionMask_Object;
    objectManager.physicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
    
    if(rayCallback.hasHit()) {
        grabbedObject = static_cast<PhysicObject*>(rayCallback.m_collisionObject->getUserPointer());
        relGrabbPos = mainCam->getTransformation().inverse()(grabbedObject->getTransformation().getOrigin());
    }
}

void ControlsMangager::handleMouseUp(int mouseX, int mouseY, SDL_Event& event) {
    grabbedObject = NULL;
}

void ControlsMangager::handleMouseMove(int mouseX, int mouseY, SDL_Event& event) {
    mouseX -= screenSize[0]/2;
    mouseY -= screenSize[1]/2;
    
    if(grabbedObject && keyState[SDLK_LALT]) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(!body) return;
        
        body->activate();
        body->setAngularVelocity(mainCam->getTransformation().getBasis() * btVector3(0.1*mouseY, 0.1*mouseX, 0.0));
        return;
    }
    
    rotX -= 0.005*mouseX;
    rotY -= 0.005*mouseY;
    
    btQuaternion rot;
    rot.setEuler(rotX, rotY, 0.0);
    btTransform camMat;
    camMat.setIdentity();
    camMat.setRotation(rot);
    camMat.setOrigin(btVector3(-5, 6, -5));
    mainCam->setTransformation(camMat);
    mainCam->gameTick();
}

void ControlsMangager::handleMouseWheel(int mouseX, int mouseY, float delta) {
    float relGrabbDist = relGrabbPos.length();
    relGrabbPos /= relGrabbDist;
    relGrabbDist -= delta*0.5;
    relGrabbPos = relGrabbPos*relGrabbDist;
}

void ControlsMangager::gameTick() {
    if(grabbedObject) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(!body) return;
        btVector3 velocity = mainCam->getTransformation()(relGrabbPos)-grabbedObject->getTransformation().getOrigin();
        float speed = velocity.length();
        velocity *= 1.0/speed;
        speed = fminf(speed*5.0, 10.0F);
        
        body->activate();
        if(!keyState[SDLK_LALT])
            body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
        body->setLinearVelocity(velocity*speed);
    }
    
    SDL_WarpMouse(screenSize[0]/screenSize[2]/2, screenSize[1]/screenSize[2]/2);
}

ControlsMangager* controlsMangager = NULL;