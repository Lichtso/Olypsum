//
//  Controls.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.01.13.
//
//

#include "AppMain.h"

ControlsMangager::ControlsMangager() :grabbedObject(NULL), rotX(0), rotY(0), targetRotX(0), targetRotY(0) {
    btTransform camMat = btTransform::getIdentity();
    camMat.setOrigin(btVector3(-5, 6, -5));
    mainCam->setTransformation(camMat);
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
    //if(!keyState[SDLK_LALT])
        grabbedObject = NULL;
}

void ControlsMangager::handleMouseMove(int mouseX, int mouseY, SDL_Event& event) {
    mouseX -= screenSize[0]/2;
    mouseY -= screenSize[1]/2;
    if(mouseX == 0.0 && mouseY == 0.0) return;
    SDL_WarpMouse(screenSize[0]/screenSize[2]/2, screenSize[1]/screenSize[2]/2);
    
    if(grabbedObject && keyState[SDLK_LALT]) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(body) {
            body->setActivationState(ACTIVE_TAG);
            body->setAngularVelocity(mainCam->getTransformation().getBasis() * (btVector3(mouseY, mouseX, 0.0)*mouseSensitivity*20.0));
        }
        return;
    }
    
    targetRotX -= mouseSensitivity*mouseX;
    targetRotY = clamp(targetRotY-mouseSensitivity*mouseY, (float)-M_PI_2, (float)M_PI_2);
}

void ControlsMangager::handleMouseWheel(int mouseX, int mouseY, float delta) {
    float relGrabbDist = relGrabbPos.length();
    relGrabbPos /= relGrabbDist;
    relGrabbDist = max(1.0, relGrabbDist-delta*0.5);
    relGrabbPos = relGrabbPos*relGrabbDist;
}

void ControlsMangager::gameTick() {
    btTransform camMat = mainCam->getTransformation();
    btVector3 rotVec(targetRotX-rotX, targetRotY-rotY, 0.0);
    float speed = rotVec.length();
    rotVec /= speed;
    speed *= mouseSmoothing;
    if(speed != 0.0) {
        rotX += rotVec.x()*speed;
        rotY += rotVec.y()*speed;
        btQuaternion rot;
        rot.setEuler(rotX, rotY, 0.0);
        camMat.setRotation(rot);
    }
    
    if(keyState[SDLK_w]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(2)*-5.0*animationFactor);
    }else if(keyState[SDLK_s]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(2)*5.0*animationFactor);
    }
    if(keyState[SDLK_e]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(1)*-5.0*animationFactor);
    }else if(keyState[SDLK_q]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(1)*5.0*animationFactor);
    }
    if(keyState[SDLK_a]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(0)*-5.0*animationFactor);
    }else if(keyState[SDLK_d]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(0)*5.0*animationFactor);
    }
    
    mainCam->setTransformation(camMat);
    mainCam->gameTick();
    
    if(grabbedObject) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(!body) return;
        btVector3 velocity = mainCam->getTransformation()(relGrabbPos)-grabbedObject->getTransformation().getOrigin();
        float speed = velocity.length();
        velocity *= 1.0/speed;
        speed = fminf(speed*5.0, 10.0F);
        
        body->setActivationState(ACTIVE_TAG);
        if(!keyState[SDLK_LALT])
            body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
        body->setLinearVelocity(velocity*speed);
    }
}

ControlsMangager* controlsMangager = NULL;