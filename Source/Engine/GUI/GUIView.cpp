//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 Gamefortec. All rights reserved.
//

#include "GUIView.h"

GUIView::GUIView() {
    
}

GUIView::~GUIView() {
    for(unsigned int i = 0; i < children.size(); i ++)
        delete children[i];
}

bool GUIView::addChild(GUIRect* child) {
    if(child->parent)
        child->parent->children.erase(child->parent->children.begin()+child->parent->getIndexOfChild(child));
    
    child->parent = this;
    children.push_back(child);
    return true;
}

int GUIView::getIndexOfChild(GUIRect* child) {
    for(unsigned int i = 0; i < children.size(); i ++)
        if(children[i] == child)
            return i;
    return -1;
}

void GUIView::deleteChild(unsigned int index) {
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

bool GUIView::handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, deltaX, deltaY))
            return true;
    return false;
}



GUIFramedView::GUIFramedView() {
    content.topColor = Color4(0.78);
    content.bottomColor = Color4(0.78);
    content.borderColor = Color4(0.78);
}

void GUIFramedView::updateContent() {
    content.width = width;
    content.height = height;
    content.updateContent();
    
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



GUIScreenView::GUIScreenView() :modalView(NULL), focused(NULL) {
    updateContent();
}

GUIScreenView::~GUIScreenView() {
    if(!scriptInstance.IsEmpty())
        scriptInstance.Dispose();
}

void GUIScreenView::updateContent() {
    width = prevOptionsState.videoWidth >> 1;
    height = prevOptionsState.videoHeight >> 1;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::drawScreen() {
    if(!visible) return;
    
    width = prevOptionsState.videoWidth >> 1;
    height = prevOptionsState.videoHeight >> 1;
    GUIClipRect clipRect;
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    shaderPrograms[spriteSP]->use();
    shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
    
    btVector3 transform = btVector3(0.0, 0.0, 0.0);
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
    
    if(modalView) {
        Texture::unbind(0, GL_TEXTURE_2D);
        
        btVector3 halfSize(width, height, 0.0);
        modelMat.setIdentity();
        modelMat.setBasis(modelMat.getBasis().scaled(halfSize));
        modelMat.setOrigin(btVector3(0, 0, 0));
        shaderPrograms[spriteSP]->use();
        shaderPrograms[spriteSP]->setUniformF("alpha", 0.5);
        rectVAO.draw();
        shaderPrograms[spriteSP]->setUniformF("alpha", 1.0);
        
        modalView->draw(transform, clipRect);
    }
    
    glEnable(GL_DEPTH_TEST);
    profiler.leaveSection("GUI Draw");
}

bool GUIScreenView::handleMouseDown(int mouseX, int mouseY) {
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
    
    if(modalView)
        return modalView->handleMouseMove(mouseX-modalView->posX, mouseY-modalView->posY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

bool GUIScreenView::handleMouseWheel(int mouseX, int mouseY, float deltaX, float deltaY) {
    if(!visible || mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    
    if(modalView)
        return modalView->handleMouseWheel(mouseX-modalView->posX, mouseY-modalView->posY, deltaX, deltaY);
    else
        for(int i = (int)children.size()-1; i >= 0; i --)
            if(children[i]->handleMouseWheel(mouseX-children[i]->posX, mouseY-children[i]->posY, deltaX, deltaY))
                return true;
    return false;
}

bool GUIScreenView::handleKeyDown(SDL_keysym* key) {
    if(!focused) return false;
    return focused->handleKeyDown(key);
}

bool GUIScreenView::handleKeyUp(SDL_keysym* key) {
    if(!focused) return false;
    return focused->handleKeyUp(key);
}

void GUIScreenView::setModalView(GUIRect* modalViewB) {
    if(focused)
        focused->setFocus(false);
    if(modalView)
        deleteChild(getIndexOfChild(modalView));
    modalView = modalViewB;
    if(!modalView) return;
    if(modalView->parent != this)
        addChild(modalView);
    modalView->visible = true;
    modalView->updateContent();
}