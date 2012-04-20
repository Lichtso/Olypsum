//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
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

void GUIView::removeChild(unsigned int index) {
    delete children[index];
    children.erase(children.begin()+index);
}

void GUIView::updateContent() {
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIView::draw(Matrix4& parentTransform, GUIClipRect& parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect, parentClipRect)) return;
    
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX, posY, 0.0));
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

void GUIView::handleMouseUp(int mouseX, int mouseY) {
    if(!visible) return;
    for(int i = (int)children.size()-1; i >= 0; i --)
        children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY);
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



GUIScreenView::GUIScreenView() {
    type = GUIType_ScreenView;
    parent = NULL;
    firstResponder = NULL;
    width = videoInfo->current_w >> 1;
    height = videoInfo->current_h >> 1;
}

bool GUIScreenView::getLimSize(GUIClipRect& clipRect) {
    width = videoInfo->current_w >> 1;
    height = videoInfo->current_h >> 1;
    clipRect.minPosX = -width;
    clipRect.minPosY = -height;
    clipRect.maxPosX = width;
    clipRect.maxPosY = height;
    return (clipRect.minPosX <= clipRect.maxPosX && clipRect.minPosY <= clipRect.maxPosY);
}

void GUIScreenView::updateContent() {
    width = videoInfo->current_w >> 1;
    height = videoInfo->current_h >> 1;
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::draw() {
    if(!visible) return;
    
    GUIClipRect clipRect;
    if(!getLimSize(clipRect)) return;
    
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    spriteShaderProgram->use();
    spriteShaderProgram->setUniformF("light", 1.0);
    
    Matrix4 transform;
    transform.setIdentity();
    transform.translate(Vector3(0.0, 0.0, -1.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, clipRect);
    
    glDisableVertexAttribArray(POSITION_ATTRIBUTE);
    glDisableVertexAttribArray(TEXTURE_COORD_ATTRIBUTE);
    glEnable(GL_DEPTH_TEST);
}

bool GUIScreenView::handleMouseDown(int mouseX, int mouseY) {
    return GUIView::handleMouseDown(mouseX-width, height-mouseY);
}

void GUIScreenView::handleMouseUp(int mouseX, int mouseY) {
    GUIView::handleMouseUp(mouseX-width, height-mouseY);
}

void GUIScreenView::handleMouseMove(int mouseX, int mouseY) {
    GUIView::handleMouseMove(mouseX-width, height-mouseY);
}

bool GUIScreenView::handleMouseWheel(int mouseX, int mouseY, float delta) {
    return GUIView::handleMouseWheel(mouseX-width, height-mouseY, delta);
}

bool GUIScreenView::handleKeyDown(SDL_keysym* key) {
    if(!firstResponder) return false;
    firstResponder->handleKeyDown(key);
    return true;
}

bool GUIScreenView::handleKeyUp(SDL_keysym* key) {
    if(!firstResponder) return false;
    firstResponder->handleKeyUp(key);
    return true;
}


GUIScreenView* currentScreenView;