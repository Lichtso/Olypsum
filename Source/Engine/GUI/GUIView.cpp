//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIView.h"

GUIView::GUIView() {
    type = GUIType_View;
}

GUIView::~GUIView() {
    for(unsigned int i = 0; i < children.size(); i ++)
        delete children[i];
}

void GUIView::addChild(GUIRect* child) {
    child->parent = this;
    children.push_back(child);
}

int GUIView::getIndexOfChild(GUIRect* child) {
    for(unsigned int i = 0; i < children.size(); i ++)
        if(children[i] == child)
            return i;
    return -1;
}

void GUIView::removeChild(unsigned int index) {
    delete children[index];
    children.erase(children.begin()+index);
}

void GUIView::updateContent() {
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIView::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}

bool GUIView::handleMouseDown(int mouseX, int mouseY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY))
            return true;
    return false;
}

bool GUIView::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY))
            return true;
    return false;
}

void GUIView::handleMouseMove(int mouseX, int mouseY) {
    if(!visible) return;
    for(int i = (int)children.size()-1; i >= 0; i --)
        children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

bool GUIView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, delta))
            return true;
    return false;
}



GUIFramedView::GUIFramedView() {
    type = GUIType_View;
    content.topColor = Color4(0.78);
    content.bottomColor = Color4(0.78);
    content.borderColor = Color4(0.78);
}

void GUIFramedView::updateContent() {
    content.width = width;
    content.height = height;
    content.drawInTexture();
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIFramedView::draw(btVector3 transform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    transform += btVector3(posX, posY, 0.0);
    content.drawOnScreen(transform, 0, 0, clipRect);
    float inset = abs(content.innerShadow);
    clipRect.minPosX += inset;
    clipRect.maxPosX -= inset;
    clipRect.minPosY += inset;
    clipRect.maxPosY -= inset;
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
}



GUIScreenView::GUIScreenView() {
    type = GUIType_ScreenView;
    modalView = NULL;
    firstResponder = NULL;
    updateContent();
}

bool GUIScreenView::getLimSize(GUIClipRect& clipRect) {
    width = prevOptionsState.videoWidth >> 1;
    height = prevOptionsState.videoHeight >> 1;
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    return (clipRect.minPosX <= clipRect.maxPosX && clipRect.minPosY <= clipRect.maxPosY);
}

void GUIScreenView::updateContent() {
    width = prevOptionsState.videoWidth >> 1;
    height = prevOptionsState.videoHeight >> 1;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::draw() {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect)) return;
    
    glEnable(GL_BLEND);
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    shaderPrograms[spriteSP]->use();
    shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
    
    btVector3 transform = btVector3(0.0, 0.0, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
    
    if(modalView) {
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        btVector3 halfSize(width, height, 0.0);
        modelMat.setIdentity();
        modelMat.setBasis(modelMat.getBasis().scaled(halfSize));
        modelMat.setOrigin(btVector3(0, 0, 0));
        shaderPrograms[spriteSP]->use();
        shaderPrograms[spriteSP]->setUniformF("alpha", 0.5);
        mainFBO.vao.draw();
        shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
        
        modalView->draw(transform, clipRect);
    }
    
    glEnable(GL_DEPTH_TEST);
}

bool GUIScreenView::handleMouseDown(int mouseX, int mouseY) {
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    if(modalView)
        return modalView->handleMouseDown(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY))
                return true;
    return false;
}

bool GUIScreenView::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return false;
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(modalView)
        return modalView->handleMouseUp(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY))
                return true;
    return false;
}

void GUIScreenView::handleMouseMove(int mouseX, int mouseY) {
    if(!visible) return;
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(modalView)
        return modalView->handleMouseMove(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

bool GUIScreenView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    mouseX -= width;
    mouseY = height-mouseY;
    
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    if(modalView)
        return modalView->handleMouseWheel(mouseX-modalView->posX, mouseY-modalView->posY, delta);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, delta))
                return true;
    return false;
}

bool GUIScreenView::handleKeyDown(SDL_keysym* key) {
    if(!firstResponder) return false;
    return firstResponder->handleKeyDown(key);
}

bool GUIScreenView::handleKeyUp(SDL_keysym* key) {
    if(!firstResponder) return false;
    return firstResponder->handleKeyUp(key);
}

void GUIScreenView::setModalView(GUIRect* modalViewB) {
    if(firstResponder) {
        firstResponder->removeFirstResponderStatus();
        firstResponder = NULL;
    }
    modalView = modalViewB;
    if(modalView) {
        modalView->parent = this;
        modalView->updateContent();
    }
}

GUIScreenView* currentScreenView = NULL;