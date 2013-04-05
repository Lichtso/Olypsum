//
//  Controls.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 09.01.13.
//
//

#include "AppMain.h"

ControlsMangager::ControlsMangager() :grabbedObject(NULL), rot(0, 0, 0), rotVelocity(0, 0, 0) {
    btTransform camMat = btTransform::getIdentity();
    camMat.setOrigin(btVector3(-5, 6, -5));
    mainCam->setTransformation(camMat);
}

ControlsMangager::~ControlsMangager() {
    
}

void ControlsMangager::handleMouseDown(int mouseX, int mouseY, SDL_Event& event) {
    btTransform transform = mainCam->getTransformation();
    Ray3 ray(transform.getOrigin(), transform.getBasis().getColumn(2)*-100.0);
    BaseObject* object;
    btVector3 point, normal;
    if(ray.hitTestNearest(CollisionMask_Object, object, point, normal) > 0) {
        grabbedObject = static_cast<PhysicObject*>(object);
        relGrabbPos = btVector3(0.0, 0.0, -(mainCam->getTransformation().getOrigin()
                                          -grabbedObject->getTransformation().getOrigin()).length());
    }
}

void ControlsMangager::handleMouseUp(int mouseX, int mouseY, SDL_Event& event) {
    if(!keyState[SDLK_LALT])
        grabbedObject = NULL;
}

void ControlsMangager::handleMouseMove(int mouseX, int mouseY, SDL_Event& event) {
    mouseX -= currentScreenView->width;
    mouseY -= currentScreenView->height;
    if(mouseX == 0.0 && mouseY == 0.0) return;
    SDL_WarpMouse(currentScreenView->width / prevOptionsState.videoScale, currentScreenView->height / prevOptionsState.videoScale);
    
    rotVelocity.setX(rotVelocity.x()-optionsState.mouseSensitivity*mouseX);
    rotVelocity.setY(rotVelocity.y()-optionsState.mouseSensitivity*mouseY);
}

void ControlsMangager::handleMouseWheel(int mouseX, int mouseY, float delta) {
    float relGrabbDist = relGrabbPos.length();
    relGrabbPos /= relGrabbDist;
    relGrabbDist = max(1.0, relGrabbDist-delta*0.5);
    relGrabbPos = relGrabbPos*relGrabbDist;
}

void ControlsMangager::gameTick() {
    //Update grabbedObject
    btTransform camMat = mainCam->getTransformation();
    btVector3 addRot = rotVelocity*optionsState.mouseSmoothing;
    rotVelocity -= addRot;
    if(grabbedObject) {
        btRigidBody* body = dynamic_cast<btRigidBody*>(grabbedObject->getBody());
        if(body) {
            btVector3 velocity = mainCam->getTransformation()(relGrabbPos)-grabbedObject->getTransformation().getOrigin();
            float speed = velocity.length();
            velocity *= 1.0/speed;
            speed = fminf(speed*5.0, 10.0F);
            
            if(menu.current == Menu::Name::inGame && keyState[SDLK_LALT])
                body->setAngularVelocity(mainCam->getTransformation().getBasis() * btVector3(addRot.y(), addRot.x(), 0.0) * -10.0);
            else
                body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
            body->setLinearVelocity(velocity*speed);
            body->setActivationState(ACTIVE_TAG);
        }
    }
    
    if(menu.current != Menu::Name::inGame) return;
    
    //Update Cam
    if(!keyState[SDLK_LALT]) {
        rot += addRot;
        rot.setY(clamp(rot.y(), (float)-M_PI_2, (float)M_PI_2));
        btQuaternion quaternion;
        quaternion.setEuler(rot.x(), rot.y(), 0.0);
        camMat.setRotation(quaternion);
    }
    
    if(keyState[SDLK_w]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(2)*-5.0*profiler.animationFactor);
    }else if(keyState[SDLK_s]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(2)*5.0*profiler.animationFactor);
    }
    if(keyState[SDLK_e]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(1)*-5.0*profiler.animationFactor);
    }else if(keyState[SDLK_q]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(1)*5.0*profiler.animationFactor);
    }
    if(keyState[SDLK_a]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(0)*-5.0*profiler.animationFactor);
    }else if(keyState[SDLK_d]) {
        camMat.setOrigin(camMat.getOrigin()+camMat.getBasis().getColumn(0)*5.0*profiler.animationFactor);
    }
    
    mainCam->setTransformation(camMat);
    mainCam->gameTick();
}

std::unique_ptr<ControlsMangager> controlsMangager;