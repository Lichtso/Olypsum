//
//  GUIView.cpp
//  Olypsum
//
//  Created by Alexander Meißner on 28.02.12.
//  Copyright (c) 2012 __MyCompanyName__. All rights reserved.
//

#include "GUIView.h"

GUIView::GUIView() {
    type = GUITypeView;
}

void GUIView::addChild(GUIRect* child) {
    children.push_back(child);
    child->parent = this;
}

void GUIView::updateContent() {
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIView::draw(Matrix4& parentTransform, GUIClipRect* parentClipRect) {
    if(!visible) return;
    
    GUIClipRect clipRect;
    getLimSize(parentClipRect, &clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    Matrix4 transform(parentTransform);
    transform.translate(Vector3(posX, posY, 0.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
}

bool GUIView::handleMouseDown(int mouseX, int mouseY) {
    if(mouseX < -width || mouseX > width || mouseY < -height || mouseY > height) return false;
    for(int i = (int)children.size()-1; i >= 0; i --)
        if(children[i]->handleMouseDown(mouseX-children[i]->posX, mouseY-children[i]->posY))
            return true;
    return false;
}

void GUIView::handleMouseUp(int mouseX, int mouseY) {
    for(int i = (int)children.size()-1; i >= 0; i --)
        children[i]->handleMouseUp(mouseX-children[i]->posX, mouseY-children[i]->posY);
}

void GUIView::handleMouseMove(int mouseX, int mouseY) {
    for(int i = (int)children.size()-1; i >= 0; i --)
        children[i]->handleMouseMove(mouseX-children[i]->posX, mouseY-children[i]->posY);
}



GUIScreenView::GUIScreenView() {
    type = GUITypeScreenView;
    parent = NULL;
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
}

void GUIScreenView::getLimSize(GUIClipRect* clipRect) {
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
    clipRect->minPosX = -width;
    clipRect->minPosY = -height;
    clipRect->maxPosX = width;
    clipRect->maxPosY = height;
}

void GUIScreenView::updateContent() {
    width = currentCam->viewport[2] >> 1;
    height = currentCam->viewport[3] >> 1;
    
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->updateContent();
}

void GUIScreenView::draw() {
    if(!visible) return;
    
    GUIClipRect clipRect;
    getLimSize(&clipRect);
    if(clipRect.minPosX > clipRect.maxPosX || clipRect.minPosY > clipRect.maxPosY) return;
    
    glDisable(GL_DEPTH_TEST);
    guiCam->use();
    spriteShaderProgram->setUnfiformF("light", 1.0);
    
    Matrix4 transform;
    transform.setIdentity();
    transform.translate(Vector3(0.0, 0.0, -1.0));
    for(unsigned int i = 0; i < children.size(); i ++)
        children[i]->draw(transform, &clipRect);
    
    glDisableVertexAttribArray(VERTEX_ATTRIBUTE);
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


GUIScreenView* currentScreenView;