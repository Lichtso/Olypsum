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
    for(int i = 0; i < consoleMessages.size(); i ++) {
        
    }
}

void ControlsMangager::consoleAdd(const std::string& message, float duration) {
    ConsoleEntry entry;
    entry.message = message;
    entry.timeLeft = duration;
    consoleMessages.push_back(entry);
}

void ControlsMangager::handleMouseDown(int mouseX, int mouseY, SDL_Event& event) {
    btTransform transform = mainCam->getTransformation();
    btCollisionWorld::ClosestRayResultCallback rayCallback(transform.getOrigin(), transform.getOrigin()-transform.getBasis().getColumn(2)*100.0);
    rayCallback.m_collisionFilterGroup = 0xFFFF;
    rayCallback.m_collisionFilterMask = CollisionMask_Object;
    objectManager.physicsWorld->rayTest(rayCallback.m_rayFromWorld, rayCallback.m_rayToWorld, rayCallback);
    
    if(rayCallback.hasHit()) {
        grabbedObject = static_cast<PhysicObject*>(rayCallback.m_collisionObject->getUserPointer());
        //relGrabbPos = mainCam->getTransformation().inverse()(grabbedObject->getTransformation().getOrigin());
        relGrabbPos = btVector3(0.0, 0.0, -(mainCam->getTransformation().getOrigin()
                                          -grabbedObject->getTransformation().getOrigin()).length());
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
    //Update GUI
    if(currentMenu == inGameMenu) {
        SDL_ShowCursor(0);
        GUIView* view = static_cast<GUIView*>(currentScreenView->children[1]);
        int posY = view->height;
        for(int i = 0; i < consoleMessages.size(); i ++) {
            GUILabel* label;
            consoleMessages[i].timeLeft -= animationFactor;
            if(consoleMessages[i].timeLeft < 0.0) {
                view->removeChild(i);
                consoleMessages.erase(consoleMessages.begin()+i);
                i --;
                continue;
            }
            if(i < view->children.size()) {
                label = static_cast<GUILabel*>(view->children[i]);
                label->color.a = fmin(1, consoleMessages[i].timeLeft);
                float fallSpeed = posY-label->height;
                fallSpeed = (fallSpeed-label->posY)*animationFactor*5.0;
                label->posY += (fallSpeed > 0.0 && fallSpeed < 1.0) ? 1.0 : fallSpeed;
                posY -= label->height*2.2;
                if(label->text == consoleMessages[i].message)
                    continue;
            }else{
                label = new GUILabel();
                view->addChild(label);
            }
            label->width = view->width;
            label->fontHeight = currentScreenView->height*0.05;
            label->textAlign = GUITextAlign_Left;
            label->sizeAlignment = GUISizeAlignment_Height;
            label->color = Color4(1.0);
            label->text = consoleMessages[i].message;
            label->updateContent();
            label->posX = 0.0;
            label->posY = -view->height*2.0;
        }
        
        for(int i = consoleMessages.size(); i < view->children.size(); i ++)
            view->removeChild(i);
    }else
        SDL_ShowCursor(1);
    
    //Calculate Screen Blur
    if(optionsState.screenBlurFactor > -1.0) {
        float speed = animationFactor*20.0;
        if(currentMenu == inGameMenu) {
            optionsState.screenBlurFactor -= min(optionsState.screenBlurFactor*speed, speed);
            if(optionsState.screenBlurFactor < 0.01) optionsState.screenBlurFactor = 0.0;
        }else
            optionsState.screenBlurFactor += min((10-optionsState.screenBlurFactor)*speed, speed);
    }
    
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
            
            if(currentMenu == inGameMenu && keyState[SDLK_LALT])
                body->setAngularVelocity(mainCam->getTransformation().getBasis() * btVector3(addRot.y(), addRot.x(), 0.0) * -10.0);
            else
                body->setAngularVelocity(btVector3(0.0, 0.0, 0.0));
            body->setLinearVelocity(velocity*speed);
            body->setActivationState(ACTIVE_TAG);
        }
    }
    
    if(currentMenu != inGameMenu) return;
    
    //Update Cam
    if(!keyState[SDLK_LALT]) {
        rot += addRot;
        rot.setY(clamp(rot.y(), (float)-M_PI_2, (float)M_PI_2));
        btQuaternion quaternion;
        quaternion.setEuler(rot.x(), rot.y(), 0.0);
        camMat.setRotation(quaternion);
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
}

std::unique_ptr<ControlsMangager> controlsMangager;